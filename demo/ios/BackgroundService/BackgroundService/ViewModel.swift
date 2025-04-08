//
//  Copyright 2025 Picovoice Inc.
//  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
//  file accompanying this source.
//  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
//  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
//  specific language governing permissions and limitations under the License.
//

import Combine
import Foundation
import ios_voice_processor
import Porcupine
import SwiftySound

class PorcupineViewModel: ObservableObject {
    @Published var isListening = false
    @Published var errorMessage: String?

    let accessKey = "${YOUR_ACCESS_KEY_HERE}" // Obtained from Picovoice Console (https://console.picovoice.ai)
    let wakeWord = Porcupine.BuiltInKeyword.porcupine

    var porcupineManager: PorcupineManager?

    func toggleListening() {
        if !isListening {
            startListening()
        } else {
            stopListening()
        }
    }

    func startListening() {
        guard VoiceProcessor.hasRecordAudioPermission else {
            VoiceProcessor.requestRecordAudioPermission { [weak self] isGranted in
                guard isGranted else {
                    DispatchQueue.main.async {
                        self?.showError(message: "Demo requires microphone permission")
                    }
                    return
                }

                DispatchQueue.main.async {
                    self?.startListening()
                }
            }
            return
        }

        NotificationManager.shared.requestNotificationAuthorization()

        let errorCallback: ((Error) -> Void) = { [weak self] error in
            self?.showError(message: "\(error)")
        }

        do {
            Sound.category = .playAndRecord
            let keywordCallback: ((Int32) -> Void) = { _ in
                NotificationManager.shared.sendNotification()
                Sound.play(file: "beep.wav")
            }

            self.porcupineManager = try PorcupineManager(
                accessKey: accessKey,
                keyword: wakeWord,
                onDetection: keywordCallback,
                errorCallback: errorCallback)

            try porcupineManager?.start()
            isListening = true

        } catch let error as PorcupineInvalidArgumentError {
            showError(message: "\(error.localizedDescription)")
        } catch is PorcupineActivationError {
            showError(message: "AccessKey activation error")
        } catch is PorcupineActivationRefusedError {
            showError(message: "AccessKey activation refused")
        } catch is PorcupineActivationLimitError {
            showError(message: "AccessKey reached its limit")
        } catch is PorcupineActivationThrottledError {
            showError(message: "AccessKey is throttled")
        } catch let error {
            showError(message: "\(error)")
        }
    }

    func stopListening() {
        do {
            try porcupineManager?.stop()
            isListening = false
        } catch {
            showError(message: "\(error)")
        }
    }

    private func showError(message: String) {
        errorMessage = message
    }
}
