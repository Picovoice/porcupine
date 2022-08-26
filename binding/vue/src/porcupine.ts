/*
  Copyright 2022 Picovoice Inc.

  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
  file accompanying this source.

  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
  specific language governing permissions and limitations under the License.
*/

import { WebVoiceProcessor } from '@picovoice/web-voice-processor';

import {
  BuiltInKeyword,
  PorcupineDetection,
  PorcupineKeyword,
  PorcupineModel,
  PorcupineOptions,
  PorcupineWorker,
} from '@picovoice/porcupine-web';

/**
 * Type alias for Porcupine Vue Mixin.
 * Use with `Vue as VueConstructor extends {$porcupine: PorcupineVue}` to get types in typescript.
 */
export interface PorcupineVue {
  $_porcupine_: PorcupineWorker | null,
  init: (
    accessKey: string,
    keywords: Array<PorcupineKeyword | BuiltInKeyword> | PorcupineKeyword | BuiltInKeyword,
    keywordDetectionCallback: (porcupineDetection: PorcupineDetection) => void,
    model: PorcupineModel,
    isLoadedCallback: (isLoaded: boolean) => void,
    isListeningCallback: (isListening: boolean) => void,
    options?: PorcupineOptions,
  ) => Promise<void>,
  start: () => Promise<void>,
  stop: () => Promise<void>,
  release: () => Promise<void>,
  keywordDetectionCallback: (porcupineDetection: PorcupineDetection) => void,
  isLoadedCallback: (isLoaded: boolean) => void,
  isListeningCallback: (isListening: boolean) => void,
  errorCallback: (error: string) => void,
}

export default {
  computed: {
    /**
     * Porcupine Vue Mixin.
     */
    $porcupine(): PorcupineVue {
      return {
        $_porcupine_: null as PorcupineWorker | null,
        keywordDetectionCallback: (porcupineDetection: PorcupineDetection) => { return; },
        isLoadedCallback: (isLoaded: boolean) => { return; },
        isListeningCallback: (isListening: boolean) => { return; },
        errorCallback: (error: string) => { console.error(error); },
        async init(
          accessKey: string,
          keywords: Array<PorcupineKeyword | BuiltInKeyword> | PorcupineKeyword | BuiltInKeyword,
          keywordDetectionCallback: (porcupineDetection: PorcupineDetection) => void,
          model: PorcupineModel,
          isLoadedCallback: (isLoaded: boolean) => void,
          isListeningCallback: (isListening: boolean) => void,
          options: PorcupineOptions = {},
        ): Promise<void> {
          try {
            if (options.processErrorCallback) {
              this.errorCallback = options.processErrorCallback;
            }

            this.$_porcupine_ = await PorcupineWorker.create(
              accessKey,
              keywords,
              keywordDetectionCallback,
              model,
              options
            );

            this.isListeningCallback = isListeningCallback;
            this.isLoadedCallback = isLoadedCallback;
            isLoadedCallback(true);
          } catch (error: any) {
            this.errorCallback(error);
          }
        },
        /**
         * Start processing audio.
         */
        async start(): Promise<void> {
          try {
            if (!this.$_porcupine_) {
              this.errorCallback('Porcupine not initialized');
              return;
            }
            await WebVoiceProcessor.subscribe(this.$_porcupine_);
            this.isListeningCallback(true);
          } catch (error: any) {
            this.errorCallback(error);
          }
        },
        /**
         * Stop processing audio.
         */
        async stop(): Promise<void> {
          try {
            if (!this.$_porcupine_) {
              this.errorCallback('Porcupine not initialized');
              return;
            }
            await WebVoiceProcessor.unsubscribe(this.$_porcupine_);
            this.isListeningCallback(false);
          } catch (error: any) {
            this.errorCallback(error);
          }
        },
        async release(): Promise<void> {
          if (this.$_porcupine_) {
            await WebVoiceProcessor.unsubscribe(this.$_porcupine_);
            this.$_porcupine_.terminate();
            this.$_porcupine_ = null;

            this.isLoadedCallback(false);
          }
        }
      };
    }
  },
  // Vue 3 method to clean resources.
  beforeUnmount(this: any): void {
    this.$porcupine.release();
  },
  // Vue 2 method to clean resources.
  beforeDestroy(this: any): void {
    this.$porcupine.release();
  }
};
