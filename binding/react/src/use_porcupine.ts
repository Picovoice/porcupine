/*
  Copyright 2021-2022 Picovoice Inc.

  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
  file accompanying this source.

  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
  specific language governing permissions and limitations under the License.
*/

import { useState, useRef } from 'react';

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
  const webVpRef = useRef(WebVoiceProcessor.instance());
  const ppnRef = useRef<PorcupineWorker | null>(null);

  const [isListening, setIsListening] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const keywordCallbackInner = useRef(keywordDetectionCallback);

  const start = async (): Promise<void> => {
    try {
      if (!ppnRef.current) {
        const handle: PorcupineWorker = await PorcupineWorker.create(
          accessKey,
          keywords,
          keywordCallbackInner.current,
          model,
          options
        );

        webVpRef.current.subscribe(handle);
        ppnRef.current = handle;
      }

      await webVpRef.current.start();
      setIsListening(true);
    } catch (e: any) {
      setError(e.toString());
      setIsListening(false);
    }
  };

  const stop = async (): Promise<void> => {
    try {
      if (ppnRef.current) {
        webVpRef.current.unsubscribe(ppnRef.current);
        ppnRef.current.terminate();
        ppnRef.current = null;
      }

      await webVpRef.current.stop();
      setIsListening(false);
    } catch (e: any) {
      setError(e.toString());
      setIsListening(false);
    }
  };

  const pause = async (): Promise<void> => {
    await webVpRef.current.pause();
    setIsListening(false);
  };

  return {
    isListening,
    error,
    start,
    stop,
    pause,
  };
};

