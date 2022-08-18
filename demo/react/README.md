# porcupine-web-react-demo

This demo application includes the `VoiceWidget` which uses the `usePorcupine` react hook to allow listening for keywords. The keywords are handled via the `keywordEventHandler` callback function that updates a React `useState` hook, and then renders the results.

If you decline microphone permission in the browser, or another such issue prevents Porcupine from starting, the error will be displayed.

The widget shows the various loading and error states, as well as mounting/unmounting the `VoiceWidget` with a toggle, demonstrating the complete lifecycle of Porcupine with in a React app.

This project was bootstrapped with [Create React App](https://github.com/facebook/create-react-app).

## Install and Run

```console
yarn
yarn start
```

(or)

```console
npm install
npm run start
```

Open `http://localhost:3000` to view it in the browser.

The page will reload if you make edits.\
You will also see any lint errors in the console.

## Try Porcupine

The demo is listening for the built-in wake words "Picovoice" and "Alexa". Say:

> "Picovoice"

or

> "Alexa"

The detections will appear on the page.
