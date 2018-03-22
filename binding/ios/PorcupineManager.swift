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

import AudioToolbox
import AVFoundation
import Foundation

import pv_porcupine

/// Errors corresponding to different status codes returned by Porcupine library.
enum PorcupineManagerError: Error {
    case OutOfMemoryError
    case IOError
    case InvalidArgumentError
}

/// iOS binding for Picovoice's wake word detection engine (aka Porcupine).
class PorcupineManager {
    private let numBuffers = 5
    private let modelFilePath: String
    private let keywordFilePath: String
    private let sensitivity: Float
    private let keywordCallback: (() -> Void)

    private var isListening = false
    private var queue: AudioQueueRef?
    private var porcupine: OpaquePointer?

    private let audioCallback: AudioQueueInputCallback = {
        userData, queue, bufferRef, startTimeRef, numPackets, packetDescriptions in

        let porcupineManager: PorcupineManager = Unmanaged<PorcupineManager>.fromOpaque(userData!).takeUnretainedValue()
        let pcm = bufferRef.pointee.mAudioData.assumingMemoryBound(to: Int16.self)
        var result = false

        pv_porcupine_process(porcupineManager.porcupine, pcm, &result)

        if result { porcupineManager.keywordCallback() }

        AudioQueueEnqueueBuffer(queue, bufferRef, 0, nil)
    }

    private func checkStatus(_ status: pv_status_t) throws {
        switch status {
        case PV_STATUS_IO_ERROR:
            throw PorcupineManagerError.IOError
        case PV_STATUS_OUT_OF_MEMORY:
            throw PorcupineManagerError.OutOfMemoryError
        case PV_STATUS_INVALID_ARGUMENT:
            throw PorcupineManagerError.InvalidArgumentError
        default:
            return
        }
    }

    
    /// Initializer.
    ///
    /// - Parameters:
    ///   - modelFilePath: Absolute path to file containing model parameters.
    ///   - keywordFilePath: Absolute path to keyword file containing hyper-parameters.
    ///   - sensitivity: Sensitivity parameter. A higher sensitivity value lowers miss rate at the cost of increased
    ///     false alarm rate. For more information regarding this parameter refer to 'include/pv_porcupine.h'.
    ///   - keywordCallback: Callback to be executed after wake word detection.
    init(modelFilePath: String, keywordFilePath: String, sensitivity: Float, keywordCallback: @escaping (() -> Void)) {
        self.modelFilePath = modelFilePath
        self.keywordFilePath = keywordFilePath
        self.sensitivity = sensitivity
        self.keywordCallback = keywordCallback
    }

    /// Initializes Porcupine engine and audio recording.
    ///
    /// - Throws: Throws 'PorcupineManagerError' when Porcupine initialization fails.
    func start() throws {
        if isListening { return }

        let status = pv_porcupine_init(modelFilePath, keywordFilePath, sensitivity, &porcupine)
        try checkStatus(status)

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
        AudioQueueNewInput(&format, audioCallback, userData, nil, nil, 0, &queue)

        guard let queue = queue else { return }
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

    /// Stops recording and releases resources acquired by Porcupine.
    func stop() {
        if !isListening { return }
        
        guard let queue = queue else { return }
        AudioQueueStop(queue, true)
        AudioQueueDispose(queue, false)

        pv_porcupine_delete(porcupine)

        isListening = false
    }
}
