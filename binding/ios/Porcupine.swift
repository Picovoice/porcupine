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
    case invalidArgument(message:String)
    case io
    case outOfMemory
    case invalidState
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
    ///   - keywordPaths: Absolute paths to keyword model files.
    ///   - modelPath: Absolute path to file containing model parameters.
    ///   - sensitivities: Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher sensitivity results in fewer misses at
    ///   the cost of increasing the false alarm rate.
    /// - Throws: PorcupineError
    public init(keywordPaths: [String], modelPath:String? = nil, sensitivities: [Float32]? = nil) throws {
        
        var modelPathArg = modelPath
        if (modelPath == nil){
            let bundle = Bundle(for: type(of: self))
            modelPathArg  = bundle.path(forResource: "porcupine_params", ofType: "pv")
            if modelPathArg == nil {
                throw PorcupineError.io
            }
        }
        
        let sensitivitiesArg = sensitivities ?? Array(repeating: 0.5, count: keywordPaths.count)
        
        if sensitivitiesArg.count != keywordPaths.count {
            throw PorcupineError.invalidArgument(message: "Number of sensitivity values (\(sensitivitiesArg.count)) does not match number of keywords (\(keywordPaths.count))")
        }
        
        if !sensitivitiesArg.allSatisfy({$0 >= 0 && $0 <= 1}) {
            throw PorcupineError.invalidArgument(message: "One or more sensitivities provided were not floating-point values between [0,1]")
        }
        
        if !FileManager().fileExists(atPath: modelPathArg!){
            throw PorcupineError.invalidArgument(message: "Model file at does not exist at '\(modelPathArg!)'")
        }
        
        for keywordPath in keywordPaths {
            if !FileManager().fileExists(atPath: keywordPath){
                throw PorcupineError.invalidArgument(message: "Keyword file at does not exist at '\(keywordPath)'")
            }
        }
        
        let status = pv_porcupine_init(
            modelPathArg,
            Int32(keywordPaths.count),
            keywordPaths.map { UnsafePointer(strdup($0)) },
            sensitivitiesArg,
            &handle)
        try checkStatus(status)
    }
    
    /// Constructor.
    ///
    /// - Parameters:
    ///   - keywordPath: Absolute paths to a keyword model file.
    ///   - modelPath: Absolute path to file containing model parameters.
    ///   - sensitivity: Sensitivity for detecting keywords. Each value should be a number within [0, 1]. A higher sensitivity results in fewer misses at
    ///   the cost of increasing the false alarm rate.
    /// - Throws: PorcupineError
    public convenience init(keywordPath: String, modelPath:String? = nil, sensitivity: Float32 = 0.5) throws {
        try self.init(keywordPaths: [keywordPath], modelPath:modelPath, sensitivities: [sensitivity])
    }
    
    /// Constructor.
    ///
    /// - Parameters:
    ///   - keywords: An array of built-in keywords from the Porcupine.BuiltInKeyword enum.
    ///   - modelPath: Absolute path to file containing model parameters.
    ///   - sensitivities: Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher sensitivity results in fewer misses at
    ///   the cost of increasing the false alarm rate.
    /// - Throws: PorcupineError
    public convenience init(keywords:[Porcupine.BuiltInKeyword], modelPath:String? = nil, sensitivities: [Float32]? = nil) throws {
        
        var keywordPaths = [String]()
        let bundle = Bundle(for: type(of: self))
        for k in keywords{
            let keywordPath = bundle.path(forResource: k.rawValue.lowercased() + "_ios", ofType: "ppn")
            if keywordPath == nil {
                throw PorcupineError.io
            }
            keywordPaths.append(keywordPath!);
        }
        
        try self.init(keywordPaths: keywordPaths, modelPath: modelPath, sensitivities: sensitivities)
    }
    
    /// Constructor.
    ///
    /// - Parameters:
    ///   - keyword: A built-in keyword from the Porcupine.BuiltInKeyword enum.
    ///   - modelPath: Absolute path to file containing model parameters.
    ///   - sensitivities: Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher sensitivity results in fewer misses at
    ///   the cost of increasing the false alarm rate.
    /// - Throws: PorcupineError
    public convenience init(keyword: Porcupine.BuiltInKeyword, modelPath:String? = nil, sensitivity: Float32 = 0.5) throws {
        try self.init(keywords: [keyword], modelPath:modelPath, sensitivities: [sensitivity])
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
            throw PorcupineError.invalidState
        }
        
        if pcm.count != Porcupine.frameLength {
            throw PorcupineError.invalidArgument(message: "Frame of audio data must contain \(Porcupine.frameLength) samples - given frame contained \(pcm.count)")
        }
        
        var result: Int32 = -1
        let status = pv_porcupine_process(self.handle, pcm, &result)
        try checkStatus(status)
        return result
    }
    
    private func checkStatus(_ status: pv_status_t) throws {
        switch status {
        case PV_STATUS_IO_ERROR:
            throw PorcupineError.io
        case PV_STATUS_OUT_OF_MEMORY:
            throw PorcupineError.outOfMemory
        case PV_STATUS_INVALID_ARGUMENT:
            throw PorcupineError.invalidArgument(message:"Porcupine rejected one of the provided arguments.")
        case PV_STATUS_INVALID_STATE:
            throw PorcupineError.invalidState
        default:
            return
        }
    }
}
