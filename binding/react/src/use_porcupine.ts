/*
  Copyright 2021-2022 Picovoice Inc.

  You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
  file accompanying this source.

  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
  specific language governing permissions and limitations under the License.
*/

import { useState, useEffect } from 'react';

import { WebVoiceProcessor } from '@picovoice/web-voice-processor';

import {
  PorcupineKeyword,
  PorcupineWorker,
  PorcupineWorkerFactory,
} from '@picovoice/porcupine-web-core';

export type PorcupineHookArgs = {
  /** Immediately start the microphone upon initialization? */
  start: boolean;
  /** AccessKey obtained from Picovoice Console (https://picovoice.ai/console/) */
  accessKey: string;
  /** Keywords to listen for */
  keywords: Array<PorcupineKeyword | string> | PorcupineKeyword | string;
};

export function usePorcupine(
  /** The language-specific worker factory, imported as { PorcupineWorkerFactory }
   * from the @picovoice/porcupine-web-xx-worker series of packages, where xx is the two-letter language code. */
  porcupineWorkerFactory: PorcupineWorkerFactory | null,
  /** usePorcupine Hook Parameters. */
  porcupineHookArgs: PorcupineHookArgs | null,
  /** User-defined callback function invoked upon detection of the keywords. */
  keywordEventHandler: (label: string) => void
): {
  /** A state indicating whether the engine is initialized successfully */
  isLoaded: boolean;
  /** A state indicating whether the webVoiceProcessor is passing audio to the engine. */
  isListening: boolean;
  /** A state indicating whether the Hook returned an error. */
  isError: boolean | null;
  /** A string expression of the error. */
  errorMessage: string | null;
  /** A pointer to the internal webVoiceProcessor object. */
  webVoiceProcessor: WebVoiceProcessor | null;
  /** A method to start listening to the mic and processing the audio. */
  start: () => void;
  /** A method to stop listening to the mic. */
  stop: () => void;
  /** A method to stop processing the audio. */
  pause: () => void;
  /** Setter for keywordEventHandler */
  setKeywordEventHandler: React.Dispatch<
    React.SetStateAction<(label: string) => void>
  >;
} {
  const [isLoaded, setIsLoaded] = useState(false);
  const [isListening, setIsListening] = useState(false);
  const [
    webVoiceProcessor,
    setWebVoiceProcessor,
  ] = useState<WebVoiceProcessor | null>(null);
  const [isError, setIsError] = useState<boolean | null>(null);
  const [errorMessage, setErrorMessage] = useState<string | null>(null);
  const [keywordCallback, setKeywordCallback] = useState(
    () => keywordEventHandler
  );

  const start = (): boolean => {
    if (webVoiceProcessor !== null) {
      webVoiceProcessor.start().then(() => {
        setIsListening(true);
        return true;
      });
    }
    return false;
  };

  const stop = (): boolean => {
    if (webVoiceProcessor !== null) {
      webVoiceProcessor.stop().then(() => {
        setIsListening(false);
        return true;
      });
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

  const processErrorCallback = (error: string | Error): void => {
    setIsError(true);
    setErrorMessage(error.toString());
  };

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

    const {
      accessKey,
      keywords,
      start: startWebVp = true,
    // eslint-disable-next-line @typescript-eslint/no-non-null-assertion
    } = porcupineHookArgs!;

    async function startPorcupine(): Promise<{
      webVp: WebVoiceProcessor;
      ppnWorker: PorcupineWorker;
    }> {
      // eslint-disable-next-line @typescript-eslint/no-non-null-assertion
      const ppnWorker: PorcupineWorker = await porcupineWorkerFactory!.create(
        accessKey,
        keywords,
        keywordCallback,
        processErrorCallback
      );

      try {
        const webVp = await WebVoiceProcessor.init({
          engines: [ppnWorker],
          start: startWebVp,
        });

        return { webVp, ppnWorker };
      } catch (error) {
        ppnWorker.postMessage({ command: 'release' });
        throw error;
      }
    }

    const startPorcupinePromise = startPorcupine();

    startPorcupinePromise
      .then(({ webVp }) => {
        setIsLoaded(true);
        setIsListening(webVp.isRecording);
        setWebVoiceProcessor(webVp);
        setIsError(false);
      })
      .catch(error => {
        setIsError(true);
        setErrorMessage(error.toString());
      });

    return async (): Promise<void> => {
      startPorcupinePromise.then(({ webVp, ppnWorker }) => {
        if (webVp !== undefined && webVp !== null) {
          webVp.release();
        }
        if (ppnWorker !== undefined && ppnWorker !== null) {
          ppnWorker.postMessage({ command: 'release' });
          ppnWorker.terminate();
        }
      }).catch(() => {
        // do nothing
      });
    };
  }, [
    porcupineWorkerFactory,
    // https://github.com/facebook/react/issues/14476#issuecomment-471199055
    // ".... we know our data structure is relatively shallow, doesn't have cycles,
    // and is easily serializable ... doesn't have functions or weird objects like Dates.
    // ... it's acceptable to pass [JSON.stringify(variables)] as a dependency."
    JSON.stringify(porcupineHookArgs),
    keywordCallback,
  ]);

  return {
    isLoaded,
    isListening,
    isError,
    errorMessage,
    webVoiceProcessor,
    start,
    pause,
    stop,
    setKeywordEventHandler: setKeywordCallback,
  };
}
