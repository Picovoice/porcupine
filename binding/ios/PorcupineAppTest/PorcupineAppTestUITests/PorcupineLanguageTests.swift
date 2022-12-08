//
//  Copyright 2022 Picovoice Inc.
//  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
//  file accompanying this source.
//  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
//  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
//  specific language governing permissions and limitations under the License.
//

import AVFoundation
import XCTest

import Porcupine

class PorcupineLanguageTests: BaseTest {

    static var testData: [[Any]] = [
        ["en",
         ["americano", "blueberry", "bumblebee", "grapefruit", "grasshopper", "picovoice", "porcupine", "terminator"],
         [6, 0, 1, 2, 3, 4, 5, 6, 7]],
        ["es", ["emparedado", "leopardo", "manzana"], [0, 1, 2]],
        ["de", ["ananas", "heuschrecke", "leguan", "stachelschwein"], [0, 1, 2, 3]],
        ["fr", ["framboise", "mon chouchou", "parapluie"], [0, 1, 0, 2]],
        ["it", ["espresso", "cameriere", "porcospino"], [2, 0, 1]],
        ["ja", ["ninja", "bushi", "ringo"], [2, 1, 0]],
        ["ko", ["aiseukeulim", "bigseubi", "koppulso"], [1, 2, 0]],
        ["pt", ["abacaxi", "fenomeno", "formiga"], [0, 2, 1]]
    ]

    var language: String = ""
    var modelPath: String = ""
    var keywordPaths: [String] = []
    var testAudioPath: URL? = URL(string: "")
    var expectedResults: [Int] = []

    override class var defaultTestSuite: XCTestSuite {
        get {
            let xcTestSuite = XCTestSuite(name: NSStringFromClass(self))
            let bundle = Bundle(for: self)

            for testCase in testData {
                let suffix = (testCase[0]) as! String == "en" ? "" : "_\(testCase[0])"
                for invocation in testInvocations {
                    let newTestCase = PorcupineLanguageTests(invocation: invocation)

                    var keywordFiles: [String] = []
                    for keyword in testCase[1] as! [String] {
                        keywordFiles.append(bundle.path(forResource: "\(keyword)_ios", ofType: "ppn")!)
                    }

                    newTestCase.language = testCase[0] as! String
                    newTestCase.modelPath = bundle.path(forResource: "porcupine_params\(suffix)", ofType: "pv")!
                    newTestCase.keywordPaths = keywordFiles
                    newTestCase.testAudioPath = bundle.url(
                        forResource: "multiple_keywords\(suffix)",
                        withExtension: "wav")!
                    newTestCase.expectedResults = testCase[2] as! [Int]
                    xcTestSuite.addTest(newTestCase)
                }
            }

            return xcTestSuite
        }
    }

    func testWrapper() throws {
        let testResults = try XCTContext.runActivity(named: "(\(language))") { _ -> [Int32] in
            let p = try Porcupine.init(
                    accessKey: accessKey,
                    keywordPaths: keywordPaths,
                    modelPath: modelPath)
            XCTAssert(Porcupine.version != "")
            XCTAssert(Porcupine.frameLength > 0)
            XCTAssert(Porcupine.sampleRate > 0)

            let ret = try processFile(p: p, testAudioURL: testAudioPath!)
            p.delete()
            return ret
        }

        XCTAssert(expectedResults.count == testResults.count)
        for i in 0..<expectedResults.count {
            XCTAssert(expectedResults[i] == testResults[i])
        }
    }
}
