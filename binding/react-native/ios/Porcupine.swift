//
// Copyright 2020 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
// file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//

import PvPorcupine

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

    @objc(create:keywordPaths:sensitivities:resolver:rejecter:)
    func create(modelPath: String, keywordPaths: [String], sensitivities: [Float32], 
        resolver resolve:RCTPromiseResolveBlock, rejecter reject:RCTPromiseRejectBlock) -> Void {                
        
        var porcupine:OpaquePointer?
        let status = pv_porcupine_init(
            modelPath,
            Int32(keywordPaths.count),
            keywordPaths.map { UnsafePointer(strdup($0)) },
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
            let pvStatus = String(cString: pv_status_to_string(status))
            reject("PvPorcupine:create", "Could not create a new instance of Porcupine: \(pvStatus)", nil)            
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
            reject("PvPorcupine:process", "Invalid Porcupine handle provided to native module.", nil)
        }
    }
}
