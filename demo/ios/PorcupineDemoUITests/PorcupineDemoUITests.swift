import XCTest
import AVFoundation
import Porcupine

class PorcupineDemoUITests: XCTestCase {

    let app = XCUIApplication()

    override func setUp() {
        super.setUp()
        // In UI tests it is usually best to stop immediately when a failure occurs.
        continueAfterFailure = false
        app.launch()
    }

    override func tearDown() {
       // Put teardown code here. This method is called after the invocation of each test method in the class.
       super.tearDown()
   }

    func testInitSuccessWithSingleBuiltIn() throws {
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
    
    func testInitSuccessWithMultipleBuiltIns() throws {
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
