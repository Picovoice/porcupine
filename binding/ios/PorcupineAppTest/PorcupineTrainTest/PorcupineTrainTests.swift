//
//  Copyright 2026 Picovoice Inc.
//  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
//  file accompanying this source.
//  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
//  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
//  specific language governing permissions and limitations under the License.
//

import AVFoundation
import XCTest
import Yams

import Porcupine

class PorcupineTrainTests: BaseTest {
    func testTrainModel() throws {
        let bundle = Bundle(for: type(of: self))
        let contextPath = bundle.path(forResource: "coffee_maker_ios", ofType: "rhn")!

        let fileManager = FileManager.default
        let urls = fileManager.urls(for: .documentDirectory, in: .userDomainMask)
        guard let documentsDirectory = urls.first else {
            XCTFail("Could not access document directory")
            return
        }
        let outputPath = documentsDirectory.appendingPathComponent("custom_coffee_maker_android.rhn").path

        let slots: [String: Set<String>] = ["size": ["macchiato", "cortado"]]

        try Porcupine.trainContextFromDynamicSlots(
            accessKey: accessKey,
            outputPath: outputPath,
            language: "en",
            contextPath: contextPath,
            modelPath: nil,
            slots: slots
        )

        let r = try Porcupine(
            accessKey: accessKey,
            contextPath: outputPath
        )

        XCTAssert(r.contextInfo != "")

        guard let content = try Yams.load(yaml: r.contextInfo) as? [String: Any],
              let context = content["context"] as? [String: Any],
              let contextSlots = context["slots"] as? [String: [String]] else {
            XCTFail("Failed to parse context information or locate slots dictionary")
            return
        }

        let expectedSlots = Array(slots["size"]!).sorted()
        let actualSlots = contextSlots["size"]?.sorted()
        XCTAssertEqual(expectedSlots, actualSlots)
    }

    func testTrainModelInvalidSlots() throws {
        let bundle = Bundle(for: type(of: self))
        let contextPath = bundle.path(forResource: "coffee_maker_ios", ofType: "rhn")!

        let fileManager = FileManager.default
        let urls = fileManager.urls(for: .documentDirectory, in: .userDomainMask)
        let outputPath = urls.first!.appendingPathComponent("custom_coffee_maker_android.rhn").path

        let slots: [String: Set<String>] = ["size": ["blue", "Blue"]]

        XCTAssertThrowsError(
            try Porcupine.trainContextFromDynamicSlots(
                accessKey: accessKey,
                outputPath: outputPath,
                language: "en",
                contextPath: contextPath,
                modelPath: nil,
                slots: slots
            )
        ) { error in
            XCTAssertTrue(error is PorcupineError)
        }
    }
}