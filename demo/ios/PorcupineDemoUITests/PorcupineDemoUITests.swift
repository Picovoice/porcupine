import AVFoundation
import XCTest

import Porcupine

class PorcupineDemoUITests: XCTestCase {

    override func setUp() {
        super.setUp()
        continueAfterFailure = false
    }

    override func tearDown() {
       super.tearDown()
   }

    func testInitSuccessWithSingleBuiltIn() throws {
        let p = try Porcupine.init(keyword: Porcupine.BuiltInKeyword.porcupine)
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
        
        let p = try Porcupine.init(keywords: keywords)
        p.delete()
    }
    
    func testInitSuccessWithCustomArguments() throws {
        let bundle = Bundle(for: type(of: self))
        let keywordPath = bundle.path(forResource: "hey barista_ios", ofType: "ppn")!
        let modelPath = bundle.path(forResource: "porcupine_params", ofType: "pv")
        
        let p = try Porcupine.init(keywordPath: keywordPath, modelPath: modelPath)
        p.delete()
    }
    
    func testInitSuccessWithMultipleCustomKeywordsAndSensitivities() throws {
        let bundle = Bundle(for: type(of: self))
        let keywordPath = bundle.path(forResource: "hey barista_ios", ofType: "ppn")!
        let keywordPath2 = bundle.path(forResource: "pico clock_ios", ofType: "ppn")!
        
        let p = try Porcupine.init(keywordPaths: [keywordPath, keywordPath2], sensitivities: [0.35, 0.6])
        p.delete()
    }
    
    func testInitFailWithMismatchedLanguage() throws {
        let bundle = Bundle(for: type(of: self))
        let keywordPath = bundle.path(forResource: "ananas_ios", ofType: "ppn")!
        let modelPath = bundle.path(forResource: "porcupine_params", ofType: "pv")!
        
        var didFail = false
        do {
            _ = try Porcupine.init(keywordPath: keywordPath, modelPath: modelPath)
        } catch {
            didFail = true
        }
        
        XCTAssert(didFail)
    }
    
    func testInitFailWithInvalidModelPath() throws {
        
        var didFail = false
        do {
            _ = try Porcupine.init(keyword: Porcupine.BuiltInKeyword.porcupine, modelPath: "bad_path/bad_path.pv")
        } catch {
            didFail = true
        }
        
        XCTAssert(didFail)
    }
    
    func testInitFailWithInvalidSensitivity() throws {
        
        var didFail = false
        do {
            _ = try Porcupine.init(keyword: Porcupine.BuiltInKeyword.porcupine, sensitivity: 10)
        } catch {
            didFail = true
        }
        
        XCTAssert(didFail)
    }
    
    func testInitFailWithMismatchedSensitivityCount() throws {
        
        let keywords = [Porcupine.BuiltInKeyword.porcupine, Porcupine.BuiltInKeyword.grasshopper]
        let sensitivities:[Float32] = [0.3, 0.8, 0.4]
        var didFail = false
        do {
            _ = try Porcupine.init(keywords: keywords, sensitivities: sensitivities)
        } catch {
            didFail = true
        }
        
        XCTAssert(didFail)
    }
    
    func testInitFailWithNoKeywords() throws {
        
        let keywords:[Porcupine.BuiltInKeyword] = []
        var didFail = false
        do {
            _ = try Porcupine.init(keywords: keywords)
        } catch {
            didFail = true
        }
        
        XCTAssert(didFail)
    }
    
    func testInitFailWithInvalidKeywordPath() throws {
        
        var didFail = false
        do {
            _ = try Porcupine.init(keywordPath: "bad_path/bad_path.ppn")
        } catch {
            didFail = true
        }
        
        XCTAssert(didFail)
    }
    
    func testInitFailWithWrongKeywordPlatform() throws {
        let bundle = Bundle(for: type(of: self))
        let keywordPath = bundle.path(forResource: "alexa_linux", ofType: "ppn")!
        
        var didFail = false
        do {
            _ = try Porcupine.init(keywordPath: keywordPath)
        } catch {
            didFail = true
        }
        
        XCTAssert(didFail)
    }
    
    func testInitSuccessES() throws {
        let bundle = Bundle(for: type(of: self))
        let keywordPath = bundle.path(forResource: "emparedado_ios", ofType: "ppn")!
        let modelPath = bundle.path(forResource: "porcupine_params_es", ofType: "pv")!
        
        let p = try Porcupine.init(keywordPath: keywordPath, modelPath: modelPath)
        p.delete()
    }
    
    func testInitSuccessFR() throws {
        let bundle = Bundle(for: type(of: self))
        let keywordPath = bundle.path(forResource: "framboise_ios", ofType: "ppn")!
        let modelPath = bundle.path(forResource: "porcupine_params_fr", ofType: "pv")!
        
        let p = try Porcupine.init(keywordPath: keywordPath, modelPath: modelPath)
        p.delete()
    }
    
    func testInitSuccessDE() throws {
        let bundle = Bundle(for: type(of: self))
        let keywordPath = bundle.path(forResource: "ananas_ios", ofType: "ppn")!
        let modelPath = bundle.path(forResource: "porcupine_params_de", ofType: "pv")!
        
        let p = try Porcupine.init(keywordPath: keywordPath, modelPath: modelPath)
        p.delete()
    }
    
    func testProcSuccessWithSingleBuiltIn() throws {
        let p:Porcupine = try Porcupine.init(keyword: Porcupine.BuiltInKeyword.porcupine)
        
        let bundle = Bundle(for: type(of: self))
        let fileURL:URL = bundle.url(forResource: "porcupine", withExtension: "wav")!
    
        let data = try Data(contentsOf: fileURL)
        let frameLengthBytes = Int(Porcupine.frameLength) * 2
        var pcmBuffer = Array<Int16>(repeating: 0, count: Int(Porcupine.frameLength))
        var results:[Int32] = []
        var index = 44
        while(index + frameLengthBytes < data.count) {
            _ = pcmBuffer.withUnsafeMutableBytes { data.copyBytes(to: $0, from: index..<(index + frameLengthBytes)) }
            let keywordIndex:Int32 = try p.process(pcm:pcmBuffer)
            if(keywordIndex == 0){
                results.append(keywordIndex)
            }
            
            index += frameLengthBytes
        }
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
        
        let p:Porcupine = try Porcupine.init(keywords: keywords)
        
        let bundle = Bundle(for: type(of: self))
        let fileURL:URL = bundle.url(forResource: "multiple_keywords", withExtension: "wav")!
    
        let data = try Data(contentsOf: fileURL)
        let frameLengthBytes = Int(Porcupine.frameLength) * 2
        var pcmBuffer = Array<Int16>(repeating: 0, count: Int(Porcupine.frameLength))
        var results:[Int32] = []
        var index = 44
        while(index + frameLengthBytes < data.count) {
            _ = pcmBuffer.withUnsafeMutableBytes { data.copyBytes(to: $0, from: index..<(index + frameLengthBytes)) }
            let keywordIndex:Int32 = try p.process(pcm:pcmBuffer)
            if(keywordIndex >= 0){
                results.append(keywordIndex)
            }
            
            index += frameLengthBytes
        }
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
