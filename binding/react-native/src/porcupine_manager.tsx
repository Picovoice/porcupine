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

import {
  VoiceProcessor,
  BufferEmitter,
} from '@picovoice/react-native-voice-processor';
import { EventSubscription, NativeEventEmitter } from 'react-native';

import Porcupine from './porcupine';
import type BuiltInKeywords from './builtin_keywords';
import type * as PorcupineErrors from './porcupine_errors';
export type DetectionCallback = (keywordIndex: number) => void;
export type ProcessErrorCallback = (
  error: PorcupineErrors.PorcupineError
) => void;

class PorcupineManager {
  private _voiceProcessor: VoiceProcessor;
  private _porcupine: Porcupine | null;
  private _detectionCallback: DetectionCallback;
  private _processErrorCallback?: ProcessErrorCallback;
  private _bufferListener?: EventSubscription;
  private _bufferEmitter: NativeEventEmitter;

  /**
   * Static creator for initializing a Porcupine Manager from built-in keywords
   * @param accessKey AccessKey obtained from Picovoice Console (https://console.picovoice.ai/).
   * @param keywords List of keywords (phrases) for detection. The list of available (default) keywords can be retrieved
   * using `Porcupine.KEYWORDS`. If `keyword_paths` is set then this argument will be ignored.
   * @param detectionCallback A callback for when Porcupine detects the specified keywords
   * @param processErrorCallback A callback for when Porcupine process function triggers an error.
   * @param modelPath Path to the file containing model parameters. If not set it will be set to the default location.
   * @param sensitivities sensitivities for each keywords model. A higher sensitivity reduces miss rate
   * at the cost of potentially higher false alarm rate. Sensitivity should be a floating-point number within
   * [0, 1].
   * @returns An instance of the Porcupine Manager
   */
  public static async fromBuiltInKeywords(
    accessKey: string,
    keywords: BuiltInKeywords[],
    detectionCallback: DetectionCallback,
    processErrorCallback?: ProcessErrorCallback,
    modelPath?: string,
    sensitivities?: number[]
  ) {
    let porcupine = await Porcupine.fromBuiltInKeywords(
      accessKey,
      keywords,
      modelPath,
      sensitivities
    );
    return new PorcupineManager(
      porcupine,
      detectionCallback,
      processErrorCallback
    );
  }

  /**
   * Static creator for initializing a Porcupine Manager from paths to custom keywords
   * @param accessKey AccessKey obtained from Picovoice Console (https://console.picovoice.ai/).
   * @param keywordPaths Absolute paths to keyword model files.
   * @param processErrorCallback A callback for when Porcupine detects the specified keywords
   * @param modelPath Path to the file containing model parameters. If not set it will be set to the default location.
   * @param sensitivities sensitivities for each keywords model. A higher sensitivity reduces miss rate
   * at the cost of potentially higher false alarm rate. Sensitivity should be a floating-point number within
   * [0, 1].
   * @returns An instance of the Porcupine Manager
   */
  public static async fromKeywordPaths(
    accessKey: string,
    keywordPaths: string[],
    detectionCallback: DetectionCallback,
    processErrorCallback?: ProcessErrorCallback,
    modelPath?: string,
    sensitivities?: number[]
  ) {
    let porcupine = await Porcupine.fromKeywordPaths(
      accessKey,
      keywordPaths,
      modelPath,
      sensitivities
    );
    return new PorcupineManager(
      porcupine,
      detectionCallback,
      processErrorCallback
    );
  }

  private constructor(
    porcupine: Porcupine,
    detectionCallback: DetectionCallback,
    processErrorCallback?: ProcessErrorCallback
  ) {
    this._detectionCallback = detectionCallback;
    this._processErrorCallback = processErrorCallback;
    this._porcupine = porcupine;
    this._voiceProcessor = VoiceProcessor.getVoiceProcessor(
      porcupine.frameLength,
      porcupine.sampleRate
    );
    this._bufferEmitter = new NativeEventEmitter(BufferEmitter);
    this._bufferListener = this._bufferEmitter.addListener(
      BufferEmitter.BUFFER_EMITTER_KEY,
      async (buffer: number[]) => {
        if (this._porcupine === null) return;

        try {
          let keywordIndex: number = await this._porcupine.process(buffer);
          if (keywordIndex >= 0) {
            this._detectionCallback(keywordIndex);
          }
        } catch (e) {
          if (this._processErrorCallback) {
            this._processErrorCallback(e as PorcupineErrors.PorcupineError);
          } else {
            console.error(e);
          }
        }
      }
    );
  }

  /**
   * Opens audio input stream and sends audio frames to Porcupine
   */
  async start() {
    return this._voiceProcessor.start();
  }

  /**
   * Closes audio stream
   */
  async stop() {
    return this._voiceProcessor.stop();
  }

  /**
   * Releases resources and listeners
   */
  delete() {
    this._bufferListener?.remove();
    if (this._porcupine != null) {
      this._porcupine.delete();
      this._porcupine = null;
    }
  }
}

export default PorcupineManager;
