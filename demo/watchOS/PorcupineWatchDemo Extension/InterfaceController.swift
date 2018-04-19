//
//  InterfaceController.swift
//  PorcupineWatchDemo Extension
//
//  Created by Dominique Yahyavi on 4/19/18.
//  Copyright © 2018 Picovoice. All rights reserved.
//

import WatchKit
import Foundation


class InterfaceController: WKInterfaceController {

    @IBOutlet var containerGroup: WKInterfaceGroup!
    @IBOutlet var wakeWordPicker: WKInterfacePicker!
    @IBOutlet var startToggleButton: WKInterfaceButton!
    var isRecording = false

    let wakeWords = [
        "Alexa", "Americano", "Avocado", "Blueberry", "Bumblebee", "Caterpillar", "Christina", "Dragonfly", "Flamingo",
        "Francesca", "Grapefruit", "Grasshopper", "Iguana", "Picovoice", "Pineapple", "Porcupine", "Raspberry",
        "Terminator", "Vancouver"]
    var wakeWord = "Alexa"

    
    override func awake(withContext context: Any?) {
        super.awake(withContext: context)
        
        
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
            startToggleButton.setTitle("Start")
        } else {
            startToggleButton.setTitle("Stop")
        }
        isRecording = !isRecording
    }
    
}
