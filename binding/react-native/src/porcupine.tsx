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
const RCTPorcupine = NativeModules.PvPorcupine;

class Porcupine {
  private _handle: string;
  private _frameLength: number;
  private _sampleRate: number;
  private _version: string;

  // a list of built-in keywords
  public static KEYWORDS = Object.keys(RCTPorcupine.KEYWORD_PATHS);

  /**
   * Static creator for initializing Porcupine from one of the built-in keywords
   * @param keywords List of keywords (phrases) for detection. The list of available (default) keywords can be retrieved
   * using `Porcupine.KEYWORDS`. If `keyword_paths` is set then this argument will be ignored.
   * @param modelPath Path to the file containing model parameters. If not set it will be set to the default location.
   * @param sensitivities sensitivities for each keywords model. A higher sensitivity reduces miss rate
   * at the cost of potentially higher false alarm rate. Sensitivity should be a floating-point number within
   * [0, 1].
   * @returns An instance of the engine.
   */
  public static async fromKeywords(
    keywords: string[],
    modelPath?: string,
    sensitivities?: number[]
  ) {
    let { handle, frameLength, sampleRate, version } = await Porcupine.create(
      keywords,
      undefined,
      modelPath,
      sensitivities
    );
    return new Porcupine(handle, frameLength, sampleRate, version);
  }

  /**
   * Static creator for initializing Porcupine from a list of paths to custom keyword files
   * @param keywordPaths Absolute paths to keyword model files.
   * @param modelPath Path to the file containing model parameters. If not set it will be set to the default location.
   * @param sensitivities sensitivities for each keywords model. A higher sensitivity reduces miss rate
   * at the cost of potentially higher false alarm rate. Sensitivity should be a floating-point number within
   * [0, 1].
   * @returns An instance of the engine.
   */
  public static async fromKeywordPaths(
    keywordsPaths: string[],
    modelPath?: string,
    sensitivities?: number[]
  ) {
    let { handle, frameLength, sampleRate, version } = await Porcupine.create(
      undefined,
      keywordsPaths,
      modelPath,
      sensitivities
    );
    return new Porcupine(handle, frameLength, sampleRate, version);
  }

  /**
   * Creates an instance of wake word engine (Porcupine).
   * @param keywords List of keywords (phrases) for detection. The list of available (default) keywords can be retrieved
   * using `Porcupine.KEYWORDS`. If `keyword_paths` is set then this argument will be ignored.
   * @param keywordPaths Absolute paths to keyword model files. If not set it will be populated from `keywords` argument.
   * @param modelPath Path to the file containing model parameters. If not set it will be set to the default location.
   * @param sensitivities sensitivities for each keywords model. A higher sensitivity reduces miss rate
   * at the cost of potentially higher false alarm rate. Sensitivity should be a floating-point number within
   * [0, 1].
   * @returns An instance of the engine.
   */
  private static async create(
    keywords?: string[],
    keywordPaths?: string[],
    modelPath?: string,
    sensitivities?: number[]
  ) {
    if (modelPath === undefined) {
      modelPath = RCTPorcupine.DEFAULT_MODEL_PATH;
    }

    if (Array.isArray(keywords) && Array.isArray(keywordPaths)) {
      return Promise.reject(
        `Both 'keywords' and 'keywordPaths' were set. Only one of the two arguments may be set for intializtion.`
      );
    }

    if (
      keywordPaths === undefined ||
      keywordPaths.length === 0 ||
      !Array.isArray(keywordPaths)
    ) {
      if (
        keywords === undefined ||
        keywords.length === 0 ||
        !Array.isArray(keywords)
      ) {
        return Promise.reject(
          `Either 'keywords' or 'keywordPaths' must be set.`
        );
      }

      keywordPaths = [];
      for (let i = 0; i < keywords.length; i++) {
        let keywordPath = RCTPorcupine.KEYWORD_PATHS[keywords[i]];
        if (keywordPath !== undefined) {
          keywordPaths[i] = keywordPath;
        } else {
          return Promise.reject(
            `One or more keywords are not available by default. Available default keywords are: ${Object.keys(
              RCTPorcupine.KEYWORD_PATHS
            )}`
          );
        }
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
        return Promise.reject(
          `Sensitivity value in 'sensitivities' not in range [0,1]: ${sensitivity}`
        );
      }
    }

    if (keywordPaths.length !== sensitivities.length) {
      return Promise.reject(
        `Number of keywords (${keywordPaths.length}) does not match number of sensitivities (${sensitivities.length})`
      );
    }

    return RCTPorcupine.create(modelPath, keywordPaths, sensitivities);
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
      return Promise.reject(
        `Frame array provided to process() is undefined or null`
      );
    } else if (frame.length !== this._frameLength) {
      return Promise.reject(
        `Size of frame array provided to 'process' (${frame.length}) does not match the engine 'frameLength' (${this._frameLength})`
      );
    }

    // sample the first frame to check for non-integer values
    if (!Number.isInteger(frame[0])) {
      return Promise.reject(
        `Non-integer frame values provided to process(): ${frame[0]}. Porcupine requires 16-bit integers`
      );
    }

    return RCTPorcupine.process(this._handle, frame);
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
}

export default Porcupine;
