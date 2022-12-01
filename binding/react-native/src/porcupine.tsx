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
import * as PorcupineErrors from './porcupine_errors';
import type BuiltInKeywords from './builtin_keywords';

const RCTPorcupine = NativeModules.PvPorcupine;

type NativeError = {
  code: string;
  message: string;
};

class Porcupine {
  private _handle: string;
  private _frameLength: number;
  private _sampleRate: number;
  private _version: string;

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
  public static async fromBuiltInKeywords(
    accessKey: string,
    keywords: BuiltInKeywords[],
    modelPath: string = '',
    sensitivities: number[] = []
  ) {
    try {
      let { handle, frameLength, sampleRate, version } =
        await RCTPorcupine.fromBuiltInKeywords(
          accessKey,
          modelPath,
          Object.values(keywords),
          sensitivities
        );
      return new Porcupine(handle, frameLength, sampleRate, version);
    } catch (err) {
      if (err instanceof PorcupineErrors.PorcupineError) {
        throw err;
      } else {
        const nativeError = err as NativeError;
        throw this.codeToError(nativeError.code, nativeError.message);
      }
    }
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
    modelPath: string = '',
    sensitivities: number[] = []
  ) {
    try {
      let { handle, frameLength, sampleRate, version } =
        await RCTPorcupine.fromKeywordPaths(
          accessKey,
          modelPath,
          keywordsPaths,
          sensitivities
        );
      return new Porcupine(handle, frameLength, sampleRate, version);
    } catch (err) {
      if (err instanceof PorcupineErrors.PorcupineError) {
        throw err;
      } else {
        const nativeError = err as NativeError;
        throw this.codeToError(nativeError.code, nativeError.message);
      }
    }
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
      throw new PorcupineErrors.PorcupineInvalidArgumentError(
        'Frame array provided to process() is undefined or null'
      );
    } else if (frame.length !== this._frameLength) {
      throw new PorcupineErrors.PorcupineInvalidArgumentError(
        `Size of frame array provided to 'process' (${frame.length}) does not match the engine 'frameLength' (${this._frameLength})`
      );
    }

    // sample the first frame to check for non-integer values
    if (!Number.isInteger(frame[0])) {
      throw new PorcupineErrors.PorcupineInvalidArgumentError(
        `Non-integer frame values provided to process(): ${frame[0]}. Porcupine requires 16-bit integers`
      );
    }

    try {
      return await RCTPorcupine.process(this._handle, frame);
    } catch (err) {
      const nativeError = err as NativeError;
      throw Porcupine.codeToError(nativeError.code, nativeError.message);
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
   * @returns Version of Porcupine
   */
  get version() {
    return this._version;
  }

  /**
   * Gets the Error type given a code.
   * @param code Code name of native Error.
   * @param message Detailed message of the error.
   */
  private static codeToError(code: string, message: string) {
    switch (code) {
      case 'PorcupineException':
        return new PorcupineErrors.PorcupineError(message);
      case 'PorcupineMemoryException':
        return new PorcupineErrors.PorcupineMemoryError(message);
      case 'PorcupineIOException':
        return new PorcupineErrors.PorcupineIOError(message);
      case 'PorcupineInvalidArgumentException':
        return new PorcupineErrors.PorcupineInvalidArgumentError(message);
      case 'PorcupineStopIterationException':
        return new PorcupineErrors.PorcupineStopIterationError(message);
      case 'PorcupineKeyException':
        return new PorcupineErrors.PorcupineKeyError(message);
      case 'PorcupineInvalidStateException':
        return new PorcupineErrors.PorcupineInvalidStateError(message);
      case 'PorcupineRuntimeException':
        return new PorcupineErrors.PorcupineRuntimeError(message);
      case 'PorcupineActivationException':
        return new PorcupineErrors.PorcupineActivationError(message);
      case 'PorcupineActivationLimitException':
        return new PorcupineErrors.PorcupineActivationLimitError(message);
      case 'PorcupineActivationThrottledException':
        return new PorcupineErrors.PorcupineActivationThrottledError(message);
      case 'PorcupineActivationRefusedException':
        return new PorcupineErrors.PorcupineActivationRefusedError(message);
      default:
        throw new Error(`unexpected code: ${code}, message: ${message}`);
    }
  }
}

export default Porcupine;
