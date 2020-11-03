// 'use strict';

// import PorcupineClass from './porcupine';
// export default PorcupineClass;

import { NativeModules } from 'react-native';

const RCTPorcupine = NativeModules.Porcupine;

class PorcupineClass{
  constructor(keyword :string, sensitivity:number, manualModelPath?:string) {
      
      let modelPath = manualModelPath;        
      if (modelPath === undefined) {
        modelPath = RCTPorcupine.DEFAULT_MODEL_PATH;          
      }    
      
      let keywordPath = RCTPorcupine.KEYWORD_PATHS[keyword];        
      RCTPorcupine.create(modelPath, keywordPath, sensitivity);             

  }

  process(frame:Array<number>, callback:any) {

      // const frameBuffer = new Int16Array(frame);
      RCTPorcupine.process(frame, callback);                

      // return keywordIndex;
  }

  release() {
      RCTPorcupine.delete();
  }
}

export default PorcupineClass;