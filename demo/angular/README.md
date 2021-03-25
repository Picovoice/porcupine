# porcupine-web-angular-demo

This demo application includes a sample `VoiceWidget` Angular component which uses the `PorcupineService` Angular service to allow listening for keywords. Porcupine keyword detections are handled via the `keyword$` event. Our VoiceWidget subscribes to this event and displays the results.

The demo uses dynamic imports to split the PorcupineService away from the main application bundle. This means that the initial download size of the Angular app will not be impacted by the ~1-2MB requirement of Porcupine. While small for all-in-one offline Voice AI, the size is large for an intial web app load.

If you decline microphone permission in the browser, or another such issue prevents Porcupine from starting, the error will be displayed.

The widget shows the various loading and error events, as well as mounting/unmounting the `VoiceWidget` with a toggle, demonstrating the complete lifecycle of Porcupine with in an Angular app.

This project was generated with [Angular CLI](https://github.com/angular/angular-cli) version 11.0.5.

## Development server

Run `ng serve` for a dev server. Navigate to `http://localhost:4200/`. The app will automatically reload if you change any of the source files.
