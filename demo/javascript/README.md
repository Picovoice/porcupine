# Prerequisites

Install dependencies by executing the following command from [demo/js/](/demo/js):

```bash
npm install -g copy-files-from-to
npm install
```

Run this to launch the demo:

```bash
npx live-server --ignore="${PWD}/node_modules"
```

## Compatibility

PorcupineManager uses the [Web Audio API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Audio_API) and
[WebAssembly](https://webassembly.org/), which are supported on all modern browsers (excluding Internet Explorer).

