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

import Flutter
import UIKit
import pv_porcupine

public class SwiftPorcupinePlugin: NSObject, FlutterPlugin {
  public static func register(with registrar: FlutterPluginRegistrar) {
    
  }


  // these dummy function calls are required to prevent the linker from 
  // stripping the porcupine static library
  public func doNotCallThisFunction_porcpuineVersion(){
    pv_porcupine_version();
  }

  public func doNotCallThisFunction_porcupineInit(modelPath: String, keywordPaths: [String], sensitivities: [Float32]){
    var porcupine:OpaquePointer?
    let status = pv_porcupine_init(
        modelPath,
        Int32(keywordPaths.count),
        keywordPaths.map { UnsafePointer(strdup($0)) },
        sensitivities,
        &porcupine)
  }
  
  public func doNotCallThisFunction_porcupineFrameLength(){
    pv_porcupine_frame_length()
  }

  public func doNotCallThisFunction_porcupineSampleRate(){
    pv_sample_rate()
  }

  public func doNotCallThisFunction_porcupineProcess(porcupine:OpaquePointer?, pcm:[Int16]){
    var keywordIndex: Int32 = -1
    pv_porcupine_process(porcupine, pcm, &keywordIndex)   
  }

  public func doNotCallThisFunction_porcupineDelete(porcupine:OpaquePointer?){
    pv_porcupine_delete(porcupine);
  }
}
