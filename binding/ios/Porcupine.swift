//
//  Copyright 2021-2022 Picovoice Inc.
//  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
//  file accompanying this source.
//  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
//  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
//  specific language governing permissions and limitations under the License.
//

import PvPorcupine

/// Low-level iOS binding for Porcupine wake word engine. Provides a Swift interface to the Porcupine library.
public class Porcupine {

    static let resourceBundle: Bundle = {
        let myBundle = Bundle(for: Porcupine.self)

        guard let resourceBundleURL = myBundle.url(
             forResource: "PorcupineResources", withExtension: "bundle")
        else { fatalError("PorcupineResources.bundle not found") }

        guard let resourceBundle = Bundle(url: resourceBundleURL)
            else { fatalError("Could not open PorcupineResources.bundle") }

        return resourceBundle
    }()

    public enum BuiltInKeyword: String, CaseIterable {
        case alexa = "Alexa"
        case americano = "Americano"
        case blueberry = "Blueberry"
        case bumblebee = "Bumblebee"
        case computer = "Computer"
        case grapefruit = "Grapefruit"
        case grasshopper = "Grasshopper"
        case heyGoogle = "Hey Google"
        case heySiri = "Hey Siri"
        case jarvis = "Jarvis"
        case okGoogle = "Ok Google"
        case picovoice = "Picovoice"
        case porcupine = "Porcupine"
        case terminator = "Terminator"
    }

    private var handle: OpaquePointer?
    public static let frameLength = UInt32(pv_porcupine_frame_length())
    public static let sampleRate = UInt32(pv_sample_rate())
    public static let version = String(cString: pv_porcupine_version())

    /// Constructor.
    ///
    /// - Parameters:
    ///   - accessKey: The AccessKey obtained from Picovoice Console (https://console.picovoice.ai).
    ///   - keywordPaths: Absolute paths to keyword model files.
    ///   - modelPath: Absolute path to file containing model parameters.
    ///   - sensitivities: Sensitivities for detecting keywords. Each value should be a number within [0, 1]. 
    ///   A higher sensitivity results in fewer misses at the cost of increasing the false alarm rate.
    /// - Throws: PorcupineError
    public init(
        accessKey: String,
        keywordPaths: [String],
        modelPath: String? = nil,
        sensitivities: [Float32]? = nil
    ) throws {

        var modelPathArg = modelPath
        if modelPath == nil {
            modelPathArg  = Porcupine.resourceBundle.path(forResource: "porcupine_params", ofType: "pv")
            if modelPathArg == nil {
                throw PorcupineIOError("Unable to find the default model path")
            }
        }

        if accessKey.count == 0 {
            throw PorcupineInvalidArgumentError("AccessKey is required for Porcupine initialization")
        }

        let sensitivitiesArg = sensitivities ?? Array(repeating: 0.5, count: keywordPaths.count)

        if sensitivitiesArg.count != keywordPaths.count {
            throw PorcupineInvalidArgumentError("Number of sensitivity values (\(sensitivitiesArg.count)) " +
                "does not match number of keywords (\(keywordPaths.count))")
        }

        if !sensitivitiesArg.allSatisfy({$0 >= 0 && $0 <= 1}) {
            throw PorcupineInvalidArgumentError(
                "One or more sensitivities provided were not floating-point values between [0,1]")
        }

        if !FileManager().fileExists(atPath: modelPathArg!) {
            modelPathArg = try getResourcePath(modelPathArg!)
        }

        var keywordPathsArgs = keywordPaths
        for i in 0..<keywordPathsArgs.count where !FileManager().fileExists(atPath: keywordPathsArgs[i]) {
            keywordPathsArgs[i] = try getResourcePath(keywordPathsArgs[i])
        }

        let status = pv_porcupine_init(
            accessKey,
            modelPathArg,
            Int32(keywordPathsArgs.count),
            keywordPathsArgs.map { UnsafePointer(strdup($0)) },
            sensitivitiesArg,
            &handle)
        try checkStatus(status, "Porcupine init failed")
    }

    /// Constructor.
    ///
    /// - Parameters:
    ///   - accessKey: The AccessKey obtained from Picovoice Console (https://console.picovoice.ai).
    ///   - keywordPath: Absolute paths to a keyword model file.
    ///   - modelPath: Absolute path to file containing model parameters.
    ///   - sensitivity: Sensitivity for detecting keywords. Each value should be a number within [0, 1].
    ///   A higher sensitivity results in fewer misses at the cost of increasing the false alarm rate.
    /// - Throws: PorcupineError
    public convenience init(
        accessKey: String,
        keywordPath: String,
        modelPath: String? = nil,
        sensitivity: Float32 = 0.5
    ) throws {
        try self.init(
            accessKey: accessKey,
            keywordPaths: [keywordPath],
            modelPath: modelPath,
            sensitivities: [sensitivity])
    }

