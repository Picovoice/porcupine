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

`yarn add @picovoice/porcupine-web-react @picovoice/porcupine-web-en-worker`

(or)

`npm install @picovoice/porcupine-web-react @picovoice/porcupine-web-en-worker`

## Usage

The `usePorcupine` hook provides a collection of fields and methods shown below. You can pass the `keywordEventHandler` to respond to Porcupine detection events. This example uses the builtin keyword "Picovoice" with a sensitivity of 0.65.

Make sure you handle the possibility of errors with the `isError` and `errorMessage` fields. Users may not have a working microphone, and they can always decline (and revoke) permissions; your application code should anticipate these scenarios.

```javascript
import React, { useState } from 'react';
import { PorcupineWorkerFactory } from '@picovoice/porcupine-web-en-worker';
import { usePorcupine } from '@picovoice/porcupine-web-react';

const porcupineFactoryArgs = [{ builtin: 'Picovoice', sensitivity: 0.65 }];

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
    { porcupineFactoryArgs: porcupineFactoryArgs, start: true },
    keywordEventHandler
  );
}
```

The `keywordEventHandler` will log the keyword detections to the browser's JavaScript console.

**Important Note**: Internally, `usePorcupine` performs work asynchronously to initialize, as well as asking for microphone permissions. Not until the asynchronous tasks are done and permission given will Porcupine actually be running. Therefore, it makes sense to use the `isLoaded` state to update your UI to let users know your application is actually ready to process voice (and `isError` in case something went wrong). Otherwise, they may start speaking and their audio data will not be processed, leading to a poor/inconsistent experience.
