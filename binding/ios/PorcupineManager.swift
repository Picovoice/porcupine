//
// Copyright 2018 Picovoice Inc.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

import AVFoundation
import pv_porcupine

/// Errors corresponding to different status codes returned by Porcupine library.
public enum PorcupineManagerError: Error {
    case outOfMemory
    case io
    case invalidArgument
}

public enum PorcupineManagerPermissionError: Error {
    case recordingDenied
}

public struct WakeWordConfiguration {
    let name: String
    let filePath: String
    let sensitivity: Float

    /// Initializer for the wake word configuration.
    ///
    /// - Parameters:
    ///   - name: The name to use to help identify this configuration.
    ///   - filePath: Absolute path to keyword file containing hyper-parameters (ppn).
    ///   - sensitivity: Sensitivity parameter. A higher sensitivity value lowers miss rate at the cost of increased
    ///     false alarm rate. For more information regarding this parameter refer to 'include/pv_porcupine.h'.
    public init(name: String, filePath: String, sensitivity: Float) {
        self.name = name
        self.filePath = filePath
        self.sensitivity = sensitivity
    }
}

/// iOS / watchOS binding for Picovoice's wake word detection engine (aka Porcupine).
public class PorcupineManager {

    private var porcupine: OpaquePointer?

    private let audioInputEngine: AudioInputEngine

    public let modelFilePath: String
    public let wakeWordConfiguration: [WakeWordConfiguration]

    /// Callback when wake keyword is detected. This will be invoked on main queue.
    public var onDetection: ((WakeWordConfiguration) -> Void)?

    /// Whether current manager is listening to audio input.
    public private(set) var isListening = false

    private var shouldBeListening: Bool = false