    /// Constructor.
    ///
    /// - Parameters:
    ///   - accessKey: The AccessKey obtained from Picovoice Console (https://console.picovoice.ai).
    ///   - keywords: An array of built-in keywords from the Porcupine.BuiltInKeyword enum.
    ///   - modelPath: Absolute path to file containing model parameters.
    ///   - sensitivities: Sensitivities for detecting keywords. Each value should be a number within [0, 1].
    ///   A higher sensitivity results in fewer misses at the cost of increasing the false alarm rate.
    /// - Throws: PorcupineError
    public convenience init(
        accessKey: String,
        keywords: [Porcupine.BuiltInKeyword],
        modelPath: String? = nil,
        sensitivities: [Float32]? = nil
    ) throws {

        var keywordPaths = [String]()
        for k in keywords {
            let keywordPath = Porcupine.resourceBundle.path(
                forResource: k.rawValue.lowercased() + "_ios",
                ofType: "ppn")
            if keywordPath == nil {
                throw PorcupineIOError("Unable to open the default keyword file for keyword '\(k)'")
            }
            keywordPaths.append(keywordPath!)
        }

        try self.init(
            accessKey: accessKey,
            keywordPaths: keywordPaths,
            modelPath: modelPath,
            sensitivities: sensitivities)
    }

    /// Constructor.
    ///
    /// - Parameters:
    ///   - accessKey: The AccessKey obtained from Picovoice Console (https://console.picovoice.ai).
    ///   - keyword: A built-in keyword from the Porcupine.BuiltInKeyword enum.
    ///   - modelPath: Absolute path to file containing model parameters.
    ///   - sensitivity: Sensitivity for detecting keywords. Each value should be a number within [0, 1].
    ///   A higher sensitivity results in fewer misses at the cost of increasing the false alarm rate.
    /// - Throws: PorcupineError
    public convenience init(
        accessKey: String,
        keyword: Porcupine.BuiltInKeyword,
        modelPath: String? = nil,
        sensitivity: Float32 = 0.5
    ) throws {
        try self.init(accessKey: accessKey, keywords: [keyword], modelPath: modelPath, sensitivities: [sensitivity])
    }

    deinit {
        self.delete()
    }

    /// Releases native resources that were allocated to Porcupine
    public func delete() {
        if handle != nil {
            pv_porcupine_delete(handle)
            handle = nil
        }
    }

    /// Process a frame of audio with the wake word engine
    ///
    /// - Parameters:
    ///   - pcm: An array of 16-bit pcm samples
    /// - Throws: PorcupineError
    /// - Returns:Index of keyword detected or -1 if no keyword was detected
    public func process(pcm: [Int16]) throws -> Int32 {
        if handle == nil {
            throw PorcupineInvalidStateError("Porcupine must be initialized before processing")
        }

        if pcm.count != Porcupine.frameLength {
            throw PorcupineInvalidArgumentError("Frame of audio data must contain \(Porcupine.frameLength) " +
                "samples - given frame contained \(pcm.count)")
        }

        var result: Int32 = -1
        let status = pv_porcupine_process(self.handle, pcm, &result)
        try checkStatus(status, "Porcupine process failed")
        return result
    }

    /// Given a path, return the full path to the resource.
    ///
    /// - Parameters:
    ///   - filePath: relative path of a file in the bundle.
    /// - Throws: PorcupineIOError
    /// - Returns: The full path of the resource.
    private func getResourcePath(_ filePath: String) throws -> String {
        if let resourcePath = Bundle(for: type(of: self)).resourceURL?.appendingPathComponent(filePath).path {
            if FileManager.default.fileExists(atPath: resourcePath) {
                return resourcePath
            }
        }

        throw PorcupineIOError("Could not find file at path '\(filePath)'. " +
            "If this is a packaged asset, ensure you have added it to your xcode project.")
    }

    private func checkStatus(_ status: pv_status_t, _ message: String) throws {
        if status == PV_STATUS_SUCCESS {
            return
        }

        switch status {
        case PV_STATUS_OUT_OF_MEMORY:
            throw PorcupineMemoryError(message)
        case PV_STATUS_IO_ERROR:
            throw PorcupineIOError(message)
        case PV_STATUS_INVALID_ARGUMENT:
            throw PorcupineInvalidArgumentError(message)
        case PV_STATUS_STOP_ITERATION:
            throw PorcupineStopIterationError(message)
        case PV_STATUS_KEY_ERROR:
            throw PorcupineKeyError(message)
        case PV_STATUS_INVALID_STATE:
            throw PorcupineInvalidStateError(message)
        case PV_STATUS_RUNTIME_ERROR:
            throw PorcupineRuntimeError(message)
        case PV_STATUS_ACTIVATION_ERROR:
            throw PorcupineActivationError(message)
        case PV_STATUS_ACTIVATION_LIMIT_REACHED:
            throw PorcupineActivationLimitError(message)
        case PV_STATUS_ACTIVATION_THROTTLED:
            throw PorcupineActivationThrottledError(message)
        case PV_STATUS_ACTIVATION_REFUSED:
            throw PorcupineActivationRefusedError(message)
        default:
            let pvStatusString = String(cString: pv_status_to_string(status))
            throw PorcupineError("\(pvStatusString): \(message)")
        }
    }
}
