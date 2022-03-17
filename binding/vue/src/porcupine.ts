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
  PorcupineKeyword,
  PorcupineWorker,
  PorcupineWorkerFactory
} from "@picovoice/porcupine-web-core";

/**
* Type alias for PorcupineWorkerFactory arguments.
*/
export type PorcupineWorkerFactoryArgs = {
  accessKey: string;
  keywords: Array<PorcupineKeyword | string> | PorcupineKeyword | string;
  start?: boolean;
};

/**
 * Type alias for Porcupine Vue Mixin.
 * Use with `Vue as VueConstructor extends {$porcupine: PorcupineVue}` to get types in typescript.
 */
export interface PorcupineVue {
  $_ppnWorker_: Worker | null;
  $_webVp_: WebVoiceProcessor | null;
  init: (
    porcupineFactoryArgs: PorcupineWorkerFactoryArgs,
    porcupineFactory: PorcupineWorkerFactory,
    keywordCallback: (label: string) => void,
    readyCallback: () => void,
    errorCallback: (error: string | Error) => void) => void;
  start: () => Promise<boolean>;
  stop: () => Promise<boolean>;
  pause: () => boolean;
  delete: () => void;
}

export default {
  computed: {
    /**
     * Porcupine Vue Mixin.
     */
    $porcupine(): PorcupineVue {
      return {
        $_ppnWorker_: null as PorcupineWorker | null,
        $_webVp_: null as WebVoiceProcessor | null,
        /**
         * Init function for Porcupine.
         * 
         * @param porcupineFactoryArgs Arguments for PorcupineWorkerFactory.
         * @param porcupineFactory The language-specific worker factory
         * @param keywordCallback A method invoked upon detection of the keywords.
         * @param readyCallback A method invoked after component has initialized.
         * @param errorCallback A method invoked if an error occurs within `PorcupineWorkerFactory`.
         */
        async init(
          porcupineFactoryArgs,
          porcupineFactory,
          keywordCallback = (_: string) => {},
          readyCallback = () => {},
          errorCallback = (error: string | Error) => {console.error(error)}
        ) {
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
        },
        /**
         * Delete used resources.
         */
        delete() {
          this.$_webVp_?.release();
          this.$_ppnWorker_?.postMessage({ command: 'release' });
          this.$_ppnWorker_?.terminate();
        }
      }
    }
  },
  // Vue 3 method to clean resources.
  beforeUnmount(this: any) {
    this.$porcupine.delete();
  },
  // Vue 2 method to clean resources.
  beforeDestory(this: any) {
    this.$porcupine.delete();
  }
};
