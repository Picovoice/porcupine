# Porcupine ReSpeaker Demo

Made in Vancouver, Canada by [Picovoice](https://picovoice.ai)

This package contains a commandline demo for controlling ReSpeaker 4-mic microphone array LEDs using Porcupine.

## Porcupine

Porcupine is a highly-accurate and lightweight wake word engine. It enables building always-listening voice-enabled
applications. It is

- using deep neural networks trained in real-world environments.
- compact and computationally-efficient. It is perfect for IoT.
- cross-platform. Raspberry Pi, BeagleBone, Android, iOS, Linux (x86_64), macOS (x86_64), Windows (x86_64), and web
  browsers are supported. Additionally, enterprise customers have access to the ARM Cortex-M SDK.
- scalable. It can detect multiple always-listening voice commands with no added runtime footprint.
- self-service. Developers can train custom wake word models using [Picovoice Console](https://picovoice.ai/console/).

## Installation

Follow the instructions on [Seeed Studio](https://wiki.seeedstudio.com/ReSpeaker_4_Mic_Array_for_Raspberry_Pi/)
to install and set up the microphone array.

Then install the demo:

```bash
sudo pip3 install ppnrespeakerdemo
```

## Usage

Below are the colors associated with supported wake words for this demo:

- ![#ffff33](https://via.placeholder.com/15/ffff33/000000?text=+) `Alexa`
- ![#ffffff](https://via.placeholder.com/15/ffffff/000000?text=+) `Computer`
- ![#ff0000](https://via.placeholder.com/15/ff0000/000000?text=+) `Hey Google`
- ![#800080](https://via.placeholder.com/15/800080/000000?text=+) `Hey Siri`
- ![#ff3399](https://via.placeholder.com/15/ff3399/000000?text=+) `Jarvis`
- ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) `Picovoice`
- ![#0000ff](https://via.placeholder.com/15/0000ff/000000?text=+) `Porcupine`
- ![#ff8000](https://via.placeholder.com/15/ff8000/000000?text=+) `Bumblebee`
- ![#000000](https://via.placeholder.com/15/000000/000000?text=+) `Terminator`

Run the demo:

```bash
porcupine_respeaker_demo
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
