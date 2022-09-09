## Requirements

```console
sudo apt-get install libasound2-dev
```

## AccessKey

Porcupine requires a valid Picovoice `AccessKey` at initialization. `AccessKey` acts as your credentials when using Porcupine SDKs.
You can get your `AccessKey` for free. Make sure to keep your `AccessKey` secret.
Signup or Login to [Picovoice Console](https://console.picovoice.ai/) to get your `AccessKey`.

## Build

From the root of the repository run:

```console
gcc -std=c99 -O3 -o demo/respeaker-rpi0/porcupine_demo_mic \
-I include/ demo/respeaker-rpi0/porcupine_demo_mic.c \
-ldl -lasound
```

## Run

Below are the colors associated with supported wake words for this demo:

<!-- markdown-link-check-disable -->
- ![#ffff33](../../resources/images/ffff33.png) `Alexa`
- ![#ff8000](../../resources/images/ff8000.png) `Bumblebee`
- ![#ffffff](../../resources/images/ffffff.png) `Computer`
- ![#ff0000](../../resources/images/ff0000.png) `Hey Google`
- ![#800080](../../resources/images/800080.png) `Hey Siri`
- ![#ff3399](../../resources/images/ff3399.png) `Jarvis`
- ![#00ff00](../../resources/images/00ff00.png) `Picovoice`
- ![#0000ff](../../resources/images/0000ff.png) `Porcupine`
- ![#000000](../../resources/images/000000.png) `Terminator`
<!-- markdown-link-check-enable -->

From the root of the repository run:

```console
./demo/respeaker-rpi0/porcupine_demo_mic \
${ACCESS_KEY} \
lib/raspberry-pi/arm11/libpv_porcupine.so \
lib/common/porcupine_params.pv \
0.65 \
plughw:CARD=seeed2micvoicec,DEV=0 \
resources/keyword_files/raspberry-pi/alexa_raspberry-pi.ppn \
resources/keyword_files/raspberry-pi/computer_raspberry-pi.ppn \
resources/keyword_files/raspberry-pi/hey\ google_raspberry-pi.ppn \
resources/keyword_files/raspberry-pi/hey\ siri_raspberry-pi.ppn \
resources/keyword_files/raspberry-pi/jarvis_raspberry-pi.ppn \
resources/keyword_files/raspberry-pi/picovoice_raspberry-pi.ppn \
resources/keyword_files/raspberry-pi/porcupine_raspberry-pi.ppn \
resources/keyword_files/raspberry-pi/bumblebee_raspberry-pi.ppn \
resources/keyword_files/raspberry-pi/terminator_raspberry-pi.ppn
```

Wait for the demo to initialize and print `[Listening]` in the terminal. Say:

> Picovoice

The demo outputs:

```text
detected 'Picovoice'
```

The lights are now set to `green`. Say:

> Alexa

The lights are set to `yellow` now. Say:

> Terminator

to turn off the lights.

