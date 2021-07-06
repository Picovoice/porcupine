import XCTest
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

    func testPorcupineInit() throws {
        let p:Porcupine = try Porcupine.init(keyword: Porcupine.BuiltInKeyword.porcupine)
        let samps = [Int16](repeating: 0, count: Int(Porcupine.frameLength))
        XCTAssert(try p.process(pcm:samps) == -1)
        p.delete()
    }
}
