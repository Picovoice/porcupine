/*
  Copyright 2021-2022 Picovoice Inc.

  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
  file accompanying this source.

  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
  specific language governing permissions and limitations under the License.
*/

import { useState, useCallback } from 'react';

import { WebVoiceProcessor } from '@picovoice/web-voice-processor';

import {
  BuiltInKeyword,
  PorcupineDetection,
  PorcupineKeyword,
  PorcupineModel,
  PorcupineOptions,
  PorcupineWorker,
} from '@picovoice/porcupine-web';

export const usePorcupine = () => {
  const [porcupine, setPorcupine] = useState<PorcupineWorker | null>(null);
  const [isLoaded, setIsLoaded] = useState(false);
  const [isListening, setIsListening] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const init = useCallback(async (
    accessKey: string,
    keywords: Array<PorcupineKeyword | BuiltInKeyword> | PorcupineKeyword | BuiltInKeyword,
    keywordDetectionCallback: (porcupineDetection: PorcupineDetection) => void,
    model: PorcupineModel,
    options: PorcupineOptions = {},
  ): Promise<void> => {

  }, []);

  const start = useCallback(async (): Promise<void> => {
    try {
      if (!porcupine) {
      }

      await WebVoiceProcessor.subscribe(porcupine);
      setIsListening(true);
    } catch (e: any) {
      setError(e.toString());
      setIsListening(false);
    }
  }, [porcupine]);

  const stop = useCallback(async (): Promise<void> => {
    try {
      await WebVoiceProcessor.unsubscribe(porcupine);
      setIsListening(false);
    } catch (e: any) {
      setError(e.toString());
      setIsListening(false);
    }
  }, [porcupine]);

  const release = useCallback(async (): Promise<void> => {
  }, [porcupine]);

  return {
    isLoaded,
    isListening,
    error,
    start,
    stop,
  };
};

