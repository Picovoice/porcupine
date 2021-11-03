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

import { NativeModules } from 'react-native';

import BuiltInKeywords  from './builtin_keywords';
import * as PorcupineExceptions from './porcupine_exceptions';

const RCTPorcupine = NativeModules.PvPorcupine;

type NativeError = {
  code: string;
  message: string;
}

class Porcupine {
  private _handle: string;
  private _frameLength: number;
  private _sampleRate: number;
  private _version: string;

  private static readonly DEFAULT_MODEL_PATH: string = RCTPorcupine.DEFAULT_MODEL_PATH;
  private static readonly KEYWORD_PATHS = RCTPorcupine.KEYWORD_PATHS;

  /**
   * Static creator for initializing Porcupine from one of the built-in keywords
   * @param accessKey AccessKey obtained from Picovoice Console (https://console.picovoice.ai/.
   * @param keywords List of keywords (phrases) for detection. The list of available (default) keywords can be retrieved
   * using `Porcupine.KEYWORDS`. If `keyword_paths` is set then this argument will be ignored.
   * @param modelPath Path to the file containing model parameters. If not set it will be set to the default location.
   * @param sensitivities sensitivities for each keywords model. A higher sensitivity reduces miss rate
   * at the cost of potentially higher false alarm rate. Sensitivity should be a floating-point number within
   * [0, 1].
   * @returns An instance of the engine.
   */
  public static async fromKeywords(
    accessKey: string,
    keywords: BuiltInKeywords[],
    modelPath?: string,
    sensitivities?: number[]
  ) {
    const keywordPaths = keywords.map((keyword) => {
      if (!Object.values(BuiltInKeywords).includes(keyword)) {
        throw new PorcupineExceptions.PorcupineInvalidArgumentException(
          `keyword '${keyword}' is not a build-in keyword.`);
      }
      if (this.KEYWORD_PATHS[keyword] === undefined || this.KEYWORD_PATHS[keyword] === null) {
        throw new PorcupineExceptions.PorcupineInvalidArgumentException(
          `keyword '${keyword}' is not a supported keyword.`);
      }

      return this.KEYWORD_PATHS[keyword];
    });

    return this.fromKeywordPaths(accessKey, keywordPaths, modelPath, sensitivities);
  }

  /**
   * Static creator for initializing Porcupine from a list of paths to custom keyword files
   * @param accessKey AccessKey obtained from Picovoice Console (https://console.picovoice.ai/).
   * @param keywordPaths Absolute paths to keyword model files.
   * @param modelPath Path to the file containing model parameters. If not set it will be set to the default location.
   * @param sensitivities sensitivities for each keywords model. A higher sensitivity reduces miss rate
   * at the cost of potentially higher false alarm rate. Sensitivity should be a floating-point number within
   * [0, 1].
   * @returns An instance of the engine.
   */
  public static async fromKeywordPaths(
    accessKey: string,
    keywordsPaths: string[],
    modelPath?: string,
    sensitivities?: number[]
  ) {
    try {
      let { handle, frameLength, sampleRate, version } = await Porcupine.fromKeywordPathsNative(
        accessKey,
        keywordsPaths,
        modelPath,
        sensitivities
      );
      return new Porcupine(handle, frameLength, sampleRate, version);
    } catch (err) {
      if (err instanceof PorcupineExceptions.PorcupineException) {
        throw err;
      } else {
        const nativeError = err as NativeError;
        throw this.codeToException(nativeError.code, nativeError.message);
      }
    }
  }

  /**
   * Creates an instance of wake word engine (Porcupine).
   * @param accessKey AccessKey obtained from Picovoice Console (https://console.picovoice.ai/).
   * @param keywordPaths Absolute paths to keyword model files. If not set it will be populated from `keywords` argument.
   * @param modelPath Path to the file containing model parameters. If not set it will be set to the default location.
   * @param sensitivities sensitivities for each keywords model. A higher sensitivity reduces miss rate
   * at the cost of potentially higher false alarm rate. Sensitivity should be a floating-point number within
   * [0, 1].
   * @returns An instance of the engine.
   */
  private static async fromKeywordPathsNative(
    accessKey: string,
    keywordPaths?: string[],
    modelPath?: string,
    sensitivities?: number[]
  ) {
    if (accessKey === undefined || accessKey === null) {
      throw new PorcupineExceptions.PorcupineInvalidArgumentException(
        "'accessKey' must be set.");
    }

    if (modelPath === undefined) {
      modelPath = this.DEFAULT_MODEL_PATH;
    }

    if (
      keywordPaths === undefined ||
      keywordPaths.length === 0 ||
      !Array.isArray(keywordPaths)
    ) {
      throw new PorcupineExceptions.PorcupineInvalidArgumentException(
          "keywordPaths must be set, not empty and an array.");
    }

    for (let i = 0; i < keywordPaths.length; i++) {
      if (keywordPaths[i] == null || keywordPaths[i] == "") {
        throw new PorcupineExceptions.PorcupineInvalidArgumentException(
            "One of the provided keyword paths was empty.");
      }
    }

    if (sensitivities === undefined || sensitivities.length === 0) {
      sensitivities = [];
      for (let i = 0; i < keywordPaths.length; i++) {
        sensitivities[i] = 0.5;
      }
    }

    for (let sensitivity of sensitivities) {
      if (sensitivity < 0 || sensitivity > 1 || isNaN(sensitivity)) {
        throw new PorcupineExceptions.PorcupineInvalidArgumentException(
          `Sensitivity value in 'sensitivities' not in range [0,1]: ${sensitivity}`
        );
      }
    }

    if (keywordPaths.length !== sensitivities.length) {
      throw new PorcupineExceptions.PorcupineInvalidArgumentException(
        `Number of keywords (${keywordPaths.length}) does not match number of sensitivities (${sensitivities.length})`
      );
    }

    return RCTPorcupine.fromKeywordPaths(accessKey, modelPath, keywordPaths, sensitivities);
  }

