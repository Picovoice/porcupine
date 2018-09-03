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


import WatchKit
import Foundation
import AVFoundation

class InterfaceController: WKInterfaceController {

    @IBOutlet var wakeWordPicker: WKInterfacePicker!
    @IBOutlet var startToggleButton: WKInterfaceButton!
    var porcupineManager: PorcupineManager!
    var isRecording = false

    let wakeWords = [
        "Alexa", "Americano", "Avocado", "Blueberry", "Bumblebee", "Caterpillar", "Christina", "Dragonfly", "Flamingo",
        "Francesca", "Grapefruit", "Grasshopper", "Iguana", "Picovoice", "Pineapple", "Porcupine", "Raspberry",
        "Terminator", "Vancouver"]
    var wakeWord = "Alexa"

    
    override func awake(withContext context: Any?) {
        super.awake(withContext: context)
        initializeAudioSession()
        
        let pickerItems: [WKPickerItem] = wakeWords.map {
            let pickerItem = WKPickerItem()
            pickerItem.title = $0
            return pickerItem
        }
        
        wakeWordPicker.setItems(pickerItems)
        wakeWordPicker.focus()
    }
    
    @IBAction func wakePickerDidSelectItem(_ value: Int) {
        wakeWord = wakeWords[value]
        print(wakeWord)
    }

    @IBAction func startTogglePressed() {
        if isRecording {
            wakeWordPicker.setEnabled(true)
            porcupineManager.stopListening()
            startToggleButton.setTitle("Start")
        } else {
            wakeWordPicker.setEnabled(false)
            startPorcupineManager()
            startToggleButton.setTitle("Stop")
        }
        isRecording = !isRecording
    }
    
    
    private func startPorcupineManager() {
        print("starting porcupine manager")
        let modelFilePath = Bundle.main.path(forResource: "porcupine_params", ofType: "pv")
        let keywordFilePath = Bundle.main.path(forResource: wakeWord.lowercased() + "_ios", ofType: "ppn")
        
        let keywordCallback: ((WakeWordConfiguration) -> Void) = { word in
            DispatchQueue.main.async {
                WKInterfaceDevice.current().play(.start)
                self.startToggleButton.setBackgroundColor(.orange)
                DispatchQueue.main.asyncAfter(deadline: .now() + 1, execute: {
                    self.startToggleButton.setBackgroundColor(.darkGray)
                })
            }
            print("found!")
        }

        let keyword = WakeWordConfiguration(name: wakeWord, filePath: keywordFilePath!, sensitivity: 0.5)

        do {
            porcupineManager = try PorcupineManager(modelFilePath: modelFilePath!, wakeKeywordConfiguration: keyword, onDetection: keywordCallback)
            try porcupineManager.startListening()
        } catch {
            print("Error starting manager")
        }
        
    }

    private func initializeAudioSession() {
        let recordingSession = AVAudioSession.sharedInstance()
        do {
            try recordingSession.setCategory(AVAudioSessionCategoryPlayAndRecord)
            try recordingSession.setActive(true)
            recordingSession.requestRecordPermission { (allowed) in
                DispatchQueue.main.async {
                    if allowed {
                        print("allowed")
                    } else {
                        print("disallowed")
                    }
                }
            }
        } catch {
            print("error intializing audio session")
        }
    }
    
}
