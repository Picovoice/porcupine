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

import Foundation

import AudioToolbox
import pv_porcupine

/// Errors corresponding to different status codes returned by Porcupine library.
public enum PorcupineManagerError: Error {
    case outOfMemory
    case io
    case invalidArgument
}

/// iOS binding for Picovoice's wake word detection engine (aka Porcupine).
public class PorcupineManager {

    private let numBuffers = 5

    private var audioQueue: AudioQueueRef?
    private var porcupine: OpaquePointer?

    public let modelFilePath: String
    public let wakeKeywordConfigurations: [WakeKeywordConfiguration]
    public var onDetection: ((WakeKeywordConfiguration) -> Void)?
    public private(set) var isListening = false

    /// Initializer for multiple keywords detection.
    ///
    /// - Parameters:
    ///   - modelFilePath: Absolute path to file containing model parameters.
    ///   - wakeKeywordConfigurations: Keyword configurations to use.
    ///   - onDetection: Detection handler to call after wake word detection. The handler is executed on main thread.
    public init(modelFilePath: String, wakeKeywordConfigurations: [WakeKeywordConfiguration], onDetection: ((WakeKeywordConfiguration) -> Void)?) throws {

        self.modelFilePath = modelFilePath
        self.wakeKeywordConfigurations = wakeKeywordConfigurations
        self.onDetection = onDetection

        let keywordFilePaths = wakeKeywordConfigurations.map { $0.filePath }
        let sensitivities = wakeKeywordConfigurations.map { $0.sensitivity }

        let status = pv_porcupine_multiple_keywords_init(
            modelFilePath,
            Int32(keywordFilePaths.count),
            keywordFilePaths.map{ UnsafePointer(strdup($0)) },
            sensitivities,
            &porcupine)
        try checkInitStatus(status)
    }

    /// Initializer for single keyword detection.
    ///
    /// - Parameters:
    ///   - modelFilePath: Absolute path to file containing model parameters.
    ///   - wakeKeywordConfigurations: Keyword configuration to use.
    ///   - onDetection: Detection handler to call after wake word detection. The handler is executed on main thread.
    public convenience init(modelFilePath: String, wakeKeywordConfiguration: WakeKeywordConfiguration, onDetection: ((WakeKeywordConfiguration) -> Void)?) throws {
        try self.init(modelFilePath: modelFilePath, wakeKeywordConfigurations: [wakeKeywordConfiguration], onDetection: onDetection)
    }

    deinit {
        pv_porcupine_delete(porcupine)
        porcupine = nil
        if isListening {
            stopListening()
        }
    }

    /// Start listening for configured wake words.
    public func startListening() {

        guard !isListening else {
            return
        }
        
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
            let bufferRef = UnsafeMutablePointer<AudioQueueBufferRef?>.allocate(capacity: 1)
            AudioQueueAllocateBuffer(queue, bufferSize, bufferRef)
            if let buffer = bufferRef.pointee {
                AudioQueueEnqueueBuffer(queue, buffer, 0, nil)
            }
        }

        AudioQueueStart(queue, nil)

        isListening = true
    }

    /// Stop listening for wake wordsl.
    public func stopListening() {

        guard isListening, let queue = audioQueue else {
            return
        }

        AudioQueueStop(queue, true)
        AudioQueueDispose(queue, false)

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

    private func createAudioQueueCallback() -> AudioQueueInputCallback {
        return { userData, queue, bufferRef, startTimeRef, numPackets, packetDescriptions in

            // `self` is passed in as userData in the audio queue callback.
            guard let userData = userData else {
                return
            }
            let `self` = Unmanaged<PorcupineManager>.fromOpaque(userData).takeUnretainedValue()

            let pcm = bufferRef.pointee.mAudioData.assumingMemoryBound(to: Int16.self)
            var result: Int32 = -1

            if self.porcupine != nil {
                pv_porcupine_multiple_keywords_process(self.porcupine, pcm, &result)
            }

            if result >= 0 {
                let index = Int(result)
                let keyword = self.wakeKeywordConfigurations[index]
                DispatchQueue.main.async {
                    self.onDetection?(keyword)
                }
            }

            AudioQueueEnqueueBuffer(queue, bufferRef, 0, nil)

        }
    }
}

public struct WakeKeywordConfiguration {
    let name: String
    let filePath: String
    let sensitivity: Float

    /// Initialiser for the wake word configuration.
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
