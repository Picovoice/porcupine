//
//  Copyright 2018-2023 Picovoice Inc.
//  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
//  file accompanying this source.
//  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
//  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
//  specific language governing permissions and limitations under the License.
//

import UIKit
import ios_voice_processor
import Porcupine
import SwiftySound

class ViewController: UIViewController, UITextViewDelegate {
    @IBOutlet weak var startButton: UIButton!
    @IBOutlet weak var textView: UITextView!
    @IBOutlet weak var errorPanel: UITextView!

    let accessKey = "${YOUR_ACCESS_KEY_HERE}" // Obtained from Picovoice Console (https://console.picovoice.ai)
    var wakeWord = Porcupine.BuiltInKeyword.porcupine

    var porcupineManager: PorcupineManager!
    var isListening = false

    override func viewDidLoad() {
        super.viewDidLoad()

        startButton.layer.cornerRadius = 0.5 * startButton.bounds.size.width
        startButton.clipsToBounds = true

        textView.text = "Press the Start button and say the wake word \"Porcupine\". " +
            "Try pressing the home button and saying it again."

        errorPanel.layer.cornerRadius = 10
        errorPanel.isHidden = true
    }

    @IBAction func toggleStartButton(_ sender: UIButton) {
        if !isListening {
            startListening()
        } else {
            stopListening()
        }
    }

    func startListening() {
        guard VoiceProcessor.hasRecordAudioPermission else {
            VoiceProcessor.requestRecordAudioPermission { isGranted in
                guard isGranted else {
                    DispatchQueue.main.async {
                        self.showErrorAlert(message: "Demo requires microphone permission")
                    }
                    return
                }

                DispatchQueue.main.async {
                    self.startListening()
                }
            }
            return
        }

        NotificationManager.shared.requestNotificationAuthorization()

        let errorCallback: ((Error) -> Void) = {error in
            self.showErrorAlert(message: "\(error)")
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

            try porcupineManager.start()
            isListening = true
            startButton.setTitle("STOP", for: UIControl.State.normal)

        } catch let error as PorcupineInvalidArgumentError {
            showErrorAlert(message: "\(error.localizedDescription)\nEnsure your accessKey '\(accessKey)' is valid")
        } catch is PorcupineActivationError {
            showErrorAlert(message: "AccessKey activation error")
        } catch is PorcupineActivationRefusedError {
            showErrorAlert(message: "AccessKey activation refused")
        } catch is PorcupineActivationLimitError {
            showErrorAlert(message: "AccessKey reached its limit")
        } catch is PorcupineActivationThrottledError {
            showErrorAlert(message: "AccessKey is throttled")
        } catch let error {
            showErrorAlert(message: "\(error)")
        }

    }

    func stopListening() {
        do {
            try porcupineManager.stop()
        } catch {
            showErrorAlert(message: "\(error)")
            return
        }
        isListening = false
        startButton.setTitle("START", for: UIControl.State.normal)
    }

    func showErrorAlert(message: String) {
        errorPanel.text = message
        errorPanel.isHidden = false
        startButton.isEnabled = false
    }
}
