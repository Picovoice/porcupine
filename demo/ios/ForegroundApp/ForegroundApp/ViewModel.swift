//
//  Copyright 2025 Picovoice Inc.
//  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
//  file accompanying this source.
//  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
//  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
//  specific language governing permissions and limitations under the License.
//

import Foundation
import ios_voice_processor
import Porcupine
import SwiftUI

class ViewModel: ObservableObject {
    @Published var isListening = false
    @Published var wakeWordKeys: [String] = []
    @Published var selectedWakeWord: String = ""
    @Published var errorMessage: String?
    @Published var backgroundColor: Color = Color(UIColor.systemBackground)

    let accessKey = "${YOUR_ACCESS_KEY_HERE}" // Obtained from Picovoice Console (https://console.picovoice.ai)
    let language: String = ProcessInfo.processInfo.environment["LANGUAGE"] ?? "en"

    private var porcupineManager: PorcupineManager?

    func setupWakeWords() {
        if language == "en" {
            wakeWordKeys = Porcupine.BuiltInKeyword.allCases.map { $0.rawValue }
        } else {
            if let keywordUrls = Bundle.main.urls(forResourcesWithExtension: "ppn", subdirectory: "keywords") {
                wakeWordKeys = keywordUrls.map {
                    $0.lastPathComponent
                        .replacingOccurrences(of: "_ios.ppn", with: "")
                        .replacingOccurrences(of: "_", with: " ")
                }
            }
        }
        selectedWakeWord = wakeWordKeys.first ?? ""
    }

    func toggleListening() {
        if isListening {
            stopListening()
        } else {
            startListening()
        }
    }

    func startListening() {
        guard VoiceProcessor.hasRecordAudioPermission else {
            VoiceProcessor.requestRecordAudioPermission { isGranted in
                if isGranted {
                    DispatchQueue.main.async {
                        self.startListening()
                    }
                } else {
                    DispatchQueue.main.async {
                        self.showError("Microphone permission is required")
                    }
                }
            }
            return
        }

        let keywordCallback: ((Int32) -> Void) = { _ in
            DispatchQueue.main.async {
                self.backgroundColor = .orange
                DispatchQueue.main.asyncAfter(deadline: .now() + 1.0) {
                    self.backgroundColor = Color(UIColor.systemBackground)
                }
            }
        }

        let errorCallback: ((Error) -> Void) = { error in
            DispatchQueue.main.async {
                self.showError(error.localizedDescription)
            }
        }

        do {
            if language == "en" {
                guard let keyword = Porcupine.BuiltInKeyword(rawValue: selectedWakeWord)
                else {
                    showError("Keyword not found")
                    return
                }
                porcupineManager = try PorcupineManager(
                    accessKey: accessKey,
                    keyword: keyword,
                    onDetection: keywordCallback,
                    errorCallback: errorCallback
                )
            } else {
                guard let keywordUrl = Bundle.main.url(
                        forResource: "\(selectedWakeWord.lowercased())_ios",
                        withExtension: "ppn",
                        subdirectory: "keywords")
                else {
                    showError("Keyword file keywords/\(selectedWakeWord.lowercased())_ios.ppn not found")
                    return
                }

                guard let modelUrl = Bundle.main.url(
                        forResource: "porcupine_params_\(language)",
                        withExtension: "pv",
                        subdirectory: "models")
                else {
                    showError("Model file models/porcupine_params_\(language).pv not found")
                    return
                }

                porcupineManager = try PorcupineManager(
                    accessKey: accessKey,
                    keywordPath: keywordUrl.path,
                    modelPath: modelUrl.path,
                    onDetection: keywordCallback,
                    errorCallback: errorCallback
                )
            }

            try porcupineManager?.start()
            isListening = true
        } catch let error as PorcupineInvalidArgumentError {
            showError(error.localizedDescription)
        } catch is PorcupineActivationError {
            showError("AccessKey activation error")
        } catch is PorcupineActivationRefusedError {
            showError("AccessKey activation refused")
        } catch is PorcupineActivationLimitError {
            showError("AccessKey limit reached")
        } catch is PorcupineActivationThrottledError {
            showError("AccessKey is throttled")
        } catch {
            showError(error.localizedDescription)
        }
    }

    func stopListening() {
        do {
            try porcupineManager?.stop()
        } catch {
            showError(error.localizedDescription)
        }
        isListening = false
    }

    func showError(_ message: String) {
        errorMessage = message
    }
}
