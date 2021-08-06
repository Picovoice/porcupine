//
//  Copyright 2018-2021 Picovoice Inc.
//  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
//  file accompanying this source.
//  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
//  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
//  specific language governing permissions and limitations under the License.
//

import ios_voice_processor

public enum PorcupineManagerError: Error {
    case recordingDenied
    case objectDisposed
}

/// High-level iOS binding for Porcupine wake word engine. It handles recording audio from microphone, processes it in real-time using Porcupine, and notifies the
/// client when any of the given keywords are detected.
public class PorcupineManager {
    private var onDetection: ((Int32) -> Void)?
    
    private var porcupine: Porcupine?
    
    private let voiceProcessor: VoiceProcessor;
    
    private var isListening = false
    
    /// Private constructor.
    private init(porcupine:Porcupine, onDetection: ((Int32) -> Void)?) throws {
        self.onDetection = onDetection
        self.porcupine = porcupine
        
        self.voiceProcessor = VoiceProcessor()
    }
    
    /// Constructor.
    ///
    /// - Parameters:
    ///   - keywordPaths: Absolute paths to keyword model files.
    ///   - modelPath: Absolute path to file containing model parameters.
    ///   - sensitivities: Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher sensitivity results in fewer misses at
    ///   the cost of increasing the false alarm rate.
    ///   - onDetection: It is invoked upon detection of the keyword.
    /// - Throws: PorcupineError
    public convenience init(keywordPaths: [String], modelPath:String? = nil, sensitivities: [Float32]? = nil, onDetection: ((Int32) -> Void)?) throws {
        try self.init(porcupine:Porcupine(keywordPaths: keywordPaths, modelPath: modelPath, sensitivities: sensitivities), onDetection:onDetection)
    }
    
    /// Constructor.
    ///
    /// - Parameters:
    ///   - keywordPath: Absolute paths to a keyword model file.
    ///   - modelPath: Absolute path to file containing model parameters.
    ///   - sensitivity: Sensitivity for detecting keywords. Each value should be a number within [0, 1]. A higher sensitivity results in fewer misses at
    ///   the cost of increasing the false alarm rate.
    ///   - onDetection: It is invoked upon detection of the keyword.
    /// - Throws: PorcupineError
    public convenience init(keywordPath: String, modelPath:String? = nil, sensitivity: Float32 = 0.5, onDetection: ((Int32) -> Void)?) throws {
        try self.init(porcupine:Porcupine(keywordPath: keywordPath, modelPath: modelPath, sensitivity: sensitivity), onDetection:onDetection)
    }
    
    /// Constructor.
    ///
    /// - Parameters:
    ///   - keywords: An array of built-in keywords from the Porcupine.BuiltInKeyword enum.
    ///   - modelPath: Absolute path to file containing model parameters.
    ///   - sensitivities: Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher sensitivity results in fewer misses at
    ///   the cost of increasing the false alarm rate.
    ///   - onDetection: It is invoked upon detection of the keyword.
    /// - Throws: PorcupineError
    public convenience init(keywords:[Porcupine.BuiltInKeyword], modelPath:String? = nil, sensitivities: [Float32]? = nil, onDetection: ((Int32) -> Void)?) throws {
        
        try self.init(porcupine:Porcupine(keywords: keywords, modelPath: modelPath, sensitivities: sensitivities), onDetection:onDetection)
    }
    
    /// Constructor.
    ///
    /// - Parameters:
    ///   - keyword: A built-in keyword from the Porcupine.BuiltInKeyword enum.
    ///   - modelPath: Absolute path to file containing model parameters.
    ///   - sensitivities: Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher sensitivity results in fewer misses at
    ///   the cost of increasing the false alarm rate.
    ///   - onDetection: It is invoked upon detection of the keyword.
    /// - Throws: PorcupineError
    public convenience init(keyword: Porcupine.BuiltInKeyword, modelPath: String? = nil, sensitivity: Float32 = 0.5, onDetection: ((Int32) -> Void)?) throws {
        try self.init(porcupine:Porcupine(keyword: keyword, modelPath: modelPath, sensitivity: sensitivity), onDetection:onDetection)
    }
    
    deinit {
        self.delete()
    }
    
    /// Stops recording and releases Porcupine resources
    public func delete() {
        if isListening {
            stop()
        }
        
        if self.porcupine != nil {
            self.porcupine!.delete()
            self.porcupine = nil
        }
    }
    
    ///  Starts recording audio from the microphone and monitors it for the utterances of the given set of keywords.
    ///
    /// - Throws: AVAudioSession, AVAudioEngine errors. Additionally PorcupineManagerError if
    ///           microphone permission is not granted or Porcupine has been disposed.
    public func start() throws {
        
        guard !isListening else {
            return
        }

        if porcupine == nil {
            throw PorcupineManagerError.objectDisposed
        }

        // Only check if it's denied, permission will be automatically asked.
        if try !voiceProcessor.hasPermissions() {
            throw PorcupineManagerError.recordingDenied
        }
        
        try voiceProcessor.start(
            frameLength: Porcupine.frameLength,
            sampleRate: Porcupine.sampleRate,
            audioCallback: self.audioCallback
        )
        
        isListening = true
    }
    
    /// Stop listening for wake words.
    public func stop() {
        guard isListening else {
            return
        }
        
        voiceProcessor.stop()
        
        isListening = false
    }
    
    /// Callback to run after after voice processor processes frames.
    private func audioCallback(frameLength: UInt32, pcm: UnsafePointer<Int16> ) {
        guard self.porcupine != nil else {
            return
        }
        
        do{
            let result:Int32 = try self.porcupine!.process(pcm: pcm)
            if result >= 0 {
                DispatchQueue.main.async {
                    self.onDetection?(result)
                }
            }
        } catch {
            print("\(error)")
        }
    }
}
