/*
    Copyright 2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

import { useState, useEffect } from 'react';

import { WebVoiceProcessor } from '@picovoice/web-voice-processor';

import {
  PorcupineHookArgs,
  PorcupineWorker,
  PorcupineWorkerFactory,
  PorcupineWorkerResponse,
} from './porcupine_types';

export function usePorcupine(
  porcupineWorkerFactory: PorcupineWorkerFactory | null,
  porcupineHookArgs: PorcupineHookArgs | null,
  detectionCallback: (label: string) => void
): {
  isLoaded: boolean;
  isListening: boolean;
  isError: boolean | null;
  errorMessage: string | null;
  webVoiceProcessor: WebVoiceProcessor | null;
  start: () => void;
  pause: () => void;
  resume: () => void;
} {
  const [isLoaded, setIsLoaded] = useState(false);
  const [isListening, setIsListening] = useState(false);
  const [
    webVoiceProcessor,
    setWebVoiceProcessor,
  ] = useState<WebVoiceProcessor | null>(null);
  const [
    porcupineWorker,
    setPorcupineWorker,
  ] = useState<PorcupineWorker | null>(null);
  const [isError, setIsError] = useState<boolean | null>(null);
  const [errorMessage, setErrorMessage] = useState<string | null>(null);

  const start = (): boolean => {
    if (webVoiceProcessor !== null) {
      webVoiceProcessor.start();
      setIsListening(true);
      return true;
    }
    return false;
  };

  const pause = (): boolean => {
    if (webVoiceProcessor !== null) {
      webVoiceProcessor.pause();
      setIsListening(false);
      return true;
    }
    return false;
  };

  const resume = (): boolean => {
    if (webVoiceProcessor !== null) {
      webVoiceProcessor.resume();
      setIsListening(true);
      return true;
    }
    return false;
  };

  // Refresh the detection callback when it changes (avoid stale closure)
  useEffect(() => {
    if (porcupineWorker !== null) {
      porcupineWorker.onmessage = (
        msg: MessageEvent<PorcupineWorkerResponse>
      ): void => {
        switch (msg.data.command) {
          case 'ppn-keyword':
            detectionCallback(msg.data.keywordLabel);
            break;
          default:
            break;
        }
      };
    }
  }, [detectionCallback]);

  useEffect(() => {
    // If using dynamic import() on porcupine-web-xx-worker,
    // initially the worker factory may not exist yet; do nothing
    if (
      porcupineWorkerFactory === null ||
      porcupineWorkerFactory === undefined
    ) {
      return (): void => {
        /* NOOP */
      };
    }

    // When these are null, don't start Porcupine.
    // If they previously were non-null, the useEffect cleanup will run and terminate the extant worker/webvp.
    if (porcupineHookArgs === null || porcupineHookArgs === undefined) {
      return (): void => {
        /* NOOP */
      };
    }

    async function startPorcupine(): Promise<{
      webVp: WebVoiceProcessor;
      ppnWorker: PorcupineWorker;
    }> {
      // eslint-disable-next-line @typescript-eslint/no-non-null-assertion
      const { keywords, start: startWebVp = true } = porcupineHookArgs!;

      // eslint-disable-next-line @typescript-eslint/no-non-null-assertion
      const ppnWorker: PorcupineWorker = await porcupineWorkerFactory!.create(
        keywords
      );

      const webVp = await WebVoiceProcessor.init({
        engines: [ppnWorker],
        start: startWebVp,
      });

      ppnWorker.onmessage = (
        msg: MessageEvent<PorcupineWorkerResponse>
      ): void => {
        switch (msg.data.command) {
          case 'ppn-keyword':
            detectionCallback(msg.data.keywordLabel);
            break;
          default:
            break;
        }
      };

      return { webVp, ppnWorker };
    }

    const startPorcupinePromise = startPorcupine();

    startPorcupinePromise
      .then(({ webVp, ppnWorker }) => {
        setIsLoaded(true);
        setIsListening(webVp.isRecording);
        setWebVoiceProcessor(webVp);
        setPorcupineWorker(ppnWorker);
        setIsError(false);
      })
      .catch(error => {
        setIsError(true);
        setErrorMessage(error.toString());
      });

    return async (): Promise<void> => {
      const { webVp, ppnWorker } = await startPorcupinePromise;
      webVp.release();
      ppnWorker.postMessage({ command: 'release' });
    };
  }, [
    porcupineWorkerFactory,
    // https://github.com/facebook/react/issues/14476#issuecomment-471199055
    // ".... we know our data structure is relatively shallow, doesn't have cycles,
    // and is easily serializable ... doesn't have functions or weird objects like Dates.
    // ... it's acceptable to pass [JSON.stringify(variables)] as a dependency."
    JSON.stringify(porcupineHookArgs),
  ]);

  return {
    isLoaded,
    isListening,
    isError,
    errorMessage,
    webVoiceProcessor,
    start,
    pause,
    resume,
  };
}
