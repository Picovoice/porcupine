/*
  Copyright 2021-2022 Picovoice Inc.

  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
  file accompanying this source.

  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
  specific language governing permissions and limitations under the License.
*/

import { useState, useCallback, useRef } from 'react';

import { WebVoiceProcessor } from '@picovoice/web-voice-processor';

import {
  BuiltInKeyword,
  PorcupineDetection,
  PorcupineKeyword,
  PorcupineModel,
  PorcupineOptions,
  PorcupineWorker,
} from '@picovoice/porcupine-web';

export const usePorcupine = (): {
  wakeWordDetection: PorcupineDetection | null,
  isLoaded: boolean,
  isListening: boolean,
  error: string | null,
  init: (
    accessKey: string,
    keywords: Array<PorcupineKeyword | BuiltInKeyword> | PorcupineKeyword | BuiltInKeyword,
    model: PorcupineModel,
    options?: PorcupineOptions,
  ) => Promise<void>,
  start: () => Promise<void>,
  stop: () => Promise<void>,
  release: () => Promise<void>,
} => {
  const porcupineRef = useRef<PorcupineWorker | null>(null);
  const [wakeWordDetection, setWakeWordDetection] = useState<PorcupineDetection | null>(null);

  const [isLoaded, setIsLoaded] = useState(false);
  const [isListening, setIsListening] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const keywordDetectionCallback = useCallback((keyword: PorcupineDetection) => {
    setWakeWordDetection(keyword);
  }, []);

  const errorCallback = useCallback((e: string) => {
    setError(e);
  }, []);

  const init = useCallback(async (
    accessKey: string,
    keywords: Array<PorcupineKeyword | BuiltInKeyword> | PorcupineKeyword | BuiltInKeyword,
    model: PorcupineModel,
    options: PorcupineOptions = {},
  ): Promise<void> => {
    if (options.processErrorCallback) {
      console.warn(`'processErrorCallback' options is not supported, use 'error' state to handle errors.`);
    }

    if (porcupineRef.current !== null) {
      porcupineRef.current = await PorcupineWorker.create(
        accessKey,
        keywords,
        keywordDetectionCallback,
        model,
        { ...options, processErrorCallback: errorCallback }
      );
      setIsLoaded(true);
      setError(null);
    }
  }, [keywordDetectionCallback, errorCallback]);

  const start = useCallback(async (): Promise<void> => {
    try {
      if (!porcupineRef.current) {
        setError("Porcupine not initialized");
        return;
      }

      await WebVoiceProcessor.subscribe(porcupineRef.current);
      setIsListening(true);
    } catch (e: any) {
      setError(e.toString());
      setIsListening(false);
    }
  }, []);

  const stop = useCallback(async (): Promise<void> => {
    try {
      if (!porcupineRef.current) {
        setError("Porcupine not initialized");
        return;
      }

      await WebVoiceProcessor.unsubscribe(porcupineRef.current);
      setIsListening(false);
    } catch (e: any) {
      setError(e.toString());
      setIsListening(false);
    }
  }, []);

  const release = useCallback(async (): Promise<void> => {
    if (porcupineRef.current) {
      await stop();
      porcupineRef.current.terminate();
      porcupineRef.current = null;

      setIsLoaded(false);
    }
  }, []);

  return {
    wakeWordDetection,
    isLoaded,
    isListening,
    error,
    init,
    start,
    stop,
    release,
  };
};

