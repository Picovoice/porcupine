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

import Flutter
import UIKit
import Porcupine

enum Method : String {
    case INIT
    case CREATE
    case PROCESS
    case DELETE
    case DEFAULT
}

public class SwiftPorcupinePlugin: NSObject, FlutterPlugin {
    private var porcupinePool:Dictionary<String, Porcupine> = [:]
    
    public static func register(with registrar: FlutterPluginRegistrar) {
        let instance = SwiftPorcupinePlugin()

        let methodChannel = FlutterMethodChannel(name: "porcupine", binaryMessenger: registrar.messenger())
        registrar.addMethodCallDelegate(instance, channel: methodChannel)
    }
    
    public func handle(_ call: FlutterMethodCall, result: @escaping FlutterResult) {
        let method = Method(rawValue: call.method.uppercased()) ?? Method.DEFAULT
        
        switch (method) {
        case .INIT:
            var constants: [String: Any] = [:]
            
            let modelPath: String = Bundle(for: Porcupine.self).path(forResource: "porcupine_params", ofType: "pv") ?? "unknown"
            constants["DEFAULT_MODEL_PATH"] = modelPath
                    
            let keywordPaths = Bundle(for: Porcupine.self).paths(forResourcesOfType: "ppn", inDirectory: nil)
            var keywordDict:Dictionary<String, String> = [:]
            for keywordPath in keywordPaths{
                let keywordName = URL(fileURLWithPath:keywordPath).lastPathComponent.components(separatedBy:"_")[0]
                keywordDict[keywordName] = keywordPath
            }
            constants["KEYWORD_PATHS"] = keywordDict
            
            result(constants)
        case .CREATE:
            do {
                let args = call.arguments as! [String: Any]
                
                if let accessKey = args["accessKey"] as? String,
                   let modelPath = args["modelPath"] as? String,
                   let keywordPaths = args["keywordPaths"] as? [String],
                   let sensitivities = args["sensitivities"] as? [Float] {
                    
                    let porcupine = try Porcupine(
                        accessKey: accessKey,
                        keywordPaths: keywordPaths,
                        modelPath: modelPath,
                        sensitivities: sensitivities)
                    
                    let handle: String = String(describing: porcupine)
                    porcupinePool[handle] = porcupine
                    
                    var param: [String: Any] = [:]
                    param["handle"] = handle
                    param["frameLength"] = Porcupine.frameLength
                    param["sampleRate"] = Porcupine.sampleRate
                    param["version"] = Porcupine.version
                    
                    result(param)
                } else {
                    result(errorToFlutterError(PorcupineError.PorcupineInvalidArgumentError("Invalid argument provided to Porcupine 'create'")))
                }
            } catch let error as PorcupineError {
                result(errorToFlutterError(error))
            } catch {
                result(errorToFlutterError(PorcupineError.PorcupineInternalError(error.localizedDescription)))
            }
            break
        case .PROCESS:
            do {
                let args = call.arguments as! [String: Any]
                
                if let handle = args["handle"] as? String,
                   let frame = args["frame"] as? [Int16] {
                    if let porcupine = porcupinePool[handle] {
                        let keywordIndex = try porcupine.process(pcm: frame)
                        result(keywordIndex)
                    } else {
                        result(errorToFlutterError(PorcupineError.PorcupineRuntimeError("Invalid handle provided to Porcupine 'process'")))
                    }
                } else {
                    result(errorToFlutterError(PorcupineError.PorcupineInvalidArgumentError("Invalid argument provided to Porcupine 'process'")))
                }
            } catch let error as PorcupineError {
                result(errorToFlutterError(error))
            } catch {
                result(errorToFlutterError(PorcupineError.PorcupineInternalError(error.localizedDescription)))
            }
            break
        case .DELETE:
            let args = call.arguments as! [String: Any]
            
            if let handle = args["handle"] as? String {
                if let porcupine = porcupinePool.removeValue(forKey: handle) {
                    porcupine.delete()
                }
            }
            break
        default:
            result(errorToFlutterError(PorcupineError.PorcupineRuntimeError("Porcupine method '\(call.method)' is not a valid function.")))
        }
    }
    
    private func errorToFlutterError(_ error: PorcupineError) -> FlutterError {
        switch(error) {
        case .PorcupineOutOfMemoryError:
            return FlutterError(code: "PorcupineMemoryException", message: extractMessage("\(error)"), details: nil)
        case .PorcupineIOError:
            return FlutterError(code: "PorcupineIOException", message: extractMessage("\(error)"), details: nil)
        case .PorcupineInvalidArgumentError:
            return FlutterError(code: "PorcupineInvalidArgumentException", message: extractMessage("\(error)"), details: nil)
        case .PorcupineStopIterationError:
            return FlutterError(code: "PorcupineStopIterationException", message: extractMessage("\(error)"), details: nil)
        case .PorcupineKeyError:
            return FlutterError(code: "PorcupineKeyException", message: extractMessage("\(error)"), details: nil)
        case .PorcupineInvalidStateError:
            return FlutterError(code: "PorcupineInvalidStateException", message: extractMessage("\(error)"), details: nil)
        case .PorcupineRuntimeError:
            return FlutterError(code: "PorcupineRuntimeException", message: extractMessage("\(error)"), details: nil)
        case .PorcupineActivationError:
            return FlutterError(code: "PorcupineActivationException", message: extractMessage("\(error)"), details: nil)
        case .PorcupineActivationLimitError:
            return FlutterError(code: "PorcupineActivationLimitException", message: extractMessage("\(error)"), details: nil)
        case .PorcupineActivationThrottledError:
            return FlutterError(code: "PorcupineActivationThrottledException", message: extractMessage("\(error)"), details: nil)
        case .PorcupineActivationRefusedError:
            return FlutterError(code: "PorcupineActivationRefusedException", message: extractMessage("\(error)"), details: nil)
        case .PorcupineInternalError:
            return FlutterError(code: "PorcupineException", message: extractMessage("\(error)"), details: nil)
        }
    }
    
    private func extractMessage(_ errorMessage: String) -> String {
        let parts = errorMessage.components(separatedBy: "\"")
        if (parts.count > 2) {
            if let message = parts.dropFirst().first {
                return message
            }
        }
        return errorMessage
    }
}
