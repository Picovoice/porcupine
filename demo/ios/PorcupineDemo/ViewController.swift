//
// Copyright 2018 Picovoice Inc.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

import UIKit

class ViewController: UIViewController, UIPickerViewDelegate, UIPickerViewDataSource {
    @IBOutlet weak var wakeWordPicker: UIPickerView!
    @IBOutlet weak var startButton: UIButton!

    let wakeWords = [
        "Hey Pico", "Americano", "Blueberry", "Bumblebee", "Grapefruit", "Grasshopper", "Picovoice", "Porcupine", "Terminator"]
    var wakeWord = "Picovoice"

    var porcupineManager: PorcupineManager!
    var isRecording = false

    override func viewDidLoad() {
        super.viewDidLoad()

        wakeWordPicker.delegate = self
        wakeWordPicker.dataSource = self
        
        let viewSize = view.frame.size
        let startButtonSize = CGSize(width: 120, height: 120)
        startButton.frame.size = startButtonSize
        startButton.frame.origin =
            CGPoint(x: (viewSize.width - startButtonSize.width) / 2, y: viewSize.height - (startButtonSize.height + 40))
        startButton.layer.cornerRadius = 0.5 * startButton.bounds.size.width
        startButton.clipsToBounds = true

        wakeWordPicker.frame.origin = CGPoint(x: 0, y: 40)
        wakeWordPicker.frame.size = CGSize(width: viewSize.width, height: viewSize.height - startButtonSize.height - 120)
    }

    func numberOfComponents(in pickerView: UIPickerView) -> Int {
        return 1
    }

    func pickerView(_ pickerView: UIPickerView, numberOfRowsInComponent component: Int) -> Int {
        return wakeWords.count
    }

    func pickerView(_ pickerView: UIPickerView, titleForRow row: Int, forComponent component: Int) -> String? {
        return wakeWords[row]
    }

    func pickerView(_ pickerView: UIPickerView, didSelectRow row: Int, inComponent component: Int) {
        wakeWord = wakeWords[row]
    }

    @IBAction func toggleStartButton(_ sender: UIButton) {
        if !isRecording {
            let modelFilePath = Bundle.main.path(forResource: "porcupine_params", ofType: "pv")
            let keywordFilePath = Bundle.main.path(forResource: wakeWord.lowercased() + "_ios", ofType: "ppn")

            let originalColor = self.view.backgroundColor
            let keywordCallback: ((WakeWordConfiguration) -> Void) = { word in
                self.view.backgroundColor = UIColor.orange
                DispatchQueue.main.asyncAfter(deadline: .now() + 1.0) {
                    self.view.backgroundColor = originalColor
                }
            }

            let keyword = WakeWordConfiguration(name: wakeWord, filePath: keywordFilePath!, sensitivity: 0.5)

            do {
                porcupineManager = try PorcupineManager(modelFilePath: modelFilePath!, wakeKeywordConfiguration: keyword, onDetection: keywordCallback)
                try porcupineManager.startListening()
            } catch {
                let alert = UIAlertController(
                        title: "Alert",
                        message: "Something went wrong",
                        preferredStyle: UIAlertControllerStyle.alert)
                alert.addAction(UIAlertAction(title: "Click", style: UIAlertActionStyle.default, handler: nil))
                self.present(alert, animated: true, completion: nil)
                return
            }

            wakeWordPicker.isUserInteractionEnabled = false
            isRecording = true
            startButton.setTitle("STOP", for: UIControlState.normal)
        } else {
            porcupineManager.stopListening()

            wakeWordPicker.isUserInteractionEnabled = true
            isRecording = false
            startButton.setTitle("START", for: UIControlState.normal)
        }
    }

}
