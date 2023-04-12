/*
  Copyright 2021-2023 Picovoice Inc.

  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
  file accompanying this source.

  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
  specific language governing permissions and limitations under the License.
*/

import { useState, useCallback, useRef, useEffect } from 'react';

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
  keywordDetection: PorcupineDetection | null,
  isLoaded: boolean,
  isListening: boolean,
  error: Error | null,
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
  const [keywordDetection, setKeywordDetection] = useState<PorcupineDetection | null>(null);

  const [isLoaded, setIsLoaded] = useState(false);
  const [isListening, setIsListening] = useState(false);
  const [error, setError] = useState<Error | null>(null);

  const keywordDetectionCallback = useCallback((keyword: PorcupineDetection) => {
    setKeywordDetection(keyword);
  }, []);

  const errorCallback = useCallback((e: Error) => {
    setError(e);
  }, []);

  const init = useCallback(async (
    accessKey: string,
    keywords: Array<PorcupineKeyword | BuiltInKeyword> | PorcupineKeyword | BuiltInKeyword,
    model: PorcupineModel,
    options: PorcupineOptions = {},
  ): Promise<void> => {
    if (options.processErrorCallback) {
      // eslint-disable-next-line no-console
      console.warn("'processErrorCallback' is only supported in the Porcupine Web SDK. Use the 'error' state to monitor for errors in the React SDK.");
    }

    try {
      if (!porcupineRef.current) {
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
    } catch (e: any) {
      setError(e);
    }
  }, [keywordDetectionCallback, errorCallback]);

  const start = useCallback(async (): Promise<void> => {
    try {
      if (!porcupineRef.current) {
        setError(new Error("Porcupine has not been initialized or has been released"));
        return;
      }

      await WebVoiceProcessor.subscribe(porcupineRef.current);
      setIsListening(true);
      setError(null);
    } catch (e: any) {
      setError(e);
      setIsListening(false);
    }
  }, []);

  const stop = useCallback(async (): Promise<void> => {
    try {
      if (!porcupineRef.current) {
        setError(new Error("Porcupine has not been initialized or has been released"));
        return;
      }

      await WebVoiceProcessor.unsubscribe(porcupineRef.current);
      setIsListening(false);
      setError(null);
    } catch (e: any) {
      setError(e);
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

  useEffect(() => () => {
    if (porcupineRef.current) {
      WebVoiceProcessor.unsubscribe(porcupineRef.current);
      porcupineRef.current.terminate();
      porcupineRef.current = null;
    }
  }, []);

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
};
