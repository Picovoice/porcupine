//
//  InterfaceController.swift
//  PorcupineWatchDemo Extension
//
//  Created by Dominique Yahyavi on 4/19/18.
//  Copyright © 2018 Picovoice. All rights reserved.
//

import WatchKit
import Foundation
import AVFoundation

class InterfaceController: WKInterfaceController {

    @IBOutlet var containerGroup: WKInterfaceGroup!
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
            porcupineManager.stop()
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
        
        let keywordCallback: (() -> Void) = {
            DispatchQueue.main.async {
                WKInterfaceDevice.current().play(.start)
                self.containerGroup.setBackgroundColor(.orange)
                DispatchQueue.main.asyncAfter(deadline: .now() + 1, execute: {
                    self.containerGroup.setBackgroundColor(.clear)
                })
            }
            print("found!")
        }
        
        porcupineManager = PorcupineManager(
            modelFilePath: modelFilePath!,
            keywordFilePath: keywordFilePath!,
            sensitivity: 0.5,
            keywordCallback: keywordCallback)
        
        do {
            try porcupineManager.start()
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