    /// Initializer for multiple keywords detection.
    ///
    /// - Parameters:
    ///   - modelFilePath: Absolute path to file containing model parameters.
    ///   - wakeKeywordConfigurations: Keyword configurations to use.
    ///   - onDetection: Detection handler to call after wake word detection. The handler is executed on main thread.
    /// - Throws: PorcupineManagerError
    public init(modelFilePath: String, wakeKeywordConfigurations: [WakeWordConfiguration], onDetection: ((WakeWordConfiguration) -> Void)?) throws {

        self.modelFilePath = modelFilePath
        self.wakeWordConfiguration = wakeKeywordConfigurations
        self.onDetection = onDetection

        #if os(iOS) || os(macOS)
        self.audioInputEngine = AudioInputEngine_AudioQueue()
        #elseif os(watchOS)
        self.audioInputEngine = AudioInputEngine_AVAudioEngine()
        #endif

        audioInputEngine.audioInput = { [weak self] audio in

            guard let `self` = self else {
                return
            }

            var result: Int32 = -1

            pv_porcupine_multiple_keywords_process(self.porcupine, audio, &result)
            if result >= 0 {
                let index = Int(result)
                let keyword = self.wakeWordConfiguration[index]
                DispatchQueue.main.async {
                    self.onDetection?(keyword)
                }
            }
        }

        let keywordFilePaths = wakeKeywordConfigurations.map { $0.filePath }
        let sensitivities = wakeKeywordConfigurations.map { $0.sensitivity }

        let status = pv_porcupine_multiple_keywords_init(
            modelFilePath,
            Int32(keywordFilePaths.count),
            keywordFilePaths.map { UnsafePointer(strdup($0)) },
            sensitivities,
            &porcupine)
        try checkInitStatus(status)

        let audioSession = AVAudioSession.sharedInstance()

        NotificationCenter.default.addObserver(self, selector: #selector(onAudioSessionInterruption(_:)), name: .AVAudioSessionInterruption, object: audioSession)
    }

    /// Initializer for single keyword detection.
    ///
    /// - Parameters:
    ///   - modelFilePath: Absolute path to file containing model parameters.
    ///   - wakeKeywordConfiguration: Keyword configuration to use.
    ///   - onDetection: Detection handler to call after wake word detection. The handler is executed on main thread.
    /// - Throws: PorcupineManagerError
    public convenience init(modelFilePath: String, wakeKeywordConfiguration: WakeWordConfiguration, onDetection: ((WakeWordConfiguration) -> Void)?) throws {
        try self.init(modelFilePath: modelFilePath, wakeKeywordConfigurations: [wakeKeywordConfiguration], onDetection: onDetection)
    }

    deinit {
        if isListening {
            stopListening()
        }
        pv_porcupine_delete(porcupine)
        porcupine = nil
    }

    /// Start listening for configured wake words.
    ///
    /// - Throws: AVAudioSession, AVAudioEngine errors. Additionally PorcupineManagerPermissionError if
    ///           microphone permission is not granted.
    public func startListening() throws {

        shouldBeListening = true

        let audioSession = AVAudioSession.sharedInstance()
        // Only check if it's denied, permission will be automatically asked.
        if audioSession.recordPermission() == .denied {
            throw PorcupineManagerPermissionError.recordingDenied
        }

        guard !isListening else {
            return
        }

        try audioSession.setCategory(AVAudioSessionCategoryRecord)
        try audioSession.setMode(AVAudioSessionModeMeasurement)
        try audioSession.setActive(true, with: .notifyOthersOnDeactivation)

        try audioInputEngine.start()

        isListening = true
    }

    /// Stop listening for wake words.
    public func stopListening() {

        shouldBeListening = false

        guard isListening else {
            return
        }

        audioInputEngine.stop()
        isListening = false
    }

    // MARK: - Private

    private func checkInitStatus(_ status: pv_status_t) throws {
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

    @objc private func onAudioSessionInterruption(_ notification: Notification) {

        guard let userInfo = notification.userInfo,
            let typeValue = userInfo[AVAudioSessionInterruptionTypeKey] as? UInt,
            let type = AVAudioSessionInterruptionType(rawValue: typeValue) else {
                return
        }

        if type == .began {
            audioInputEngine.pause()
        } else if type == .ended {
            // Interruption options are ignored. AudioEngine should be restarted
            // unless PorcupineManager is told to stop listening.
            guard let _ = userInfo[AVAudioSessionInterruptionOptionKey] as? UInt else {
                return
            }
            if shouldBeListening {
                audioInputEngine.unpause()
            }
        }
    }
}

private protocol AudioInputEngine: AnyObject {

    var audioInput: ((UnsafePointer<Int16>) -> Void)? { get set }

    func start() throws
    func stop()

    func pause()
    func unpause()
}

// 2 different audio input engine. watchOS requires the use of AVAudioEngine.
// However, AVAudioEngine has limitation of input latency only going as low as 100ms.

#if os(iOS) || os(macOS)
private class AudioInputEngine_AudioQueue: AudioInputEngine {

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

    func pause() {
        guard let audioQueue = audioQueue else {
            return
        }
        AudioQueuePause(audioQueue)
    }

    func unpause() {
        guard let audioQueue = audioQueue else {
            return
        }
        AudioQueueFlush(audioQueue)
        AudioQueueStart(audioQueue, nil)
    }

    private func createAudioQueueCallback() -> AudioQueueInputCallback {
        return { userData, queue, bufferRef, startTimeRef, numPackets, packetDescriptions in

            // `self` is passed in as userData in the audio queue callback.
            guard let userData = userData else {
                return
            }
            let `self` = Unmanaged<AudioInputEngine_AudioQueue>.fromOpaque(userData).takeUnretainedValue()

            let pcm = bufferRef.pointee.mAudioData.assumingMemoryBound(to: Int16.self)

            if let audioInput = self.audioInput {
                audioInput(pcm)
            }

            AudioQueueEnqueueBuffer(queue, bufferRef, 0, nil)
        }
    }
}
#endif

#if os(watchOS)
private class AudioInputEngine_AVAudioEngine: AudioInputEngine {

    private let busNumber = 0
    private lazy var audioEngine = AVAudioEngine()

    var audioInput: ((UnsafePointer<Int16>) -> Void)?

    func start() throws {
        let inputNode = audioEngine.inputNode

        let frameLength = UInt32(pv_porcupine_frame_length())
        let sampleRate = Double(pv_sample_rate())

        let recordingFormat = inputNode.inputFormat(forBus: busNumber)

        let duration = Double(frameLength) / sampleRate

        // Buffer of 4, porcupine reads audio in 32ms frames.
        // Default AVAudioEngine input node will only go as low as 4410 for buffer size (100ms).
        // So as a workaround, read 4 at once so it's 4 * 32ms, which is bigger than 100 ms.
        let numberOfBuffer: UInt32 = 4
        let frameCapacity = numberOfBuffer * frameLength
        let bufferSize = AVAudioFrameCount(duration * recordingFormat.sampleRate) * numberOfBuffer

        // Format is hardcoded. It can only be nil if channels > 2 according to documentation. Assume non-nil.
        let picoFormat = AVAudioFormat(commonFormat: .pcmFormatInt16, sampleRate: sampleRate, channels: 1, interleaved: true)!
        // Output format is hardcoded. Converter will only be nil if the conversion is not possible.
        let converter = AVAudioConverter(from: recordingFormat, to: picoFormat)
        assert(converter != nil, "Unable to convert \(recordingFormat) to Porcupine's expected format.")

        inputNode.installTap(onBus: busNumber, bufferSize: bufferSize, format: recordingFormat) { [weak self] buffer, _ in

            guard let `self` = self,
                let picoBuffer = AVAudioPCMBuffer(pcmFormat: picoFormat, frameCapacity: frameCapacity),
                let audioInput = self.audioInput else {
                    return
            }

            let input: AVAudioConverterInputBlock = { inNumPackets, outStatus in
                outStatus.pointee = AVAudioConverterInputStatus.haveData
                return buffer
            }

            converter?.convert(to: picoBuffer, error: nil, withInputFrom: input)
            converter?.reset()

            guard let pcm = picoBuffer.int16ChannelData?.pointee else {
                return
            }

            for i in 0..<Int(numberOfBuffer) {
                let offsetPointer = pcm + i * Int(frameLength)
                audioInput(offsetPointer)
            }
        }

        try audioEngine.start()
    }

    func stop() {
        audioEngine.inputNode.removeTap(onBus: busNumber)
        audioEngine.stop()
    }

    func pause() {
        audioEngine.pause()
    }

    func unpause() {
        try? audioEngine.start()
    }
}
#endif
