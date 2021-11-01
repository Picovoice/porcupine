//
//  Copyright 2021 Picovoice Inc.
//  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
//  file accompanying this source.
//  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
//  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
//  specific language governing permissions and limitations under the License.
//

import PvPorcupine

public enum PorcupineError: Error {
    case PorcupineOutOfMemoryError(_ message:String)
    case PorcupineIOError(_ message:String)
    case PorcupineInvalidArgumentError(_ message:String)
    case PorcupineStopIterationError(_ message:String)
    case PorcupineKeyError(_ message:String)
    case PorcupineInvalidStateError(_ message:String)
    case PorcupineRuntimeError(_ message:String)
    case PorcupineActivationError(_ message:String)
    case PorcupineActivationLimitError(_ message:String)
    case PorcupineActivationThrottledError(_ message:String)
    case PorcupineActivationRefusedError(_ message:String)
    case PorcupineInternalError(_ message:String)
}

/// Low-level iOS binding for Porcupine wake word engine. Provides a Swift interface to the Porcupine library.
public class Porcupine {
    
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
    ///   - accessKey: The AccessKey obtained from Picovoice console.
    ///   - keywordPaths: Absolute paths to keyword model files.
    ///   - modelPath: Absolute path to file containing model parameters.
    ///   - sensitivities: Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher sensitivity results in fewer misses at
    ///   the cost of increasing the false alarm rate.
    /// - Throws: PorcupineError
    public init(accessKey: String, keywordPaths: [String], modelPath:String? = nil, sensitivities: [Float32]? = nil) throws {
        
        var modelPathArg = modelPath
        if (modelPath == nil){
            let bundle = Bundle(for: type(of: self))
            modelPathArg  = bundle.path(forResource: "porcupine_params", ofType: "pv")
            if modelPathArg == nil {
                throw PorcupineError.PorcupineIOError("Unable to open the the porcupine_params model at \(modelPathArg)")
            }
        }
        
        let sensitivitiesArg = sensitivities ?? Array(repeating: 0.5, count: keywordPaths.count)
        
        if sensitivitiesArg.count != keywordPaths.count {
            throw PorcupineError.PorcupineInvalidArgumentError("Number of sensitivity values (\(sensitivitiesArg.count)) does not match number of keywords (\(keywordPaths.count))")
        }
        
        if !sensitivitiesArg.allSatisfy({$0 >= 0 && $0 <= 1}) {
            throw PorcupineError.PorcupineInvalidArgumentError("One or more sensitivities provided were not floating-point values between [0,1]")
        }
        
        if !FileManager().fileExists(atPath: modelPathArg!){
            throw PorcupineError.PorcupineInvalidArgumentError("Model file at does not exist at '\(modelPathArg!)'")
        }
        
        for keywordPath in keywordPaths {
            if !FileManager().fileExists(atPath: keywordPath){
                throw PorcupineError.PorcupineInvalidArgumentError("Keyword file at does not exist at '\(keywordPath)'")
            }
        }
        
        let status = pv_porcupine_init(
            accessKey,
            modelPathArg,
            Int32(keywordPaths.count),
            keywordPaths.map { UnsafePointer(strdup($0)) },
            sensitivitiesArg,
            &handle)
        try checkStatus(status, "Porcupine init failed")
    }
    
    /// Constructor.
    ///
    /// - Parameters:
    ///   - accessKey: The AccessKey obtained from Picovoice console.
    ///   - keywordPath: Absolute paths to a keyword model file.
    ///   - modelPath: Absolute path to file containing model parameters.
    ///   - sensitivity: Sensitivity for detecting keywords. Each value should be a number within [0, 1]. A higher sensitivity results in fewer misses at
    ///   the cost of increasing the false alarm rate.
    /// - Throws: PorcupineError
    public convenience init(accessKey: String, keywordPath: String, modelPath:String? = nil, sensitivity: Float32 = 0.5) throws {
        try self.init(accessKey: accessKey, keywordPaths: [keywordPath], modelPath:modelPath, sensitivities: [sensitivity])
    }
    
