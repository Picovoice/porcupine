//
// Copyright 2020-2021 Picovoice Inc.
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
  /**
   * Creates an instance of Porcupine.
   * @param {string} accessKey AccessKey obtained from Picovoice Console (https://console.picovoice.ai/).
   * @param {Array} keywords Absolute paths to keyword model files (`.ppn`).
   * @param {Array} sensitivities Sensitivity values for detecting keywords. 
   * Each value should be a number within [0, 1]. A higher sensitivity results in fewer misses at the cost of increasing the false alarm rate.
   * @param {string} manualModelPath Absolute path to the file containing model parameters (`.pv`).
   * @param {string} manualLibraryPath Absolute path to Porcupine's dynamic library (platform-dependent extension).
   */
  constructor(
    accessKey,
    keywords,
    sensitivities,
    manualModelPath,
    manualLibraryPath
  ) {
    if (
      accessKey === null ||
      accessKey === undefined ||
      accessKey.length === 0
    ) {
      throw new PvArgumentError(`No AccessKey provided to Porcupine`);
    }

    let modelPath = manualModelPath;
    if (modelPath === undefined || modelPath === null) {
      modelPath = path.resolve(__dirname, MODEL_PATH_DEFAULT);
    }

    let libraryPath = manualLibraryPath;
    if (libraryPath === undefined || modelPath === null) {
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

    const packed = pvPorcupine.init(
      accessKey,
      modelPath,
      keywordPaths.length,
      keywordPaths,
      sensitivities
    );
    const status = Number(packed % 10n);
    if (status !== PV_STATUS_T.SUCCESS) {
      pvStatusToException(status, "Porcupine failed to initialize");
    }
    this.handle = packed / 10n;

    this._frameLength = pvPorcupine.frame_length();
    this._sampleRate = pvPorcupine.sample_rate();
    this._version = pvPorcupine.version();
  }

  /**
   * @returns number of audio samples per frame (i.e. the length of the array provided to the process function)
   * @see {@link process}
   */
  get frameLength() {
    return this._frameLength;
  }

  /**
   * @returns the audio sampling rate accepted by Porcupine
   */
  get sampleRate() {
    return this._sampleRate;
  }

  /**
   * @returns the version of the Porcupine engine
   */
  get version() {
    return this._version;
  }

  /**
   * Process a frame of pcm audio.
   *
   * @param {Array} frame of mono, 16-bit, linear-encoded PCM audio.
   * The specific array length can be attained by calling `.frameLength`. 
   * The incoming audio needs to have a sample rate equal to `.sampleRate` and be 16-bit linearly-encoded. 
   * Porcupine operates on single-channel audio.
   * @returns {number} Index of observed keyword at the end of the current frame. 
   * Indexing is 0-based and matches the ordering of keyword models provided to the constructor. 
   * If no keyword is detected then it returns -1.
   */
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

  /**
   * Releases the resources acquired by Porcupine.
   *
   * Be sure to call this when finished with the instance
   * to reclaim the memory that was allocated by the C library.
   */
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
