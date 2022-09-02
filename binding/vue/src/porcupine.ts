/*
  Copyright 2022 Picovoice Inc.

  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
  file accompanying this source.

  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
  specific language governing permissions and limitations under the License.
*/

import { computed, ComputedRef, ref } from 'vue';

import { WebVoiceProcessor } from '@picovoice/web-voice-processor';

import {
  BuiltInKeyword,
  PorcupineDetection,
  PorcupineKeyword,
  PorcupineModel,
  PorcupineOptions,
  PorcupineWorker,
} from '@picovoice/porcupine-web';

export function usePorcupine(): {
  keywordDetection: ComputedRef<PorcupineDetection | null>,
  isLoaded: ComputedRef<boolean>,
  isListening: ComputedRef<boolean>,
  error: ComputedRef<string | null>,
  init: (
    accessKey: string,
    keywords: Array<PorcupineKeyword | BuiltInKeyword> | PorcupineKeyword | BuiltInKeyword,
    model: PorcupineModel,
    options?: PorcupineOptions,
  ) => Promise<void>,
  start: () => Promise<void>,
  stop: () => Promise<void>,
  release: () => Promise<void>,
  } {
  const porcupineRef = ref<PorcupineWorker | null>(null);

  const keywordDetectionRef = ref<PorcupineDetection | null>(null);
  const isLoadedRef = ref(false);
  const isListeningRef = ref(false);
  const errorRef = ref<string | null>(null);

  const keywordDetection = computed(() => keywordDetectionRef.value);
  const isLoaded = computed(() => isLoadedRef.value);
  const isListening = computed(() => isListeningRef.value);
  const error = computed(() => errorRef.value);

  const keywordDetectionCallback = (porcupineDetection: PorcupineDetection): void => {
    keywordDetectionRef.value = porcupineDetection;
  };

  const errorCallback = (e: any): void => {
    errorRef.value = e.toString();
  };

  const init = async (
    accessKey: string,
    keywords: Array<PorcupineKeyword | BuiltInKeyword> | PorcupineKeyword | BuiltInKeyword,
    model: PorcupineModel,
    options: PorcupineOptions = {},
  ): Promise<void> => {
    if (options.processErrorCallback) {
      console.warn("'processErrorCallback' is only supported in the Porcupine Web SDK. Watch the 'error' ref to monitor for errors in the Vue SDK.");
    }

    try {
      if (!porcupineRef.value) {
        porcupineRef.value = await PorcupineWorker.create(
          accessKey,
          keywords,
          keywordDetectionCallback,
          model,
          { ...options, processErrorCallback: errorCallback }
        );
        isLoadedRef.value = true;
        errorRef.value = null;
      }
    } catch (e: any) {
      errorRef.value = e.toString();
    }
  };

  const start = async (): Promise<void> => {
    try {
      if (!porcupineRef.value) {
        errorRef.value = "Porcupine has not been initialized or has been released";
        return;
      }

      await WebVoiceProcessor.subscribe(porcupineRef.value);
      isListeningRef.value = true;
      errorRef.value = null;
    } catch (e: any) {
      errorRef.value = e.toString();
      isListeningRef.value = false;
    }
  };

  const stop = async (): Promise<void> => {
    try {
      if (!porcupineRef.value) {
        errorRef.value = "Porcupine has not been initialized or has been released";
        return;
      }

      await WebVoiceProcessor.unsubscribe(porcupineRef.value);
      isListeningRef.value = false;
      errorRef.value = null;
    } catch (e: any) {
      errorRef.value = e.toString();
      isListeningRef.value = false;
    }
  };

  const release = async (): Promise<void> => {
    if (porcupineRef.value) {
      await stop();
      porcupineRef.value.terminate();
      porcupineRef.value = null;

      isLoadedRef.value = false;
    }
  };

  return {
    keywordDetection,
    isLoaded,
    isListening,
    error,
    init,
    start,
    stop,
    release,
  };
}
