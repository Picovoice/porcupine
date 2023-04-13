/*
  Copyright 2022-2023 Picovoice Inc.

  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
  file accompanying this source.

  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
  specific language governing permissions and limitations under the License.
*/

import { reactive, Ref, ref, UnwrapNestedRefs, UnwrapRef, version } from 'vue';

import { WebVoiceProcessor } from '@picovoice/web-voice-processor';

import {
  BuiltInKeyword,
  PorcupineDetection,
  PorcupineKeyword,
  PorcupineModel,
  PorcupineOptions,
  PorcupineWorker,
} from '@picovoice/porcupine-web';

const createRef = <T>(data: T): Ref<UnwrapRef<T>> => {
  if (!ref || !version || version.charAt(0) < "3") {
    const obj = {
      value: data
    };

    return new Proxy(obj as Ref<UnwrapRef<T>>, {
      get(target, property, receiver): T {
        return Reflect.get(target, property, receiver);
      },
      set(target, property, newValue: T, receiver): boolean {
        return Reflect.set(target, property, newValue, receiver);
      }
    });
  }

  return ref<T>(data);
};

const createReactive = <T extends object>(data: T): UnwrapNestedRefs<T> => {
  if (!reactive || !version || version.charAt(0) < "3") {
    return data as UnwrapNestedRefs<T>;
  }

  return reactive<T>(data);
};

export type PorcupineVue = {
  state: {
    keywordDetection: PorcupineDetection | null,
    isLoaded: boolean,
    isListening: boolean,
    error: Error | null,
  },
  init: (
    accessKey: string,
    keywords: Array<PorcupineKeyword | BuiltInKeyword> | PorcupineKeyword | BuiltInKeyword,
    model: PorcupineModel,
    options?: PorcupineOptions,
  ) => Promise<void>,
  start: () => Promise<void>,
  stop: () => Promise<void>,
  release: () => Promise<void>,
};

export function usePorcupine(): PorcupineVue {
  const porcupineRef = createRef<PorcupineWorker | null>(null);

  const state = createReactive<{
    keywordDetection: PorcupineDetection | null,
    isLoaded: boolean,
    isListening: boolean,
    error: Error | null,
  }>({
    keywordDetection: null,
    isLoaded: false,
    isListening: false,
    error: null,
  });

  const keywordDetectionCallback = (porcupineDetection: PorcupineDetection): void => {
    state.keywordDetection = porcupineDetection;
  };

  const errorCallback = (e: Error): void => {
    state.error = e;
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
        state.isLoaded = true;
        state.error = null;
      }
    } catch (e: any) {
      state.error = e;
    }
  };

  const start = async (): Promise<void> => {
    try {
      if (!porcupineRef.value) {
        state.error = new Error("Porcupine has not been initialized or has been released");
        return;
      }

      await WebVoiceProcessor.subscribe(porcupineRef.value);
      state.isListening = true;
      state.error = null;
    } catch (e: any) {
      state.error = e;
      state.isListening = false;
    }
  };

  const stop = async (): Promise<void> => {
    try {
      if (!porcupineRef.value) {
        state.error = new Error("Porcupine has not been initialized or has been released");
        return;
      }

      await WebVoiceProcessor.unsubscribe(porcupineRef.value);
      state.isListening = false;
      state.error = null;
    } catch (e: any) {
      state.error = e;
      state.isListening = false;
    }
  };

  const release = async (): Promise<void> => {
    if (porcupineRef.value) {
      await stop();
      porcupineRef.value.terminate();
      porcupineRef.value = null;

      state.isLoaded = false;
    }
  };

  return {
    state,
    init,
    start,
    stop,
    release,
  };
}
