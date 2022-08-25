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
    options?: PorcupineOptions,
  ) => Promise<void>,
  start: () => Promise<void>,
  pause: () => Promise<void>,
  stop: () => Promise<void>,
  $_keywordDetectionCallback_: (porcupineDetection: PorcupineDetection) => void,
  isLoaded: boolean,
  isListening: boolean,
  error: Error | string | null,
}

export default {
  computed: {
    /**
     * Porcupine Vue Mixin.
     */
    $porcupine(): PorcupineVue {
      return {
        $_porcupine_: null as PorcupineWorker | null,
        isLoaded: false,
        isListening: false,
        error: null,
        async init(
          accessKey: string,
          keywords: Array<PorcupineKeyword | BuiltInKeyword> | PorcupineKeyword | BuiltInKeyword,
          keywordDetectionCallback: (porcupineDetection: PorcupineDetection) => void,
          model: PorcupineModel,
          options: PorcupineOptions = {},
        ): Promise<void> {
          try {
            const { accessKey, keywords } = porcupineFactoryArgs;
            this.$_ppnWorker_ = await porcupineFactory.create(
              accessKey,
              JSON.parse(JSON.stringify(keywords)),
              keywordCallback,
              errorCallback
            );
            this.$_webVp_ = await WebVoiceProcessor.init({
              engines: [this.$_ppnWorker_!],
            });
            readyCallback();
          } catch (error) {
            errorCallback(error as Error);
          }
        },
        /**
         * Start processing audio.
         */
        async start() {
          if (this.$_webVp_ !== null) {
            await this.$_webVp_.start();
            return true;
          } else {
            return false;
          }
        },
        /**
         * Pause processing audio.
         */
        pause() {
          if (this.$_webVp_ !== null) {
            this.$_webVp_.pause();
            return true;
          } else {
            return false;
          }
        },
        /**
         * Stop processing audio.
         */
        async stop() {
          if (this.$_webVp_ !== null) {
            await this.$_webVp_.stop();
            return true;
          } else {
            return false;
          }
        }
      }
    }
  },
  // Vue 3 method to clean resources.
  beforeUnmount(this: any) {
    this.$porcupine.delete();
  },
  // Vue 2 method to clean resources.
  beforeDestroy(this: any) {
    this.$porcupine.delete();
  }
};
