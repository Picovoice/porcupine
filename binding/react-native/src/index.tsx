//
// Copyright 2020 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
// file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//

import { NativeModules } from 'react-native';
const RCTPorcupine = NativeModules.Porcupine;

class Porcupine{

  private _handle: string | undefined;
  private _frameLength: number | undefined;
  private _sampleRate: number | undefined;
  private _version: string | undefined;
   
  static async fromKeywords(keywords:string[], modelPath?:string, sensitivities?:number[]){        
    
    try{
    var { 
        handle, 
        frameLength, 
        sampleRate, 
        version
      } = await Porcupine.create(keywords, undefined, modelPath, sensitivities);
      return new Porcupine(handle, frameLength, sampleRate, version);                    
    }
    catch(e){
      throw e;
    }      
  }

  static async fromKeywordPaths(keywordsPaths: string[], modelPath?:string, sensitivities?:number[]){
    
    var { 
      handle, 
      frameLength, 
      sampleRate, 
      version
    } = await Porcupine.create(undefined, keywordsPaths, modelPath, sensitivities);  
    return new Porcupine(handle, frameLength, sampleRate, version);          
  }

  constructor(handle: string, frameLength: number, sampleRate: number, version: string) {       
    this._handle = handle;      
    this._frameLength = frameLength;
    this._sampleRate = sampleRate;
    this._version = version;            
  }

  private static async create(keywords?:string[], keywordPaths?:string[], modelPath?:string, sensitivities?:number[]){
    if (modelPath === undefined) {
      modelPath = RCTPorcupine.DEFAULT_MODEL_PATH;          
    }    
  
    if (Array.isArray(keywords) && Array.isArray(keywordPaths)) {            
      return Promise.reject(`Both 'keywords' and 'keywordPaths' were set. Only one of the two arguments may be set for intializtion.`);      
    }

    if(keywordPaths === undefined || keywordPaths.length === 0 || !Array.isArray(keywordPaths)){
        if(keywords === undefined || keywords.length === 0 || !Array.isArray(keywords)){          
          return Promise.reject(`Either 'keywords' or 'keywordPaths' must be set.`);          
        }

        keywordPaths = []
        for (let i = 0; i < keywords.length; i++) {
          let keywordPath = RCTPorcupine.KEYWORD_PATHS[keywords[i]]; 
          if(keywordPath !== undefined) {
            keywordPaths[i] = keywordPath;
          }
          else{            
            return Promise.reject(`One or more keywords are not available by default. Available default keywords are: ${Object.keys(RCTPorcupine.KEYWORD_PATHS)}`);            
          }
        }
    }

    if (sensitivities === undefined || sensitivities.length === 0) {
      sensitivities = []
      for (let i = 0; i < keywordPaths.length; i++) {
        sensitivities[i] = 0.5;
      }
    }

    for (let sensitivity of sensitivities) {
      if (sensitivity < 0 || sensitivity > 1 || isNaN(sensitivity)) {                  
        return Promise.reject(`Sensitivity value in 'sensitivities' not in range [0,1]: ${sensitivity}`);        
      }
    }  

    if (keywordPaths.length !== sensitivities.length) {      
      return Promise.reject(`Number of keywords (${keywordPaths.length}) does not match number of sensitivities (${sensitivities.length})`);      
    }    
             
    return RCTPorcupine.create(modelPath, keywordPaths, sensitivities);          
  }

  async process(frame:number[]) {
      
      if (frame === undefined) {        
        return Promise.reject(`Frame array provided to process() is undefined or null`);        
      } else if (frame.length !== this._frameLength) {                    
        return Promise.reject(`Size of frame array provided to 'process' (${frame.length}) does not match the engine 'frameLength' (${this._frameLength})`);          
      }

      // sample the first frame to check for non-integer values
      if (!Number.isInteger(frame[0])) {          
        return Promise.reject(`Non-integer frame values provided to process(): ${frame[0]}. Porcupine requires 16-bit integers`);          
      }

      return RCTPorcupine.process(this._handle, frame);
  }

  async release() {
    console.log("destroying" + this._handle)
    return RCTPorcupine.delete(this._handle);
  }

  get frameLength(){
    return this._frameLength;
  }

  get sampleRate(){
    return this._sampleRate;
  }

  get version(){
    return this._version;
  }
}

export default Porcupine;