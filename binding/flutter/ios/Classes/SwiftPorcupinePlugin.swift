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

enum Method: String {
    case FROM_BUILTIN_KEYWORDS
    case FROM_KEYWORD_PATHS
    case PROCESS
    case DELETE
}

public class SwiftPorcupinePlugin: NSObject, FlutterPlugin {
    private var porcupinePool: [String: Porcupine] = [:]

    public static func register(with registrar: FlutterPluginRegistrar) {
        let instance = SwiftPorcupinePlugin()

        let methodChannel = FlutterMethodChannel(name: "porcupine", binaryMessenger: registrar.messenger())
        registrar.addMethodCallDelegate(instance, channel: methodChannel)
    }

    public func handle(_ call: FlutterMethodCall, result: @escaping FlutterResult) {
        guard let method = Method(rawValue: call.method.uppercased()) else {
            result(errorToFlutterError(
                PorcupineRuntimeError("Porcupine method '\(call.method)' is not a valid function")))
            return
        }
        let args = call.arguments as! [String: Any]

        switch method {
        case .FROM_BUILTIN_KEYWORDS:
            do {
                if let accessKey = args["accessKey"] as? String,
                   let keywords = args["keywords"] as? [String] {
                    let modelPath = args["modelPath"] as? String
                    let sensitivities = args["sensitivities"] as? [Float]

                    var keywordValues: [Porcupine.BuiltInKeyword] = []
                    for keyword in keywords {
                        if let builtIn = Porcupine.BuiltInKeyword(rawValue: keyword.capitalized) {
                            keywordValues.append(builtIn)
                        } else {
                            result(errorToFlutterError(
                                PorcupineKeyError("'\(keyword.lowercased())' is not a built in keyword")))
                            return
                        }
                    }

                    let porcupine = try Porcupine(
                        accessKey: accessKey,
                        keywords: keywordValues,
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
                    result(errorToFlutterError(
                        PorcupineInvalidArgumentError("missing required arguments 'accessKey' and 'keywords'")))
                }
            } catch let error as PorcupineError {
                result(errorToFlutterError(error))
            } catch {
                result(errorToFlutterError(PorcupineError(error.localizedDescription)))
            }
        case .FROM_KEYWORD_PATHS:
            do {
                if let accessKey = args["accessKey"] as? String,
                   let keywordPaths = args["keywordPaths"] as? [String] {
                    let modelPath = args["modelPath"] as? String
                    let sensitivities = args["sensitivities"] as? [Float]

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
                    result(errorToFlutterError(
                        PorcupineInvalidArgumentError("missing required arguments 'accessKey' and 'keywordPaths'")))
                }
            } catch let error as PorcupineError {
                result(errorToFlutterError(error))
            } catch {
                result(errorToFlutterError(PorcupineError(error.localizedDescription)))
            }
        case .PROCESS:
            do {
                if let handle = args["handle"] as? String,
                   let frame = args["frame"] as? [Int16] {
                    if let porcupine = porcupinePool[handle] {
                        let keywordIndex = try porcupine.process(pcm: frame)
                        result(keywordIndex)
                    } else {
                        result(errorToFlutterError(
                            PorcupineRuntimeError("Invalid handle provided to Porcupine 'process'")))
                    }
                } else {
                    result(errorToFlutterError(PorcupineInvalidArgumentError("missing required arguments 'frame'")))
                }
            } catch let error as PorcupineError {
                result(errorToFlutterError(error))
            } catch {
                result(errorToFlutterError(PorcupineError(error.localizedDescription)))
            }
        case .DELETE:
            if let handle = args["handle"] as? String {
                if let porcupine = porcupinePool.removeValue(forKey: handle) {
                    porcupine.delete()
                }
            }
        }
    }

    private func errorToFlutterError(_ error: PorcupineError) -> FlutterError {
        return FlutterError(
            code: error.name.replacingOccurrences(of: "Error", with: "Exception"),
            message: error.localizedDescription, details: nil)
    }
}
