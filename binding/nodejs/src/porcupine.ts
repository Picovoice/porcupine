//
// Copyright 2020-2022 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
// file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//
"use strict";

import * as fs from "fs";
import * as path from "path";

import PvStatus from "./pv_status_t";
import {
  PorcupineInvalidArgumentError,
  PorcupineInvalidStateError,
  pvStatusToException,
} from "./errors";
import { getSystemLibraryPath } from "./platforms";
import {
  BuiltinKeyword,
  getBuiltinKeywordPath,
} from "./builtin_keywords";

const MODEL_PATH_DEFAULT = "../lib/common/porcupine_params.pv";

type PorcupineHandleAndStatus = { handle: any, status: PvStatus };
type KeywordAndStatus = { keyword_index: number, status: PvStatus }

export default class Porcupine {
  private _pvPorcupine: any;

  private _handle: any;

  private readonly _version: string;
  private readonly _sampleRate: number;
  private readonly _frameLength: number;

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
    accessKey: string,
    keywords: string[],
    sensitivities: number[],
    manualModelPath?: string,
    manualLibraryPath?: string
  ) {
    if (
      accessKey === null ||
      accessKey === undefined ||
      accessKey.length === 0
    ) {
      throw new PorcupineInvalidArgumentError(`No AccessKey provided to Porcupine`);
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
      throw new PorcupineInvalidArgumentError(
        `keywordPaths are null/undefined/empty (${keywords})`
      );
    }

    if (
      sensitivities === null ||
      sensitivities === undefined ||
      sensitivities.length === 0
    ) {
      throw new PorcupineInvalidArgumentError(
        `sensitivities are null/undefined/empty (${sensitivities})`
      );
    }

    for (const sensitivity of sensitivities) {
      if (sensitivity < 0 || sensitivity > 1 || isNaN(sensitivity)) {
        throw new RangeError(
          `Sensitivity value in 'sensitivities' not in range [0,1]: ${sensitivity}`
        );
      }
    }

    if (!Array.isArray(keywords)) {
      throw new PorcupineInvalidArgumentError(`Keywords is not an array: ${keywords}`);
    }

    if (keywords.length !== sensitivities.length) {
      throw new PorcupineInvalidArgumentError(
        `Number of keywords (${keywords.length}) does not match number of sensitivities (${sensitivities.length})`
      );
    }

    if (!fs.existsSync(libraryPath)) {
      throw new PorcupineInvalidArgumentError(
        `File not found at 'libraryPath': ${libraryPath}`
      );
    }

    if (!fs.existsSync(modelPath)) {
      throw new PorcupineInvalidArgumentError(`File not found at 'modelPath': ${modelPath}`);
    }

    const keywordPaths: string[] = [];
    for (let i = 0; i < keywords.length; i++) {
      const keyword = keywords[i];
      if ((<any>Object).values(BuiltinKeyword).includes(keyword)) {
        keywordPaths[i] = getBuiltinKeywordPath(<BuiltinKeyword>keyword);
      } else {
        if (!fs.existsSync(keyword)) {
          throw new PorcupineInvalidArgumentError(`File not found in 'keywords': ${keyword}`);
        } else {
          keywordPaths[i] = keyword;
        }
      }
    }

    const pvPorcupine = require(libraryPath); // eslint-disable-line

    let porcupineHandleAndStatus: PorcupineHandleAndStatus | null = null;
    try {
      porcupineHandleAndStatus = pvPorcupine.init(
        accessKey,
        modelPath,
        keywordPaths.length,
        keywordPaths,
        sensitivities
      );
    } catch (err: any) {
      pvStatusToException(<PvStatus>err.code, err);
    }

    const status = porcupineHandleAndStatus!.status;
    if (status !== PvStatus.SUCCESS) {
      pvStatusToException(status, "Porcupine failed to initialize");
    }

    this._handle = porcupineHandleAndStatus!.handle;
    this._pvPorcupine = pvPorcupine;
    this._frameLength = pvPorcupine.frame_length();
    this._sampleRate = pvPorcupine.sample_rate();
    this._version = pvPorcupine.version();
  }

  /**
   * @returns number of audio samples per frame (i.e. the length of the array provided to the process function)
   * @see {@link process}
   */
  get frameLength(): number {
    return this._frameLength;
  }

  /**
   * @returns the audio sampling rate accepted by Porcupine
   */
  get sampleRate(): number {
    return this._sampleRate;
  }

  /**
   * @returns the version of the Porcupine engine
   */
  get version(): string {
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
  process(frame: Int16Array): number {
    if (
      this._handle === 0 ||
      this._handle === null ||
      this._handle === undefined
    ) {
      throw new PorcupineInvalidStateError("Porcupine is not initialized");
    }

    if (frame === undefined || frame === null) {
      throw new PorcupineInvalidArgumentError(
        `Frame array provided to process() is undefined or null`
      );
    } else if (frame.length !== this.frameLength) {
      throw new PorcupineInvalidArgumentError(
        `Size of frame array provided to 'process' (${frame.length}) does not match the engine 'frameLength' (${this.frameLength})`
      );
    }

    // sample the first frame to check for non-integer values
    if (!Number.isInteger(frame[0])) {
      throw new PorcupineInvalidArgumentError(
        `Non-integer frame values provided to process(): ${frame[0]}. Porcupine requires 16-bit integers`
      );
    }

    const frameBuffer = new Int16Array(frame);

    let keywordAndStatus: KeywordAndStatus | null = null;
    try {
      keywordAndStatus = this._pvPorcupine.process(this._handle, frameBuffer);
    } catch (err: any) {
      pvStatusToException(<PvStatus>err.code, err);
    }

    const status = keywordAndStatus!.status;
    if (status !== PvStatus.SUCCESS) {
      pvStatusToException(status, "Porcupine failed to process the frame");
    }
    const keywordIndex = keywordAndStatus!.keyword_index;

    return keywordIndex;
  }

  /**
   * Releases the resources acquired by Porcupine.
   *
   * Be sure to call this when finished with the instance
   * to reclaim the memory that was allocated by the C library.
   */
  release(): void {
    if (this._handle > 0) {
      this._pvPorcupine.delete(this._handle);
      this._handle = 0;
    } else {
      // eslint-disable-next-line no-console
      console.warn("Porcupine is not initialized; nothing to destroy");
    }
  }
}
