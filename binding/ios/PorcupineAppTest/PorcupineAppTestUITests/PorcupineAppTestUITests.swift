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

class PorcupineAppTestUITests: BaseTest {

    func testInitSuccessWithSingleBuiltIn() throws {
        let p = try Porcupine.init(accessKey: accessKey, keyword: Porcupine.BuiltInKeyword.porcupine)
        XCTAssert(Porcupine.version != "")
        XCTAssert(Porcupine.frameLength > 0)
        XCTAssert(Porcupine.sampleRate > 0)
        p.delete()
    }

    func testInitSuccessWithMultipleBuiltIns() throws {
        let keywords = [
            Porcupine.BuiltInKeyword.alexa,
            Porcupine.BuiltInKeyword.grasshopper,
            Porcupine.BuiltInKeyword.picovoice,
            Porcupine.BuiltInKeyword.terminator
        ]

        let p = try Porcupine.init(accessKey: accessKey, keywords: keywords)
        p.delete()
    }

    func testInitSuccessWithCustomArguments() throws {
        let bundle = Bundle(for: type(of: self))
        let keywordPath = bundle.path(
            forResource: "hey barista_ios",
            ofType: "ppn",
            inDirectory: "test_resources/keyword_files/en")!
        let modelPath = bundle.path(
            forResource: "porcupine_params",
            ofType: "pv",
            inDirectory: "test_resources/model_files")

        let p = try Porcupine.init(accessKey: accessKey, keywordPath: keywordPath, modelPath: modelPath)
        p.delete()
    }

    func testInitSuccessWithMultipleCustomKeywordsAndSensitivities() throws {
        let bundle = Bundle(for: type(of: self))
        let keywordPath = bundle.path(
            forResource: "hey barista_ios",
            ofType: "ppn",
            inDirectory: "test_resources/keyword_files/en")!
        let keywordPath2 = bundle.path(
            forResource: "pico clock_ios",
            ofType: "ppn",
            inDirectory: "test_resources/keyword_files/en")!

        let p = try Porcupine.init(
            accessKey: accessKey,
            keywordPaths: [keywordPath, keywordPath2],
            sensitivities: [0.35, 0.6])
        p.delete()
    }

    func testInitFailWithMismatchedLanguage() throws {
        let bundle = Bundle(for: type(of: self))
        let keywordPath = bundle.path(
            forResource: "ananas_ios",
            ofType: "ppn",
            inDirectory: "test_resources/keyword_files/de")!
        let modelPath = bundle.path(
            forResource: "porcupine_params",
            ofType: "pv",
            inDirectory: "test_resources/model_files")!

        var didFail = false
        do {
            _ = try Porcupine.init(accessKey: accessKey, keywordPath: keywordPath, modelPath: modelPath)
        } catch {
            didFail = true
        }

        XCTAssert(didFail)
    }

    func testInitFailWithInvalidModelPath() throws {

        var didFail = false
        do {
            _ = try Porcupine.init(
                accessKey: accessKey,
                keyword: Porcupine.BuiltInKeyword.porcupine,
                modelPath: "bad_path/bad_path.pv")
        } catch {
            didFail = true
        }

        XCTAssert(didFail)
    }

    func testInitFailWithInvalidSensitivity() throws {

        var didFail = false
        do {
            _ = try Porcupine.init(accessKey: accessKey, keyword: Porcupine.BuiltInKeyword.porcupine, sensitivity: 10)
        } catch {
            didFail = true
        }

        XCTAssert(didFail)
    }

    func testInitFailWithMismatchedSensitivityCount() throws {

        let keywords = [Porcupine.BuiltInKeyword.porcupine, Porcupine.BuiltInKeyword.grasshopper]
        let sensitivities: [Float32] = [0.3, 0.8, 0.4]
        var didFail = false
        do {
            _ = try Porcupine.init(accessKey: accessKey, keywords: keywords, sensitivities: sensitivities)
        } catch {
            didFail = true
        }

        XCTAssert(didFail)
    }

