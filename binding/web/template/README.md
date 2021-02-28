# porcupine-web-en

The Picovoice Porcupine library for web browsers, powered by WebAssembly. Intended (but not required) to be used with the [@picovoice/web-voice-processor](https://www.npmjs.com/package/@picovoice/web-voice-processor).

This library processes always-listening voice commands in-browser, offline. All processing is done via WebAssembly and Workers in a separate thread.

Looking for Porcupine on NodeJS? See the [@picovoice/porcupine-node](https://www.npmjs.com/package/@picovoice/porcupine-node) package.

## Compatibility

- Chrome / Edge
- Firefox
- Safari

This library requires several modern browser features: WebAssembly, Web Workers, and promises. Internet Explorer will _not_ work.

If you are using this library with the [@picovoice/web-voice-processor](https://www.npmjs.com/package/@picovoice/web-voice-processor) to access the microphone, it requires some additional browser features like Web Audio API. Its overall browser support is approximately the same.

## Installation

Using `yarn` or `npm`:

```bash
yarn add @picovoice/porcupine-web-en
```

(or)

```bash
npm install @picovoice/porcupine-web-en
```

## Build from source (IIFE + ESM outputs)

This library uses Rollup and TypeScript along with Babel and other popular rollup plugins. There are two outputs: an IIFE version intended for script tags / CDN usage, and an ESM version intended for use with modern JavaScript/TypeScript development (e.g. Create React App, Webpack).

```
yarn
yarn build
```

The output will appear in the ./dist/ folder.
