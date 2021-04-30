# porcupine-web-react

React hook for Porcupine for Web.

## Porcupine

Porcupine is a highly accurate and lightweight wake word engine. It enables building always-listening voice-enabled
applications.

> Porcupine wake word models for all major voice assistants (`Alexa`, `Hey Google`, `Ok Google`, and `Hey Siri`) are
> available for free (under Apache 2.0)

Porcupine is also available for React Native, as a separate package. See [@picovoice/porcupine-react-native](https://www.npmjs.com/package/@picovoice/porcupine-react-native).

## Introduction

The Porcupine SDK for React is based on the Porcupine SDK for Web. The library provides a React hook: `usePorcupine`. The hook will take care of microphone access and audio downsampling (via `@picovoice/web-voice-processor`) and provide a wake word detection event to which your application can subscribe.

## Compatibility

The Picovoice SDKs for Web are powered by WebAssembly (WASM), the Web Audio API, and Web Workers.

All modern browsers (Chrome/Edge/Opera, Firefox, Safari) are supported, including on mobile. Internet Explorer is _not_ supported.

Using the Web Audio API requires a secure context (HTTPS connection), with the exception of `localhost`, for local development.

## Installation

Use `npm` or `yarn` to install the package and its peer dependencies. Each spoken language (e.g. 'en', 'de') is a separate package. For this example we'll use English:

```console
yarn add @picovoice/porcupine-web-react @picovoice/porcupine-web-en-worker @picovoice/web-voice-processor`
```

(or)

```console
npm install @picovoice/porcupine-web-react @picovoice/porcupine-web-en-worker @picovoice/web-voice-processor`
```

## Usage

The `usePorcupine` hook provides a collection of fields and methods shown below. You can pass the `keywordEventHandler` to respond to Porcupine detection events. This example uses the builtin keyword "Picovoice" with a sensitivity of 0.65.

Make sure you handle the possibility of errors with the `isError` and `errorMessage` fields. Users may not have a working microphone, and they can always decline (and revoke) permissions; your application code should anticipate these scenarios.

### Static Import

Using static imports for the `@picovoice/porcupine-web-xx-worker` packages is straightforward, but will impact your initial bundle size with an additional ~2MB. Depending on your requirements, this may or may not be feasible. If you require a small bundle size, see dynamic importing below.

```javascript
import React, { useState } from 'react';
import { PorcupineWorkerFactory } from '@picovoice/porcupine-web-en-worker';
import { usePorcupine } from '@picovoice/porcupine-web-react';

const keywords = [{ builtin: 'Picovoice', sensitivity: 0.65 }];

function VoiceWidget(props) {
  const keywordEventHandler = keywordLabel => {
    console.log(`Porcupine detected ${keywordLabel}`);
  };

  const {
    isLoaded,
    isListening,
    isError,
    errorMessage,
    start,
    resume,
    pause,
  } = usePorcupine(
    PorcupineWorkerFactory,
    { keywords: keywords, start: true },
    keywordEventHandler
  );
}
```

The `keywordEventHandler` will log the keyword detections to the browser's JavaScript console.

**Important Note**: Internally, `usePorcupine` performs work asynchronously to initialize, as well as asking for microphone permissions. Not until the asynchronous tasks are done and permission given will Porcupine actually be running. Therefore, it makes sense to use the `isLoaded` state to update your UI to let users know your application is actually ready to process voice (and `isError` in case something went wrong). Otherwise, they may start speaking and their audio data will not be processed, leading to a poor/inconsistent experience.

### Dynamic Import / Code Splitting

If you are shipping Porcupine for Web and wish to avoid adding its ~2MB to your application's initial bundle, you can use dynamic imports. These will split off the porcupine-web-xx-worker packages into separate bundles and load them asynchronously. This means we need additional logic.

We add a `useEffect` hook to kick off the dynamic import. We store the result of the dynamically loaded worker chunk into a `useState` hook. When `usePorcupine` receives a non-null/undefined value for the worker factory, it will start up Porcupine.

See the [Webpack docs](https://webpack.js.org/guides/code-splitting/) for more information about Code Splitting.

```javascript
import { useEffect, useState } from 'react';
import { usePorcupine } from '@picovoice/porcupine-web-react';

export default function VoiceWidget() {
  const [keywordDetections, setKeywordDetections] = useState([]);
  const [workerChunk, setWorkerChunk] = useState({ workerFactory: null });
  const [keywords] = useState([
    { builtin: 'Alexa', sensitivity: 0.7 },
    'Picovoice',
  ]);

  useEffect(() => {
    async function loadPorcupineWorkerChunk() {
      const ppnWorkerFactory = (
        await import('@picovoice/porcupine-web-en-worker')
      ).PorcupineWorkerFactory; // <-- Dynamically import the worker
      console.log('Porcupine worker chunk is loaded.');
      return ppnWorkerFactory;
    }

    if (workerChunk.factory === null) {
      // <-- We only want to load once!
      loadPorcupineWorkerChunk().then(ppnWorkerFactory => {
        setWorkerChunk({ workerFactory: ppnWorkerFactory });
      });
    }
  }, [workerChunk]);

  const keywordEventHandler = porcupineKeywordLabel => {
    setKeywordDetections(x => [...x, porcupineKeywordLabel]);
  };

  const {
    isLoaded,
    isListening,
    isError,
    errorMessage,
    start,
    resume,
    pause,
  } = usePorcupine(
    workerChunk.workerFactory, // <-- When this is null/undefined, it's ignored. Otherwise, usePorcupine will start.
    { keywords: keywords },
    keywordEventHandler
  );
}
```
