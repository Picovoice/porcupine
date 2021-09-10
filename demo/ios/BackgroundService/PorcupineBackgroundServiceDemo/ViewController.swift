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
    
    var wakeWord = Porcupine.BuiltInKeyword.porcupine

    var porcupineManager: PorcupineManager!
    var isRecording = false
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        startButton.layer.cornerRadius = 0.5 * startButton.bounds.size.width
        startButton.clipsToBounds = true
        
        textView.text = "Press the Start button and say the wake word \"Porcupine\". Try pressing the home button and saying it again."
        
        Sound.category = .playAndRecord
        let keywordCallback: ((Int32) -> Void) = { keywordIndex in
            NotificationManager.shared.sendNotification()
            Sound.play(file: "beep.wav")
        }
        do {
            self.porcupineManager = try PorcupineManager(keyword: wakeWord, onDetection: keywordCallback)
        } catch {
            showAlert(message: "Failed to initialize Porcupine Manager")
        }
    }

    @IBAction func toggleStartButton(_ sender: UIButton) {
        if !isRecording {
            NotificationManager.shared.requestNotificationAuthorization()

            do {
                try porcupineManager.start()
            } catch {
                showAlert(message: "Failed to start Porcupine Manager")
                return
            }

            isRecording = true
            startButton.setTitle("STOP", for: UIControl.State.normal)
        } else {
            porcupineManager.stop()

            isRecording = false
            startButton.setTitle("START", for: UIControl.State.normal)
        }
    }
    
    private func showAlert(message: String) {
        let alert = UIAlertController(
                title: "Alert",
                message: message,
                preferredStyle: UIAlertController.Style.alert)
        alert.addAction(UIAlertAction(title: "Click", style: UIAlertAction.Style.default, handler: nil))
        self.present(alert, animated: true, completion: nil)
    }
}
