//
//  Copyright 2018-2023 Picovoice Inc.
//  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
//  file accompanying this source.
//  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
//  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
//  specific language governing permissions and limitations under the License.
//

import ios_voice_processor

/// High-level iOS binding for Porcupine wake word engine. It handles recording audio from microphone,
/// processes it in real-time using Porcupine, and notifies when any of the given keywords are detected.
public class PorcupineManager {
    private var porcupine: Porcupine?

    private var frameListener: VoiceProcessorFrameListener?
    private var errorListener: VoiceProcessorErrorListener?

    private var isListening = false

    /// Private constructor.
    private init(porcupine: Porcupine, onDetection: ((Int32) -> Void)?, errorCallback: ((Error) -> Void)?) throws {
        self.porcupine = porcupine
        self.errorListener = VoiceProcessorErrorListener({ error in
            guard let callback = errorCallback else {
                print("\(error.localizedDescription)")
                return
            }
            DispatchQueue.main.async {
                callback(PorcupineError(error.localizedDescription))
            }
        })
        self.frameListener = VoiceProcessorFrameListener({ frame in
            guard let porcupine = self.porcupine else {
                return
            }

            do {
                let keywordIndex: Int32 = try porcupine.process(pcm: frame)
                if keywordIndex >= 0 {
                    DispatchQueue.main.async {
                        onDetection?(keywordIndex)
                    }
                }
            } catch {
                guard let callback = errorCallback else {
                    print("\(error)")
                    return
                }
                DispatchQueue.main.async {
                    callback(error)
                }
            }
        })

    }

    /// Constructor.
    ///
    /// - Parameters:
    ///   - accessKey: The AccessKey obtained from Picovoice console.
    ///   - keywordPaths: Absolute paths to keyword model files.
    ///   - modelPath: Absolute path to file containing model parameters.
    ///   - sensitivities: Sensitivities for detecting keywords. Each value should be a number within [0, 1].
    ///   A higher sensitivity results in fewer misses at the cost of increasing the false alarm rate.
    ///   - onDetection: It is invoked upon detection of the keyword.
    ///   - errorCallback: Invoked if an error occurs while processing frames.
    ///   If missing, error will be printed to console.
    /// - Throws: PorcupineError
    public convenience init(
            accessKey: String,
            keywordPaths: [String],
            modelPath: String? = nil,
            sensitivities: [Float32]? = nil,
            onDetection: ((Int32) -> Void)?,
            errorCallback: ((Error) -> Void)? = nil) throws {

        try self.init(
                porcupine: Porcupine(
                        accessKey: accessKey,
                        keywordPaths: keywordPaths,
                        modelPath: modelPath,
                        sensitivities: sensitivities
                ),
                onDetection: onDetection,
                errorCallback: errorCallback)
    }

    /// Constructor.
    ///
    /// - Parameters:
    ///   - accessKey: The AccessKey obtained from Picovoice console.
    ///   - keywordPath: Absolute paths to a keyword model file.
    ///   - modelPath: Absolute path to file containing model parameters.
    ///   - sensitivity: Sensitivity for detecting keywords. Each value should be a number within [0, 1].
    ///   A higher sensitivity results in fewer misses at the cost of increasing the false alarm rate.
    ///   - onDetection: It is invoked upon detection of the keyword.
    ///   - errorCallback: Invoked if an error occurs while processing frames.
    ///   If missing, error will be printed to console.
    /// - Throws: PorcupineError
    public convenience init(
            accessKey: String,
            keywordPath: String,
            modelPath: String? = nil,
            sensitivity: Float32 = 0.5,
            onDetection: ((Int32) -> Void)?,
            errorCallback: ((Error) -> Void)? = nil) throws {

        try self.init(
                porcupine: Porcupine(
                        accessKey: accessKey,
                        keywordPath: keywordPath,
                        modelPath: modelPath,
                        sensitivity: sensitivity
                ),
                onDetection: onDetection,
                errorCallback: errorCallback)
    }

