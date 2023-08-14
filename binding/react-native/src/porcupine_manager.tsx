//
// Copyright 2020-2023 Picovoice Inc.
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
  VoiceProcessorError,
  VoiceProcessorErrorListener,
  VoiceProcessorFrameListener,
} from '@picovoice/react-native-voice-processor';

import Porcupine from './porcupine';
import * as PorcupineErrors from './porcupine_errors';
import type BuiltInKeywords from './builtin_keywords';
export type DetectionCallback = (keywordIndex: number) => void;
export type ProcessErrorCallback = (
  error: PorcupineErrors.PorcupineError
) => void;

class PorcupineManager {
  private readonly _voiceProcessor: VoiceProcessor;
  private readonly _errorListener: VoiceProcessorErrorListener;
  private readonly _frameListener: VoiceProcessorFrameListener;
  private _porcupine: Porcupine | null;

  private _isListening: boolean = false;

  /**
   * Static creator for initializing a Porcupine Manager from built-in keywords
   *
   * @param accessKey AccessKey obtained from Picovoice Console (https://console.picovoice.ai/).
   * @param keywords List of keywords (phrases) for detection. The list of available (default) keywords can be retrieved.
   * using `Porcupine.KEYWORDS`. If `keyword_paths` is set then this argument will be ignored.
   * @param detectionCallback A callback for when Porcupine detects the specified keywords.
   * @param processErrorCallback A callback for when Porcupine process function triggers an error.
   * @param modelPath Path to the file containing model parameters. If not set it will be set to the default location.
   * @param sensitivities sensitivities for each keyword model. A higher sensitivity reduces miss rate
   * at the cost of potentially higher false alarm rate. Sensitivity should be a floating-point number within
   * [0, 1].
   * @returns An instance of the Porcupine Manager.
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
   * Static creator for initializing a Porcupine Manager from paths to custom keywords.
   *
   * @param accessKey AccessKey obtained from Picovoice Console (https://console.picovoice.ai/).
   * @param keywordPaths Absolute paths to keyword model files.
   * @param detectionCallback A callback for when Porcupine detects the specified keywords.
   * @param processErrorCallback A callback for when Porcupine process function triggers an error.
   * @param modelPath Path to the file containing model parameters. If not set it will be set to the default location.
   * @param sensitivities sensitivities for each keyword model. A higher sensitivity reduces miss rate
   * at the cost of potentially higher false alarm rate. Sensitivity should be a floating-point number within
   * [0, 1].
   * @returns An instance of the Porcupine Manager.
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
    this._porcupine = porcupine;
    this._voiceProcessor = VoiceProcessor.instance;
    this._frameListener = async (frame: number[]) => {
      if (this._porcupine === null || !this._isListening) {
        return;
      }
      try {
        let keywordIndex: number = await this._porcupine.process(frame);
        if (keywordIndex >= 0) {
          detectionCallback(keywordIndex);
        }
      } catch (e) {
        if (processErrorCallback) {
          processErrorCallback(e as PorcupineErrors.PorcupineError);
        } else {
          console.error(e);
        }
      }
    };

    this._errorListener = (error: VoiceProcessorError) => {
      if (processErrorCallback) {
        processErrorCallback(new PorcupineErrors.PorcupineError(error.message));
      } else {
        console.error(error);
      }
    };
  }

  /**
   * Opens audio input stream and sends audio frames to Porcupine
   */
  public async start(): Promise<void> {
    if (this._isListening) {
      return;
    }

    if (this._porcupine === null) {
      throw new PorcupineErrors.PorcupineInvalidStateError(
        'Cannot start Porcupine - resources have already been released'
      );
    }

    if (await this._voiceProcessor.hasRecordAudioPermission()) {
      this._voiceProcessor.addFrameListener(this._frameListener);
      this._voiceProcessor.addErrorListener(this._errorListener);
      try {
        await this._voiceProcessor.start(
          this._porcupine.frameLength,
          this._porcupine.sampleRate
        );
      } catch (e: any) {
        throw new PorcupineErrors.PorcupineRuntimeError(
          `Failed to start audio recording: ${e.message}`
        );
      }
    } else {
      throw new PorcupineErrors.PorcupineRuntimeError(
        'User did not give permission to record audio.'
      );
    }

    this._isListening = true;
  }

  /**
   * Closes audio stream
   */
  public async stop(): Promise<void> {
    if (!this._isListening) {
      return;
    }

    this._voiceProcessor.removeErrorListener(this._errorListener);
    this._voiceProcessor.removeFrameListener(this._frameListener);

    if (this._voiceProcessor.numFrameListeners === 0) {
      try {
        await this._voiceProcessor.stop();
      } catch (e: any) {
        throw new PorcupineErrors.PorcupineRuntimeError(
          `Failed to stop audio recording: ${e.message}`
        );
      }
    }

    this._isListening = false;
  }

  /**
   * Releases resources and listeners
   */
  delete() {
    if (this._porcupine !== null) {
      this._porcupine.delete();
      this._porcupine = null;
    }
  }
}

export default PorcupineManager;