  private constructor(
    handle: string,
    frameLength: number,
    sampleRate: number,
    version: string
  ) {
    this._handle = handle;
    this._frameLength = frameLength;
    this._sampleRate = sampleRate;
    this._version = version;
  }

  /**
   * Process a frame of audio with the wake word engine.
   * @param frame A frame of audio samples to be assessed by Porcupine. The required audio format is found by
   * calling `.sampleRate` to get the required sample rate and `.frameLength` to get the required frame size.
   * Audio must be single-channel and 16-bit linearly-encoded.
   * @returns {Promise} Index of the detected keyword, or -1 if no detection occurred
   */
  async process(frame: number[]) {
    if (frame === undefined) {
      throw new PorcupineExceptions.PorcupineInvalidStateException(
        "Frame array provided to process() is undefined or null");
    } else if (frame.length !== this._frameLength) {
      throw new PorcupineExceptions.PorcupineInvalidStateException(
        `Size of frame array provided to 'process' (${frame.length}) does not match the engine 'frameLength' (${this._frameLength})`);
    }

    // sample the first frame to check for non-integer values
    if (!Number.isInteger(frame[0])) {
      throw new PorcupineExceptions.PorcupineInvalidStateException(
        `Non-integer frame values provided to process(): ${frame[0]}. Porcupine requires 16-bit integers`);
    }

    try {
      return await RCTPorcupine.process(this._handle, frame);
    } catch(err) {
      const nativeError = err as NativeError;
      throw Porcupine.codeToException(nativeError.code, nativeError.message);
    }
  }

  /**
   * Frees memory that was allocated for Porcupine
   */
  async delete() {
    return RCTPorcupine.delete(this._handle);
  }

  /**
   * Gets the required number of audio samples per frame.
   * @returns Required frame length.
   */
  get frameLength() {
    return this._frameLength;
  }

  /**
   * Get the audio sample rate required by Porcupine.
   * @returns Required sample rate.
   */
  get sampleRate() {
    return this._sampleRate;
  }

  /**
   * Gets the version number of the Porcupine library.
   * @returns Version of Pocupine
   */
  get version() {
    return this._version;
  }

  /**
   * Gets the exception type given a code.
   * @param code Code name of nativee exception.
   */
  private static codeToException(code: string, message: string) {
    switch(code) {
      case 'PorcupineException':
        return new PorcupineExceptions.PorcupineException(message);
      case 'PorcupineMemoryException':
        return new PorcupineExceptions.PorcupineMemoryException(message);
      case 'PorcupineIOException':
        return new PorcupineExceptions.PorcupineIOException(message);
      case 'PorcupineInvalidArgumentException':
        return new PorcupineExceptions.PorcupineInvalidArgumentException(message);
      case 'PorcupineStopIterationException':
        return new PorcupineExceptions.PorcupineStopIterationException(message);
      case 'PorcupineKeyException':
        return new PorcupineExceptions.PorcupineKeyException(message);
      case 'PorcupineInvalidStateException':
        return new PorcupineExceptions.PorcupineInvalidStateException(message);
      case 'PorcupineRuntimeException':
        return new PorcupineExceptions.PorcupineRuntimeException(message);
      case 'PorcupineActivationException':
        return new PorcupineExceptions.PorcupineActivationException(message);
      case 'PorcupineActivationLimitException':
        return new PorcupineExceptions.PorcupineActivationLimitException(message);
      case 'PorcupineActivationThrottledException':
        return new PorcupineExceptions.PorcupineActivationThrottledException(message);
      case 'PorcupineActivationRefusedException':
        return new PorcupineExceptions.PorcupineActivationRefusedException(message);
      default:
        throw new Error(`unexpected code: ${code}, message: ${message}`);
    }
  }
}

export default Porcupine;
