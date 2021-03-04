import { useState, useEffect, useRef } from 'react';

import WebVoiceProcessor from '@picovoice/web-voice-processor';

type PorcupineFactoryArgs = {
  keywords: any; // TODO shared porcupine type
}

type PorcupineHookArgs = {
  /* Immediately start the microphone upon initialization */
  start: boolean,
  /* Arguments forwarded to PorcupineWorkerFactory */
  porcupineFactoryArgs: PorcupineFactoryArgs
}

export function usePorcupine(
  porcupineWorkerFactory: any, // TODO shared porcupine type
  porcupineArgs: PorcupineHookArgs,
  detectionCallback: (label: string) => void
): {
  isLoaded: boolean,
  isListening: boolean,
  isError: boolean,
  errorMessage: string
  start: () => void,
  pause: () => void,
  resume: () => void,
} {
  const [isLoaded, setIsLoaded] = useState(false);
  const [isListening, setIsListening] = useState(false);
  const [webVoiceProcessor, setWebVoiceProcessor] = useState(null);
  const [isError, setIsError] = useState(null);
  const [errorMessage, setErrorMessage] = useState(null);
  const callback = useRef(detectionCallback);

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

  useEffect(() => {
    async function startPorcupine():
      Promise<{ webVp: WebVoiceProcessor, ppnWorker: Worker }> {
      const { porcupineFactoryArgs, start: startOnInit } = porcupineArgs;

      const ppnWorker: Worker = await porcupineWorkerFactory.create(
        porcupineFactoryArgs
      );

      const webVp = await WebVoiceProcessor.init({
        engines: [ppnWorker],
        start: startOnInit,
      });

      ppnWorker.onmessage = (msg: MessageEvent): void => {
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
        if (webVp !== undefined) {
          webVp.release();
        }
        if (ppnWorker !== undefined) {
          ppnWorker.postMessage({ command: 'release' });
        }
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
