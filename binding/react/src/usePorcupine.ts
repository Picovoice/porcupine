/*
    Copyright 2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

import { useState, useEffect, useRef } from 'react';

import WebVoiceProcessor from '@picovoice/web-voice-processor';

import {
  PorcupineHookArgs,
  PorcupineWorker,
  PorcupineWorkerFactory,
  PorcupineWorkerResponse,
} from './porcupine_types';

export function usePorcupine(
  porcupineWorkerFactory: PorcupineWorkerFactory,
  porcupineArgs: PorcupineHookArgs,
  detectionCallback: (label: string) => void
): {
  isLoaded: boolean,
  isListening: boolean,
  isError: boolean | null,
  errorMessage: string | null,
  start: () => void,
  pause: () => void,
  resume: () => void,
} {
  const [isLoaded, setIsLoaded] = useState(false);
  const [isListening, setIsListening] = useState(false);
  const [webVoiceProcessor, setWebVoiceProcessor] = useState<WebVoiceProcessor>();
  const [isError, setIsError] = useState<boolean | null>(null);
  const [errorMessage, setErrorMessage] = useState<string | null>(null);
  const callback = useRef(detectionCallback);

  const start = (): boolean => {
    if (webVoiceProcessor !== undefined) {
      webVoiceProcessor.start();
      setIsListening(true);
      return true;
    }
    return false;
  };

  const pause = (): boolean => {
    if (webVoiceProcessor !== undefined) {
      webVoiceProcessor.pause();
      setIsListening(false);
      return true;
    }
    return false;
  };

  const resume = (): boolean => {
    if (webVoiceProcessor !== undefined) {
      webVoiceProcessor.resume();
      setIsListening(true);
      return true;
    }
    return false;
  };

  useEffect(() => {
    async function startPorcupine():
      Promise<{ webVp: WebVoiceProcessor, ppnWorker: PorcupineWorker }> {
      const { porcupineFactoryArgs, start: startOnInit } = porcupineArgs;

      const ppnWorker: Worker = await porcupineWorkerFactory.create(
        porcupineFactoryArgs
      );

      const webVp = await WebVoiceProcessor.init({
        engines: [ppnWorker],
        start: startOnInit,
      });

      ppnWorker.onmessage = (msg: MessageEvent<PorcupineWorkerResponse>): void => {
        switch (msg.data.command) {
          case 'ppn-keyword':
            callback.current(msg.data.keywordLabel);
            break;
          default:
            break;
        }
      };

      return { webVp, ppnWorker };
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

    return (): void => {
      startPorcupinePromise.then(({ webVp, ppnWorker }) => {
        webVp.release();
        ppnWorker.postMessage({ command: 'release' });
      });
    };
  }, [
    porcupineWorkerFactory,
    // https://github.com/facebook/react/issues/14476#issuecomment-471199055
    // ".... we know our data structure is relatively shallow, doesn't have cycles,
    // and is easily serializable ... doesn't have functions or weird objects like Dates.
    // ... it's acceptable to pass [JSON.stringify(variables)] as a dependency."
    JSON.stringify(porcupineArgs),
  ]);

  return {
    isLoaded,
    isListening,
    isError,
    errorMessage,
    start,
    pause,
    resume,
  };
}
