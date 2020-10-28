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

import { LogBox, NativeModules } from 'react-native';
const RCTPorcupine = NativeModules.Porcupine;
import PV_STATUS_T from './pv_status_t';

const {
    PvArgumentError,
    PvStateError,
    pvStatusToException,
} = require("./errors");
const {
    BUILTIN_KEYWORDS_ENUMS,
    BUILTIN_KEYWORDS_STRINGS,
    getBuiltinKeywordPath,
} = require("./builtin_keywords");


// const MODEL_PATH_DEFAULT = require('./assets/lib/common/porcupine_params.pv');
// const KEYWORDS = require('./assets/resources/android/porcupine_android.ppn');

class PorcupineClass{
    constructor(keywords, sensitivities, manualModelPath) {
        
        // let modelPath = manualModelPath;
        // if (modelPath === undefined) {
        //   modelPath = MODEL_PATH_DEFAULT;
        // }    
        console.log(MODEL_PATH_DEFAULT);
        console.log(MODEL_PATH_DEFAULT);
        
        RCTPorcupine.init(MODEL_PATH_DEFAULT, KEYWORDS, 0.5);        

        // if (keywords === null || keywords === undefined || keywords.length === 0) {
        //   throw new PvArgumentError(
        //     `keywordPaths are null/undefined/empty (${keywords})`
        //   );
        // }
    
        // if (
        //   sensitivities === null ||
        //   sensitivities === undefined ||
        //   sensitivities.length === 0
        // ) {
        //   throw new PvArgumentError(
        //     `sensitivities are null/undefined/empty (${sensitivities})`
        //   );
        // }
    
        // for (let sensitivity of sensitivities) {
        //   if (sensitivity < 0 || sensitivity > 1 || isNaN(sensitivity)) {
        //     throw new RangeError(
        //       `Sensitivity value in 'sensitivities' not in range [0,1]: ${sensitivity}`
        //     );
        //   }
        // }
    
        // if (!Array.isArray(keywords)) {
        //   throw new PvArgumentError(`Keywords is not an array: ${keywords}`);
        // }
    
        // if (keywords.length !== sensitivities.length) {
        //   throw new PvArgumentError(
        //     `Number of keywords (${keywords.length}) does not match number of sensitivities (${sensitivities.length})`
        //   );
        // }
    
        // if (!fs.existsSync(modelPath)) {
        //   throw new PvArgumentError(`File not found at 'modelPath': ${modelPath}`);
        // }
    
        // let keywordPaths = [];
        // for (let i = 0; i < keywords.length; i++) {
        //   let keyword = keywords[i];
        //   // builtin keywords are "enums" that have integer values
        //   if (Number.isInteger(keyword)) {
        //     if (BUILTIN_KEYWORDS_ENUMS.has(keyword)) {
        //       keywordPaths[i] = getBuiltinKeywordPath(keyword);
        //     } else {
        //       throw new PvArgumentError(
        //         `Builtin keyword argument ${keyword} does not match a builtin keyword: ${Array.from(
        //           BUILTIN_KEYWORDS_STRINGS
        //         )}`
        //       );
        //     }
        //   } else {
        //     if (!fs.existsSync(keyword)) {
        //       throw new PvArgumentError(`File not found in 'keywords': ${keyword}`);
        //     } else {
        //       keywordPaths[i] = keyword;
        //     }
        //   }        
        // }

 
        // this.frameLength = 4;//RCTPorcupine.getFrameLength();
        // this.sampleRate = 4;//RCTPorcupine.getSampleRate();
        // this.version = "2.3";//RCTPorcupine.getVersion();
    }

    // process() {
    //     if (this.handle === 0) {
    //         throw new PvStateError("Porcupine is not initialized");
    //     }

    //     if (frame === undefined || frame === null) {
    //         throw new PvArgumentError(
    //         `Frame array provided to process() is undefined or null`
    //         );
    //     } else if (frame.length !== this.frameLength) {
    //         throw new PvArgumentError(
    //         `Size of frame array provided to 'process' (${frame.length}) does not match the engine 'frameLength' (${this.frameLength})`
    //         );
    //     }

    //     // sample the first frame to check for non-integer values
    //     if (!Number.isInteger(frame[0])) {
    //         throw new PvArgumentError(
    //         `Non-integer frame values provided to process(): ${frame[0]}. Porcupine requires 16-bit integers`
    //         );
    //     }

    //     const frameBuffer = new Int16Array(frame);
    //     const keywordIndex = RCTPorcupine.process(frameBuffer);                

    //     return keywordIndex;
    // }

    release() {
        RCTPorcupine.delete();
    }
}


export default PorcupineClass;