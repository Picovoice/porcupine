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
"use strict";

const ArrayType = require("ref-array-napi");
const ffi = require("ffi-napi");
const fs = require("fs");
const path = require("path");
const ref = require("ref-napi");

const PV_STATUS_T = require("./pv_status_t");
const {
  PvArgumentError,
  PvStateError,
  pvStatusToException,
} = require("./errors");
const { getSystemLibraryPath } = require("./platforms");
const {
  BUILTIN_KEYWORDS_ENUMS,
  BUILTIN_KEYWORDS_STRINGS,
  getBuiltinKeywordPath,
} = require("./builtin_keywords");

const MODEL_PATH_DEFAULT = "lib/common/porcupine_params.pv";

// ffi types
const int16Array = ArrayType(ref.types.int16);
const int32Ptr = ref.refType(ref.types.int32);
const floatArray = ArrayType(ref.types.float);
const stringArray = ArrayType(ref.types.CString);
const ppnObjPtr = ref.refType(ref.types.void);
const ppnObjPtrPtr = ref.refType(ppnObjPtr);

class Porcupine {
  constructor(keywords, sensitivities, manualModelPath, manualLibraryPath) {
    let modelPath = manualModelPath;
    if (modelPath === undefined) {
      modelPath = path.resolve(__dirname, MODEL_PATH_DEFAULT);
    }

    let libraryPath = manualLibraryPath;
    if (libraryPath === undefined) {
      libraryPath = getSystemLibraryPath();
    }

    if (keywords === null || keywords === undefined || keywords.length === 0) {
      throw new PvArgumentError(
        `keywordPaths are null/undefined/empty (${keywords})`
      );
    }

    if (
      sensitivities === null ||
      sensitivities === undefined ||
      sensitivities.length === 0
    ) {
      throw new PvArgumentError(
        `sensitivities are null/undefined/empty (${sensitivities})`
      );
    }

    for (let sensitivity of sensitivities) {
      if (sensitivity < 0 || sensitivity > 1 || isNaN(sensitivity)) {
        throw new RangeError(
          `Sensitivity value in 'sensitivities' not in range [0,1]: ${sensitivity}`
        );
      }
    }

    if (!Array.isArray(keywords)) {
      throw new PvArgumentError(`Keywords is not an array: ${keywords}`);
    }

    if (keywords.length !== sensitivities.length) {
      throw new PvArgumentError(
        `Number of keywords (${keywords.length}) does not match number of sensitivities (${sensitivities.length})`
      );
    }

    if (!fs.existsSync(libraryPath)) {
      throw new PvArgumentError(
        `File not found at 'libraryPath': ${libraryPath}`
      );
    }

    if (!fs.existsSync(modelPath)) {
      throw new PvArgumentError(`File not found at 'modelPath': ${modelPath}`);
    }

    let keywordPaths = [];
    for (let i = 0; i < keywords.length; i++) {
      let keyword = keywords[i];
      // builtin keywords are "enums" that have integer values
      if (Number.isInteger(keyword)) {
        if (BUILTIN_KEYWORDS_ENUMS.has(keyword)) {
          keywordPaths[i] = getBuiltinKeywordPath(keyword);
        } else {
          throw new PvArgumentError(
            `Builtin keyword argument ${keyword} does not match a builtin keyword: ${Array.from(
              BUILTIN_KEYWORDS_STRINGS
            )}`
          );
        }
      } else {
        if (!fs.existsSync(keyword)) {
          throw new PvArgumentError(`File not found in 'keywords': ${keyword}`);
        } else {
          keywordPaths[i] = keyword;
        }
      }
    }

    this.ppnPtrPtr = ref.alloc(ppnObjPtrPtr);
    this.keywordIndexPtr = ref.alloc(ref.types.int32);

    this.libPpn = ffi.Library(libraryPath, {
      pv_porcupine_delete: [ref.types.void, [ppnObjPtr]],
      pv_sample_rate: [ref.types.int32, []],
      pv_porcupine_frame_length: [ref.types.int32, []],
      pv_porcupine_init: [
        ref.types.int32,
        [
          ref.types.CString,
          ref.types.int32,
          stringArray,
          floatArray,
          ppnObjPtrPtr,
        ],
      ],
      pv_porcupine_version: [ref.types.CString, []],
      pv_porcupine_process: [
        ref.types.int32,
        [ppnObjPtr, int16Array, int32Ptr],
      ],
    });

    this.frameLength = this.libPpn.pv_porcupine_frame_length();
    this.sampleRate = this.libPpn.pv_sample_rate();
    this.version = this.libPpn.pv_porcupine_version();

    const initStatus = this.libPpn.pv_porcupine_init(
      modelPath,
      keywordPaths.length,
      keywordPaths,
      sensitivities,
      this.ppnPtrPtr
    );

    this.ppnPtr = this.ppnPtrPtr.deref();

    if (initStatus !== PV_STATUS_T.SUCCESS) {
      pvStatusToException(initStatus, "Porcupine failed to initialize");
    }
  }

  process(frame) {
    if (this.ppnPtr === null) {
      throw new PvStateError("Porcupine is not initialized");
    }

    if (frame === undefined || frame === null) {
      throw new PvArgumentError(
        `Frame array provided to process() is undefined or null`
      );
    } else if (frame.length !== this.frameLength) {
      throw new PvArgumentError(
        `Size of frame array provided to 'process' (${frame.length}) does not match the engine 'frameLength' (${this.frameLength})`
      );
    }

    // sample the first frame to check for non-integer values
    if (!Number.isInteger(frame[0])) {
      throw new PvArgumentError(
        `Non-integer frame values provided to process(): ${frame[0]}. Porcupine requires 16-bit integers`
      );
    }

    const frameBuffer = int16Array(this.frameLength);
    for (let i = 0; i < this.frameLength; i++) {
      frameBuffer[i] = frame[i];
    }

    const status = this.libPpn.pv_porcupine_process(
      this.ppnPtr,
      frameBuffer,
      this.keywordIndexPtr
    );

    if (status !== PV_STATUS_T.SUCCESS) {
      pvStatusToException(status, "Porcupine failed to process the frame");
    }

    return this.keywordIndexPtr.deref();
  }

  release() {
    if (this.ppnPtr !== null) {
      this.libPpn.pv_porcupine_delete(this.ppnPtr);
      this.ppnPtr = null;
      this.ppnPtrPtr = null;
    } else {
      console.warn("Porcupine is not initialized; nothing to destroy");
    }
  }
}

module.exports = Porcupine;
