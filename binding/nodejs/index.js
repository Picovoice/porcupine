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

const fs = require("fs");
const path = require("path");

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

const pvPorcupine = require(getSystemLibraryPath());

const MODEL_PATH_DEFAULT = "lib/common/porcupine_params.pv";

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


    const packed = pvPorcupine.init(modelPath, keywordPaths.length, keywordPaths, sensitivities);
    const status = Number(packed % 10n);
    if (status !== PV_STATUS_T.SUCCESS) {
      pvStatusToException(status, "Porcupine failed to initialize");
    }
    this.handle = packed / 10n;


    this.frameLength = pvPorcupine.frame_length();
    this.sampleRate = pvPorcupine.sample_rate();
    this.version = pvPorcupine.version();
  }

  process(frame) {
    if (this.handle === 0) {
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

    const frameBuffer = new Int16Array(frame);

    const packed = pvPorcupine.process(this.handle, frameBuffer);
    const status = packed % 10;
    if (status !== PV_STATUS_T.SUCCESS) {
      pvStatusToException(status, "Porcupine failed to process the frame");
    }
    const keywordIndex = packed / 10;

    return keywordIndex;
  }

  release() {
    if (this.handle > 0) {
      pvPorcupine.delete(this.handle);
      this.handle = 0;
    } else {
      console.warn("Porcupine is not initialized; nothing to destroy");
    }
  }
}

module.exports = Porcupine;