    /// Constructor.
    ///
    /// - Parameters:
    ///   - accessKey: The AccessKey obtained from Picovoice console.
    ///   - keywords: An array of built-in keywords from the Porcupine.BuiltInKeyword enum.
    ///   - modelPath: Absolute path to file containing model parameters.
    ///   - sensitivities: Sensitivities for detecting keywords. Each value should be a number within [0, 1].
    ///   A higher sensitivity results in fewer misses at the cost of increasing the false alarm rate.
    ///   - onDetection: It is invoked upon detection of the keyword.
    ///   - errorCallback: Invoked if an error occurs while processing frames.
    ///   If missing, error will be printed to console.
    /// - Throws: PorcupineError
    public convenience init(
            accessKey: String,
            keywords: [Porcupine.BuiltInKeyword],
            modelPath: String? = nil,
            sensitivities: [Float32]? = nil,
            onDetection: ((Int32) -> Void)?,
            errorCallback: ((Error) -> Void)? = nil) throws {

        try self.init(
                porcupine: Porcupine(
                        accessKey: accessKey,
                        keywords: keywords,
                        modelPath: modelPath,
                        sensitivities: sensitivities
                ),
                onDetection: onDetection,
                errorCallback: errorCallback)
    }

    /// Constructor.
    ///
    /// - Parameters:
    ///   - accessKey: The AccessKey obtained from Picovoice console.
    ///   - keyword: A built-in keyword from the Porcupine.BuiltInKeyword enum.
    ///   - modelPath: Absolute path to file containing model parameters.
    ///   - sensitivities: Sensitivities for detecting keywords. Each value should be a number within [0, 1].
    ///   A higher sensitivity results in fewer misses at the cost of increasing the false alarm rate.
    ///   - onDetection: It is invoked upon detection of the keyword.
    ///   - errorCallback: Invoked if an error occurs while processing frames.
    ///   If missing, error will be printed to console.
    /// - Throws: PorcupineError
    public convenience init(
            accessKey: String,
            keyword: Porcupine.BuiltInKeyword,
            modelPath: String? = nil,
            sensitivity: Float32 = 0.5,
            onDetection: ((Int32) -> Void)?,
            errorCallback: ((Error) -> Void)? = nil) throws {
        try self.init(
                porcupine: Porcupine(
                        accessKey: accessKey,
                        keyword: keyword,
                        modelPath: modelPath,
                        sensitivity: sensitivity
                ),
                onDetection: onDetection,
                errorCallback: errorCallback)
    }

    deinit {
        if self.porcupine != nil {
            self.porcupine!.delete()
            self.porcupine = nil
        }
    }

    /// Stops audio recording and releases Porcupine resources
    ///
    /// - Throws: PorcupineError if there was an error stopping PorcupineManager
    public func delete() throws {
        if isListening {
            try stop()
        }

        if self.porcupine != nil {
            self.porcupine!.delete()
            self.porcupine = nil
        }
    }

    ///  Starts recording audio from the microphone and monitors it for the utterances of the given set of keywords.
    ///
    /// - Throws: PorcupineError if there was an error starting PorcupineManager.
    public func start() throws {
        guard !isListening else {
            return
        }

        if porcupine == nil {
            throw PorcupineInvalidStateError("Porcupine has been deleted.")
        }

        VoiceProcessor.instance.addErrorListener(errorListener!)
        VoiceProcessor.instance.addFrameListener(frameListener!)

        do {
            try VoiceProcessor.instance.start(
                    frameLength: Porcupine.frameLength,
                    sampleRate: Porcupine.sampleRate
            )
        } catch {
            throw PorcupineError(error.localizedDescription)
        }
        isListening = true
    }

    /// Stop listening for wake words.
    ///
    /// - Throws: PorcupineError if there was an error stopping PorcupineManager.
    public func stop() throws {
        guard isListening else {
            return
        }

        VoiceProcessor.instance.removeErrorListener(errorListener!)
        VoiceProcessor.instance.removeFrameListener(frameListener!)

        if VoiceProcessor.instance.numFrameListeners == 0 {
            do {
                try VoiceProcessor.instance.stop()
            } catch {
                throw PorcupineError(error.localizedDescription)
            }
        }
        isListening = false
    }
}
