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

struct TestData: Decodable {
    var tests: TestDataTests
}

struct TestDataTests: Decodable {
    var singleKeyword: [SingleKeywordTest]
    var multipleKeyword: [MultipleKeywordTest]
}

struct SingleKeywordTest: Decodable {
    var language: String
    var wakeword: String
}

struct MultipleKeywordTest: Decodable {
    var language: String
    var wakewords: [String]
    var groundTruth: [Int]
}

class PorcupineLanguageTests: BaseTest {
    func testWrapper() throws {
        let bundle = Bundle(for: type(of: self))
        let testDataJsonUrl = bundle.url(
            forResource: "test_data",
            withExtension: "json",
            subdirectory: "test_resources")!

        let testDataJsonData = try Data(contentsOf: testDataJsonUrl)
        let testData = try JSONDecoder().decode(TestData.self, from: testDataJsonData)

        for testCase in testData.tests.multipleKeyword {
            let suffix = testCase.language == "en" ? "" : "_\(testCase.language)"

            let language: String = testCase.language
            let modelPath: String = bundle.path(
                forResource: "porcupine_params\(suffix)",
                ofType: "pv",
                inDirectory: "test_resources/model_files")!
            var keywordPaths: [String] = []
            for keyword in testCase.wakewords {
                keywordPaths.append(bundle.path(
                    forResource: "\(keyword)_ios",
                    ofType: "ppn",
                    inDirectory: "test_resources/keyword_files/\(testCase.language)")!)
            }
            let testAudioPath: URL = bundle.url(
                forResource: "multiple_keywords\(suffix)",
                withExtension: "wav",
                subdirectory: "test_resources/audio_samples")!
            let expectedResults: [Int] = testCase.groundTruth

            try XCTContext.runActivity(named: "(\(language))") { _ in
                let p = try Porcupine.init(
                        accessKey: accessKey,
                        keywordPaths: keywordPaths,
                        modelPath: modelPath)
                XCTAssert(Porcupine.version != "")
                XCTAssert(Porcupine.frameLength > 0)
                XCTAssert(Porcupine.sampleRate > 0)

                let results = try processFile(p: p, testAudioURL: testAudioPath)

                XCTAssert(expectedResults.count == results.count)
                for i in 0..<expectedResults.count {
                    XCTAssert(expectedResults[i] == results[i])
                }

                p.delete()
            }
        }
    }
}
