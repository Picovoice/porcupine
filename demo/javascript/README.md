# Usage

Install dependencies by executing the following commands from [demo/javascript](/demo/javascript):

```bash
npm install
npm install -g copy-files-from-to
copy-files-from-to
```

Run this to launch the demo:

```bash
npx live-server --ignore="${PWD}/node_modules"
```

## Compatibility

[PorcupineManager](scripts/porcupine_manager.js) uses the
[Web Audio API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Audio_API) and
[WebAssembly](https://webassembly.org/), which are supported on all modern browsers (excluding Internet Explorer).
