//
//  Copyright 2018-2020 Picovoice Inc.
//  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
//  file accompanying this source.
//  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
//  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
//  specific language governing permissions and limitations under the License.
//

import AVFoundation
import PvPorcupine

public enum PorcupineManagerError: Error {
    case invalidArgument
    case io
    case outOfMemory
    case recordingDenied
}

/// High-level iOS binding for Porcupine wake word engine. It handles recording audio from microphone, processes it in real-time using Porcupine, and notifies the
/// client when any of the given keywords are detected.
public class PorcupineManager {
    private var onDetection: ((Int32) -> Void)?
    
    private var porcupine: OpaquePointer?
    
    private let audioInputEngine: AudioInputEngine
    
    private var isListening = false
    
    /// Constructor.
    ///
    /// - Parameters:
    ///   - modelPath: Absolute path to file containing model parameters.
    ///   - keywordPaths: Absolute paths to keyword model files.
    ///   - sensitivities: Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher sensitivity results in fewer misses at
    ///   the cost of increasing the false alarm rate.
    ///   - onDetection: It is invoked upon detection of any of the keywords.
    /// - Throws: PorcupineManagerError
    public init(modelPath: String, keywordPaths: [String], sensitivities: [Float32], onDetection: ((Int32) -> Void)?) throws {
        self.onDetection = onDetection
        
        self.audioInputEngine = AudioInputEngine()
        
        audioInputEngine.audioInput = { [weak self] audio in
            
            guard let `self` = self else {
                return
            }
            
            var result: Int32 = -1
            pv_porcupine_process(self.porcupine, audio, &result)
            if result >= 0 {
                DispatchQueue.main.async {
                    self.onDetection?(result)
                }
            }
        }
        
        let status = pv_porcupine_init(
            modelPath,
            Int32(keywordPaths.count),
            keywordPaths.map { UnsafePointer(strdup($0)) },
            sensitivities,
            &porcupine)
        try checkStatus(status)
    }
    
    /// Constructor.
    ///
    /// - Parameters:
    ///   - modelPath: Absolute path to file containing model parameters.
    ///   - keywordPath: Absolute path to keyword model file.
    ///   - sensitivity: Sensitivity for detecting keyword. Should be a floating point number within [0, 1]. A higher sensitivity results in fewer misses at the
    ///   cost of increasing false alarm rate..
    ///   - onDetection: It is invoked upon detection of the keyword.
    /// - Throws: PorcupineManagerError
    public convenience init(modelPath: String, keywordPath: String, sensitivity: Float32, onDetection: ((Int32) -> Void)?) throws {
        try self.init(modelPath: modelPath, keywordPaths: [keywordPath], sensitivities: [sensitivity], onDetection: onDetection)
    }
    
    deinit {
        if isListening {
            stop()
        }
        
        pv_porcupine_delete(porcupine)
        porcupine = nil
    }
    
    ///  Starts recording audio from the microphone and monitors it for the utterances of the given set of keywords.
    ///
    /// - Throws: AVAudioSession, AVAudioEngine errors. Additionally PorcupineManagerPermissionError if
    ///           microphone permission is not granted.
    public func start() throws {
        let audioSession = AVAudioSession.sharedInstance()
        // Only check if it's denied, permission will be automatically asked.
        if audioSession.recordPermission == .denied {
            throw PorcupineManagerError.recordingDenied
        }
        
        guard !isListening else {
            return
        }
        
        try audioSession.setCategory(AVAudioSession.Category.playAndRecord, options: [.mixWithOthers, .defaultToSpeaker, .allowBluetooth])
        try audioSession.setMode(AVAudioSession.Mode.voiceChat)
        try audioSession.setActive(true, options: .notifyOthersOnDeactivation)
        
        try audioInputEngine.start()
        
        isListening = true
    }
    
    /// Stop listening for wake words.
    public func stop() {
        guard isListening else {
            return
        }
        
        audioInputEngine.stop()
        
        do {
            try AVAudioSession.sharedInstance().setActive(false)
        }
        catch {
            NSLog("Unable to explicitly deactivate AVAudioSession: \(error)");
        }
        
        isListening = false
    }
    
    private func checkStatus(_ status: pv_status_t) throws {
        switch status {
        case PV_STATUS_IO_ERROR:
            throw PorcupineManagerError.io
        case PV_STATUS_OUT_OF_MEMORY:
            throw PorcupineManagerError.outOfMemory
        case PV_STATUS_INVALID_ARGUMENT:
            throw PorcupineManagerError.invalidArgument
        default:
            return
        }
    }
}

private class AudioInputEngine {
    private let numBuffers = 3
    private var audioQueue: AudioQueueRef?
    
    var audioInput: ((UnsafePointer<Int16>) -> Void)?
    
    func start() throws {
        var format = AudioStreamBasicDescription(
            mSampleRate: Float64(pv_sample_rate()),
            mFormatID: kAudioFormatLinearPCM,
            mFormatFlags: kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked,
            mBytesPerPacket: 2,
            mFramesPerPacket: 1,
            mBytesPerFrame: 2,
            mChannelsPerFrame: 1,
            mBitsPerChannel: 16,
            mReserved: 0)
        let userData = UnsafeMutableRawPointer(Unmanaged.passUnretained(self).toOpaque())
        AudioQueueNewInput(&format, createAudioQueueCallback(), userData, nil, nil, 0, &audioQueue)
        
        guard let queue = audioQueue else {
            return
        }
        
        let bufferSize = UInt32(pv_porcupine_frame_length()) * 2
        for _ in 0..<numBuffers {
            var bufferRef: AudioQueueBufferRef? = nil
            AudioQueueAllocateBuffer(queue, bufferSize, &bufferRef)
            if let buffer = bufferRef {
                AudioQueueEnqueueBuffer(queue, buffer, 0, nil)
            }
        }
        
        AudioQueueStart(queue, nil)
    }
    
    func stop() {
        guard let audioQueue = audioQueue else {
            return
        }
        AudioQueueStop(audioQueue, true)
        AudioQueueDispose(audioQueue, false)
    }
    
    private func createAudioQueueCallback() -> AudioQueueInputCallback {
        return { userData, queue, bufferRef, startTimeRef, numPackets, packetDescriptions in
            
            // `self` is passed in as userData in the audio queue callback.
            guard let userData = userData else {
                return
            }
            let `self` = Unmanaged<AudioInputEngine>.fromOpaque(userData).takeUnretainedValue()
            
            let pcm = bufferRef.pointee.mAudioData.assumingMemoryBound(to: Int16.self)
            
            if let audioInput = self.audioInput {
                audioInput(pcm)
            }
            
            AudioQueueEnqueueBuffer(queue, bufferRef, 0, nil)
        }
    }
}
