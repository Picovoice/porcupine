//
// Copyright 2020-2021 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
// file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//

import PvPorcupine
import Foundation

enum PvPorcupineError: Error {
    case RuntimeError(_ message: String)
}

@objc(PvPorcupine)
class PvPorcupine: NSObject {
    private var porcupinePool:Dictionary<String, OpaquePointer?> = [:]

    @objc func constantsToExport() -> Dictionary<String, Any> {        
        
        let modelPath : String = Bundle.main.path(forResource: "porcupine_params", ofType: "pv") ?? "unknown"
        
        let keywordPaths = Bundle.main.paths(forResourcesOfType: "ppn", inDirectory: nil)                        
        var keywordDict:Dictionary<String, String> = [:]
        for keywordPath in keywordPaths{
            
            let keywordName = URL(fileURLWithPath:keywordPath).lastPathComponent.components(separatedBy:"_")[0]            
            keywordDict[keywordName] = keywordPath
        }
        
        return [
            "DEFAULT_MODEL_PATH": modelPath,
            "KEYWORD_PATHS": keywordDict                 
        ]
    }

    @objc(fromKeywordPaths:modelPath:keywordPaths:sensitivities:resolver:rejecter:)
    func fromKeywordPaths(accessKey: String, modelPath: String, keywordPaths: [String], sensitivities: [Float32],
        resolver resolve:RCTPromiseResolveBlock, rejecter reject:RCTPromiseRejectBlock) -> Void {
        var modelPath = modelPath
        var keywordPaths = keywordPaths
        
        do {
            modelPath = try getResourcePath(modelPath)
            keywordPaths = try keywordPaths.map { try getResourcePath($0) }
        } catch {
            reject(pvStatusToExceptionCode(PV_STATUS_IO_ERROR), "failed to get resource path: \(error)", nil)
        }
        
        var porcupine:OpaquePointer?
        let status = pv_porcupine_init(
            accessKey,
            modelPath,
            Int32(keywordPaths.count),
            keywordPaths.map{ UnsafePointer(strdup($0)) },
            sensitivities,
            &porcupine);

        if status == PV_STATUS_SUCCESS {
            let handle:String = String(describing:porcupine)
            porcupinePool[handle] = porcupine;
            
            let porcupineParameters:Dictionary<String, Any> = [
                "handle": handle,
                "frameLength": UInt32(pv_porcupine_frame_length()),
                "sampleRate": UInt32(pv_sample_rate()),
                "version": String(cString: pv_porcupine_version())
            ]
            resolve(porcupineParameters)
        }
        else {
            reject(pvStatusToExceptionCode(status), "Failed to initialize Porcupine", nil)
        }
    }
    
    @objc(delete:)
    func delete(handle:String) -> Void {        
        if var porcupine = porcupinePool.removeValue(forKey: handle){
            pv_porcupine_delete(porcupine)
            porcupine = nil
        }
    }
    
    @objc(process:pcm:resolver:rejecter:)
    func process(handle:String, pcm:[Int16], 
        resolver resolve:RCTPromiseResolveBlock, rejecter reject:RCTPromiseRejectBlock) -> Void {
                
        if let porcupine = porcupinePool[handle]{
            var keywordIndex: Int32 = -1
            pv_porcupine_process(porcupine, pcm, &keywordIndex)        
            
            resolve(keywordIndex)
        }
        else{
            reject(pvStatusToExceptionCode(PV_STATUS_INVALID_STATE), "Invalid Porcupine handle provided to native module.", nil)
        }
    }

    private func getResourcePath(_ filePath: String) throws -> String {
        if (!FileManager.default.fileExists(atPath: filePath)) {
            if let resourcePath = Bundle.main.resourceURL?.appendingPathComponent("resources/\(filePath)").path {
                if (FileManager.default.fileExists(atPath: resourcePath)) {
                    return resourcePath
                }
            }
            
            throw PvPorcupineError.RuntimeError("Could not find file at path '\(filePath)'. If this is a packaged asset, ensure you have added it to your xcode project.")
        }
        
        return filePath
    }
    
    private func pvStatusToExceptionCode(_ status: pv_status_t) -> String {
        switch (status) {
        case PV_STATUS_OUT_OF_MEMORY:
            return "PorcupineMemoryException"
        case PV_STATUS_IO_ERROR:
            return "PorcupineIOException"
        case PV_STATUS_INVALID_ARGUMENT:
            return "PorcupineInvalidArgumentException"
        case PV_STATUS_STOP_ITERATION:
            return "PorcupineStopIterationException"
        case PV_STATUS_KEY_ERROR:
            return "PorcupineKeyException"
        case PV_STATUS_INVALID_STATE:
            return "PorcupineInvalidStateException"
        case PV_STATUS_RUNTIME_ERROR:
            return "PorcupineRuntimeException"
        case PV_STATUS_ACTIVATION_ERROR:
            return "PorcupineActivationException"
        case PV_STATUS_ACTIVATION_LIMIT_REACHED:
            return "PorcupineActivationLimitException"
        case PV_STATUS_ACTIVATION_THROTTLED:
            return "PorcupineActivationThrottledException"
        case PV_STATUS_ACTIVATION_REFUSED:
            return "PorcupineActivationRefusedException"
        default:
            return "PorcupineException"
        }
    }
}
