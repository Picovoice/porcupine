import pv_porcupine

@objc(Porcupine)
class Porcupine: NSObject {

    private var porcupine: OpaquePointer?
    
    @objc func constantsToExport() -> Dictionary<String, Any> {        
        
        let modelPath : String = Bundle.main.path(forResource: "porcupine_params", ofType: "pv") ?? "unknown"
        NSLog(modelPath)

        let keywordPath: String = Bundle.main.path(forResource: "porcupine_ios", ofType: "ppn") ?? "unknown"        
        NSLog(keywordPath)

        return [
            "DEFAULT_MODEL_PATH": modelPath,
            "KEYWORD_PATHS": [
                "porcupine": keywordPath
            ]        
        ]
    }

    @objc(create:keywordPath:sensitivity:)
    func create(modelPath: String, keywordPath: String, sensitivity: Float32) -> Void {        
        let keywordPaths = [keywordPath]
        let status = pv_porcupine_init(
            modelPath,
            Int32(1),
            keywordPaths.map { UnsafePointer(strdup($0)) },
            [sensitivity],
            &porcupine);
        
        let statusStr: String = String(cString: pv_status_to_string(status));
        NSLog(statusStr);
        
    }
    
    @objc(delete)
    func delete() -> Void {
        pv_porcupine_delete(porcupine)
        porcupine = nil
    }
    
    @objc(process:callback:)
    func process(pcm:[Int16], callback: RCTResponseSenderBlock) -> Void {
                
        var keywordIndex: Int32 = -1
        pv_porcupine_process(porcupine, pcm, &keywordIndex)        
        callback([keywordIndex])
    }
    
    @objc(getFrameLength:)
    func getFrameLength(callback: RCTResponseSenderBlock) -> Void {
        let frameLength = UInt32(pv_porcupine_frame_length())        
        callback([frameLength])
    }
    
    @objc(getSampleRate:)
    func getSampleRate(callback: RCTResponseSenderBlock) -> Void {
        let sampleRate = Float64(pv_sample_rate())        
        callback([sampleRate])
    }
    
    @objc(getVersion:)
    func getVersion(callback: RCTResponseSenderBlock) -> Void {
        let version: String = String(cString: pv_porcupine_version())        
        callback([version])
    }
}
