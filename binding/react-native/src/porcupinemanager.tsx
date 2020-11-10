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

import { VoiceProcessor, BufferEmitter } from 'react-native-voice-processor';
import { EventSubscription, NativeEventEmitter } from 'react-native';

import Porcupine from './porcupine';
export type DetectionCallback = (keywordIndex: number) => void;

class PorcupineManager {
  private _voiceProcessor: VoiceProcessor;
  private _porcupine: Porcupine;
  private _detectionCallback: DetectionCallback;
  private _bufferListener?: EventSubscription;
  private _bufferEmitter: NativeEventEmitter;

  // a list of build-in keywords
  static KEYWORDS = Porcupine.KEYWORDS;

  /**
   * Static creator for initializing a Porcupine Manager from built-in keywords
   * @param keywords List of keywords (phrases) for detection. The list of available (default) keywords can be retrieved
   * using `Porcupine.KEYWORDS`. If `keyword_paths` is set then this argument will be ignored.
   * @param detectionCallback A callback for when Porcupine detects the specified keywords
   * @param modelPath Path to the file containing model parameters. If not set it will be set to the default location.
   * @param sensitivities sensitivities for each keywords model. A higher sensitivity reduces miss rate
   * at the cost of potentially higher false alarm rate. Sensitivity should be a floating-point number within
   * [0, 1].
   * @returns An instance of the Porcupine Manager
   */
  static async fromKeywords(
    keywords: string[],
    detectionCallback: DetectionCallback,
    modelPath?: string,
    sensitivities?: number[]
  ) {
    try {
      let porcupine = await Porcupine.fromKeywords(
        keywords,
        modelPath,
        sensitivities
      );
      return new PorcupineManager(porcupine, detectionCallback);
    } catch (e) {
      throw e;
    }
  }

  /**
   * Static creator for initializing a Porcupine Manager from paths to custom keywords
   * @param keywordPaths Absolute paths to keyword model files.
   * @param detectionCallback A callback for when Porcupine detects the specified keywords
   * @param modelPath Path to the file containing model parameters. If not set it will be set to the default location.
   * @param sensitivities sensitivities for each keywords model. A higher sensitivity reduces miss rate
   * at the cost of potentially higher false alarm rate. Sensitivity should be a floating-point number within
   * [0, 1].
   * @returns An instance of the Porcupine Manager
   */
  static async fromKeywordPaths(
    keywordPaths: string[],
    detectionCallback: DetectionCallback,
    modelPath?: string,
    sensitivities?: number[]
  ) {
    try {
      let porcupine = await Porcupine.fromKeywordPaths(
        keywordPaths,
        modelPath,
        sensitivities
      );
      return new PorcupineManager(porcupine, detectionCallback);
    } catch (e) {
      throw e;
    }
  }

  constructor(porcupine: Porcupine, detectionCallback: DetectionCallback) {
    this._detectionCallback = detectionCallback;
    this._porcupine = porcupine;
    this._voiceProcessor = new VoiceProcessor(
      porcupine.frameLength,
      porcupine.sampleRate
    );
    this._bufferEmitter = new NativeEventEmitter(BufferEmitter);
    this._bufferListener = this._bufferEmitter.addListener(
      BufferEmitter.BUFFER_EMITTER_KEY,
      async (buffer: number[]) => {
        try {
          let keywordIndex: number = await this._porcupine.process(buffer);
          if (keywordIndex >= 0) {
            this._detectionCallback(keywordIndex);
          }
        } catch (e) {
          console.error(e);
        }
      }
    );
  }

  /**
   * Opens audio input stream and sends frames to Porcupine for wake word detection
   */
  start() {
    this._voiceProcessor.start();
  }

  /**
   * Closes audio stream
   */
  stop() {
    this._voiceProcessor.stop();
  }

  /**
   * Releases resources and listeners
   */
  delete() {
    this._bufferListener?.remove();
    this._porcupine.delete();
  }
}

export default PorcupineManager;
