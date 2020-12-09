import Flutter
import UIKit
import pv_porcupine

public class SwiftPorcupinePlugin: NSObject, FlutterPlugin {
  public static func register(with registrar: FlutterPluginRegistrar) {
    
  }

  // this function call is required to prevent the linker from 
  // stripping the porcupine static library
  public func bindPorcupine(){
    pv_porcupine_version();
  }
}
