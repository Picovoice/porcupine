/*
  Copyright 2021-2022 Picovoice Inc.

  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
  file accompanying this source.

  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
  specific language governing permissions and limitations under the License.
*/

import { useState, useRef, useCallback, useEffect } from 'react';

import { WebVoiceProcessor } from '@picovoice/web-voice-processor';

import {
  BuiltInKeyword,
  PorcupineDetection,
  PorcupineKeyword,
  PorcupineModel,
  PorcupineOptions,
  PorcupineWorker,
} from '@picovoice/porcupine-web';

export const usePorcupine = (
  accessKey: string,
  keywords: Array<PorcupineKeyword | BuiltInKeyword> | PorcupineKeyword | BuiltInKeyword,
  keywordDetectionCallback: (porcupineDetection: PorcupineDetection) => void,
  model: PorcupineModel,
  options: PorcupineOptions = {},
) => {
  const ppnRef = useRef<PorcupineWorker | null>(null);

  const [isListening, setIsListening] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const keywordDetectionInner = useRef(keywordDetectionCallback);

  const keywordDetectionWrapper = useRef((porcupineDetection: PorcupineDetection) => {
    keywordDetectionInner.current(porcupineDetection);
  });

  useEffect(() => {
    keywordDetectionInner.current = keywordDetectionCallback;
  }, [keywordDetectionCallback]);

  const start = useCallback(async (): Promise<void> => {
    try {
      if (!ppnRef.current) {
        const handle: PorcupineWorker = await PorcupineWorker.create(
          accessKey,
          keywords,
          keywordDetectionWrapper.current,
          model,
          options
        );

        await WebVoiceProcessor.subscribe(handle);
        ppnRef.current = handle;
      }

      setIsListening(true);
    } catch (e: any) {
      setError(e.toString());
      setIsListening(false);
    }
  }, [accessKey, keywords, model, options]);

  const stop = useCallback(async (): Promise<void> => {
    try {
      if (ppnRef.current) {
        await WebVoiceProcessor.unsubscribe(ppnRef.current);
        ppnRef.current.terminate();
        ppnRef.current = null;
      }

      setIsListening(false);
    } catch (e: any) {
      setError(e.toString());
      setIsListening(false);
    }
  }, []);

  return {
    isListening,
    error,
    start,
    stop,
  };
};

