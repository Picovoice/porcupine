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
    private let keywordFilePaths: [String]
    private let sensitivities: [Float]
    private let keywordCallback: (() -> Void)?
    private let multiKeywordCallback: ((Int32) -> Void)?
    
    private var isListening = false
    private var porcupine: OpaquePointer?
    private var audioEngine : AVAudioEngine!
    
    var file : AVAudioFile!
    
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
    
    private func createAudioEngine() {
        audioEngine = AVAudioEngine()
        let inputNode = audioEngine.inputNode
        
        let userData = UnsafeMutableRawPointer(Unmanaged.passUnretained(self).toOpaque())
        
        inputNode.installTap(onBus: 0, bufferSize: UInt32(0.128 * inputNode.inputFormat(forBus: 0).sampleRate), format: inputNode.inputFormat(forBus: 0), block:
            { (buffer: AVAudioPCMBuffer!, time: AVAudioTime!) -> Void in
                let picoFormat = AVAudioFormat(
                    commonFormat: .pcmFormatInt16, sampleRate: Double(16000), channels: 1, interleaved: true)
                
                let converter = AVAudioConverter(from: inputNode.inputFormat(forBus: 0), to: picoFormat!)
                let resBuffer = AVAudioPCMBuffer(pcmFormat: picoFormat!, frameCapacity: 4 * UInt32(pv_porcupine_frame_length()))!
                var error: NSErrorPointer
                
                let inputBlock: AVAudioConverterInputBlock = { inNumPackets, outStatus in
                    outStatus.pointee = AVAudioConverterInputStatus.haveData
                    return buffer
                }
                
                converter!.convert(to: resBuffer, error: error, withInputFrom: inputBlock)
                
                let porcupineManager: PorcupineManager = Unmanaged<PorcupineManager>.fromOpaque(userData).takeUnretainedValue()
                let pcm: UnsafeMutablePointer<Int16> = (resBuffer.int16ChannelData?.pointee)!
                
                var result: Int32 = -1
                
                // process 4 buffers in a loop
                for i in 0...3 {
                    pv_porcupine_multiple_keywords_process(
                        porcupineManager.porcupine, (pcm + (i * Int(pv_porcupine_frame_length()))), &result)
                    if result >= 0 {
                        if porcupineManager.keywordCallback != nil {
                            porcupineManager.keywordCallback!()
                        } else {
                            porcupineManager.multiKeywordCallback!(result)
                        }
                    }
                }
        })
        
        audioEngine.prepare()
        
        do {
            try audioEngine.start()
        } catch {
            print("error starting audio engine")
        }
    }
    
    
    /// Initializer for single keyword use case.
    ///
    /// - Parameters:
    ///   - modelFilePath: Absolute path to file containing model parameters.
    ///   - keywordFilePath: Absolute path to keyword file containing hyper-parameters.
    ///   - sensitivity: Sensitivity parameter. A higher sensitivity value lowers miss rate at the cost of increased
    ///     false alarm rate. For more information regarding this parameter refer to 'include/pv_porcupine.h'.
    ///   - keywordCallback: Callback to be executed after wake word detection.
    init(modelFilePath: String, keywordFilePath: String, sensitivity: Float, keywordCallback: @escaping (() -> Void)) {
        self.modelFilePath = modelFilePath
        self.keywordFilePaths = [keywordFilePath]
        self.sensitivities = [sensitivity]
        self.keywordCallback = keywordCallback
        self.multiKeywordCallback = nil
    }
    
    
    /// Initializer for multiple keyword use case.
    ///
    /// - Parameters:
    ///   - modelFilePath: Absolute path to file containing model parameters.
    ///   - keywordFilePaths: Absolute paths to keyword files.
    ///   - sensitivities: List of sensitivity parameters for each keyword.
    ///   - keywordCallback: Callback to be executed after wake word detection.
    init(modelFilePath: String, keywordFilePaths: [String], sensitivities: [Float], keywordCallback: @escaping ((Int32) -> Void)) {
        self.modelFilePath = modelFilePath
        self.keywordFilePaths = keywordFilePaths
        self.sensitivities = sensitivities
        self.multiKeywordCallback = keywordCallback
        self.keywordCallback = nil
    }
    
    /// Initializes Porcupine engine and audio recording.
    ///
    /// - Throws: Throws 'PorcupineManagerError' when Porcupine initialization fails.
    func start() throws {
        if isListening { return }
        
        let status = pv_porcupine_multiple_keywords_init(
            modelFilePath,
            Int32(keywordFilePaths.count),
            keywordFilePaths.map{ UnsafePointer(strdup($0)) },
            sensitivities,
            &porcupine)
        try checkStatus(status)
        
        createAudioEngine()
        
        
        isListening = true
    }
    
    /// Stops recording and releases resources acquired by Porcupine.
    func stop() {
        if !isListening { return }
        audioEngine.inputNode.removeTap(onBus: 0)
        pv_porcupine_delete(porcupine)
        
        isListening = false
    }
    
    func URLFor(filename: String) -> URL {
        let dirs  = NSSearchPathForDirectoriesInDomains(FileManager.SearchPathDirectory.documentDirectory, FileManager.SearchPathDomainMask.userDomainMask, true)
        let dir = dirs[0] //documents directory
        let path = dir.appending(filename)
        return URL(fileURLWithPath: path)
    }
}
