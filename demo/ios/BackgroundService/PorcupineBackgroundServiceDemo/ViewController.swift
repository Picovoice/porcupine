//
//  Copyright 2018-2021 Picovoice Inc.
//  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
//  file accompanying this source.
//  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
//  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
//  specific language governing permissions and limitations under the License.
//

import UIKit
import Porcupine
import SwiftySound

class ViewController: UIViewController, UITextViewDelegate {
    @IBOutlet weak var startButton: UIButton!
    @IBOutlet weak var textView: UITextView!
    @IBOutlet weak var errorPanel: UITextView!

    let accessKey = "${YOUR_ACCESS_KEY_HERE}" // Obtained from Picovoice Console (https://console.picovoice.ai)
    var wakeWord = Porcupine.BuiltInKeyword.porcupine

    var porcupineManager: PorcupineManager!
    var isRecording = false

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
        if !isRecording {
            NotificationManager.shared.requestNotificationAuthorization()

            do {
                Sound.category = .playAndRecord
                let keywordCallback: ((Int32) -> Void) = { _ in
                    NotificationManager.shared.sendNotification()
                    Sound.play(file: "beep.wav")
                }

                self.porcupineManager = try PorcupineManager(
                    accessKey: accessKey,
                    keyword: wakeWord,
                    onDetection: keywordCallback)

                try porcupineManager.start()
                isRecording = true
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

        } else {
            porcupineManager.stop()

            isRecording = false
            startButton.setTitle("START", for: UIControl.State.normal)
        }
    }

    private func showErrorAlert(message: String) {
        errorPanel.text = message
        errorPanel.isHidden = false
        startButton.isEnabled = false
    }
}