    /// Constructor.
    ///
    /// - Parameters:
    ///   - accessKey: The AccessKey obtained from Picovoice console.
    ///   - keywords: An array of built-in keywords from the Porcupine.BuiltInKeyword enum.
    ///   - modelPath: Absolute path to file containing model parameters.
    ///   - sensitivities: Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher sensitivity results in fewer misses at
    ///   the cost of increasing the false alarm rate.
    /// - Throws: PorcupineError
    public convenience init(accessKey: String, keywords:[Porcupine.BuiltInKeyword], modelPath:String? = nil, sensitivities: [Float32]? = nil) throws {
        
        var keywordPaths = [String]()
        let bundle = Bundle(for: type(of: self))
        for k in keywords{
            let keywordPath = bundle.path(forResource: k.rawValue.lowercased() + "_ios", ofType: "ppn")
            if keywordPath == nil {
                throw PorcupineError.PorcupineIOError("Unable to open the the keyword file at \(keywordPath)")
            }
            keywordPaths.append(keywordPath!);
        }
        
        try self.init(accessKey: accessKey, keywordPaths: keywordPaths, modelPath: modelPath, sensitivities: sensitivities)
    }
    
    /// Constructor.
    ///
    /// - Parameters:
    ///   - accessKey: The AccessKey obtained from Picovoice console.
    ///   - keyword: A built-in keyword from the Porcupine.BuiltInKeyword enum.
    ///   - modelPath: Absolute path to file containing model parameters.
    ///   - sensitivities: Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher sensitivity results in fewer misses at
    ///   the cost of increasing the false alarm rate.
    /// - Throws: PorcupineError
    public convenience init(accessKey: String, keyword: Porcupine.BuiltInKeyword, modelPath:String? = nil, sensitivity: Float32 = 0.5) throws {
        try self.init(accessKey: accessKey, keywords: [keyword], modelPath:modelPath, sensitivities: [sensitivity])
    }
    
    deinit {
        self.delete()
    }
    
    /// Releases native resources that were allocated to Porcupine
    public func delete(){
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
    public func process(pcm:[Int16]) throws -> Int32 {
        if handle == nil {
            throw PorcupineError.PorcupineInvalidStateError("Porcupine must be initialized before processing")
        }
        
        if pcm.count != Porcupine.frameLength {
            throw PorcupineError.PorcupineInvalidArgumentError("Frame of audio data must contain \(Porcupine.frameLength) samples - given frame contained \(pcm.count)")
        }
        
        var result: Int32 = -1
        let status = pv_porcupine_process(self.handle, pcm, &result)
        try checkStatus(status, "Porcupine process failed")
        return result
    }
    
    private func checkStatus(_ status: pv_status_t, _ message: String) -> PorcupineError {
        switch status {
        case PV_STATUS_OUT_OF_MEMORY:
            return PorcupineError.PorcupineOutOfMemoryError(message)
        case PV_STATUS_IO_ERROR:
            return PorcupineError.PorcupineIOError(message)
        case PV_STATUS_INVALID_ARGUMENT:
            return PorcupineError.PorcupineInvalidArgumentError(message)
        case PV_STATUS_STOP_ITERATION:
            return PorcupineError.PorcupineStopIterationError(message)
        case PV_STATUS_KEY_ERROR:
            return PorcupineError.PorcupineKeyError(message)
        case PV_STATUS_INVALID_STATE:
            return PorcupineError.PorcupineInvalidStateError(message)
        case PV_STATUS_RUNTIME_ERROR:
            return PorcupineError.PorcupineRuntimeError(message)
        case PV_STATUS_ACTIVATION_ERROR:
            return PorcupineError.PorcupineActivationError(message)
        case PV_STATUS_ACTIVATION_LIMIT_REACHED:
            return PorcupineError.PorcupineActivationLimitError(message)
        case PV_STATUS_ACTIVATION_THROTTLED:
            return PorcupineError.PorcupineActivationThrottledError(message)
        case PV_STATUS_ACTIVATION_REFUSED:
            return PorcupineError.PorcupineActivationRefusedError(message)
        default:
            let pvStatusString = String(cString: pv_status_to_string(status))
            return PorcupineError.PorcupineInternalError("\(pvStatusString): \(message)")
        }
    }
}