    func testInitFailWithNoKeywords() throws {

        let keywords: [Porcupine.BuiltInKeyword] = []
        var didFail = false
        do {
            _ = try Porcupine.init(accessKey: accessKey, keywords: keywords)
        } catch {
            didFail = true
        }

        XCTAssert(didFail)
    }

    func testInitFailWithInvalidKeywordPath() throws {

        var didFail = false
        do {
            _ = try Porcupine.init(accessKey: accessKey, keywordPath: "bad_path/bad_path.ppn")
        } catch {
            didFail = true
        }

        XCTAssert(didFail)
    }

    func testInitFailWithWrongKeywordPlatform() throws {
        let bundle = Bundle(for: type(of: self))
        let keywordPath = bundle.path(
            forResource: "alexa_linux",
            ofType: "ppn",
            inDirectory: "test_resources/keyword_files/en")!

        var didFail = false
        do {
            _ = try Porcupine.init(accessKey: accessKey, keywordPath: keywordPath)
        } catch {
            didFail = true
        }

        XCTAssert(didFail)
    }

    func testInitWithNonAsciiModelName() throws {
        let bundle = Bundle(for: type(of: self))
        let keywordPath = bundle.path(
            forResource: "murciélago_ios",
            ofType: "ppn",
            inDirectory: "test_resources/keyword_files/es")!
        let modelPath = bundle.path(
            forResource: "porcupine_params_es",
            ofType: "pv",
            inDirectory: "test_resources/model_files")!

        let p = try Porcupine.init(accessKey: accessKey, keywordPath: keywordPath, modelPath: modelPath)
        p.delete()
    }

    func testProcSuccessWithSingleBuiltIn() throws {
        let p: Porcupine = try Porcupine.init(accessKey: accessKey, keyword: Porcupine.BuiltInKeyword.porcupine)

        let bundle = Bundle(for: type(of: self))
        let fileURL: URL = bundle.url(
            forResource: "porcupine",
            withExtension: "wav",
            subdirectory: "test_resources/audio_samples")!

        let results = try processFile(p: p, testAudioURL: fileURL)
        p.delete()

        XCTAssert(results.count == 1 && results[0] == 0)
    }

    func testProcSuccessWithMultipleBuiltIns() throws {
        let keywords = [
            Porcupine.BuiltInKeyword.alexa,
            Porcupine.BuiltInKeyword.americano,
            Porcupine.BuiltInKeyword.blueberry,
            Porcupine.BuiltInKeyword.bumblebee,
            Porcupine.BuiltInKeyword.grapefruit,
            Porcupine.BuiltInKeyword.grasshopper,
            Porcupine.BuiltInKeyword.picovoice,
            Porcupine.BuiltInKeyword.porcupine,
            Porcupine.BuiltInKeyword.terminator
        ]

        let p: Porcupine = try Porcupine.init(accessKey: accessKey, keywords: keywords)

        let bundle = Bundle(for: type(of: self))
        let fileURL: URL = bundle.url(
            forResource: "multiple_keywords",
            withExtension: "wav",
            subdirectory: "test_resources/audio_samples")!

        let results = try processFile(p: p, testAudioURL: fileURL)
        p.delete()

        let expectedResults = [
            Porcupine.BuiltInKeyword.porcupine,
            Porcupine.BuiltInKeyword.alexa,
            Porcupine.BuiltInKeyword.americano,
            Porcupine.BuiltInKeyword.blueberry,
            Porcupine.BuiltInKeyword.bumblebee,
            Porcupine.BuiltInKeyword.grapefruit,
            Porcupine.BuiltInKeyword.grasshopper,
            Porcupine.BuiltInKeyword.picovoice,
            Porcupine.BuiltInKeyword.porcupine,
            Porcupine.BuiltInKeyword.terminator
        ]
        XCTAssert(expectedResults.count == results.count)
        for i in 0..<expectedResults.count {
            let expectedKeyword = expectedResults[i]
            XCTAssert(results[i] < keywords.count)
            let keywordDetected = keywords[Int(results[i])]
            XCTAssert(expectedKeyword == keywordDetected)
        }
    }
}
