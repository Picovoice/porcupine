# Porcupine

Provides a low-level API to the engine. It can be used to incorporate Porcupine into already existing audio processing
pipelines.

# PorcupineManager

Provides a high-level API that takes care of audio recording as well. For example usage refer to
[Activity demo](/demo/android/Activity) or [Service demo](/demo/android/Service).

## Resouce Usage 

- CPU <= 1%
- Battery Usage <= LOW
- Memory <= 128 MB

The following profile graph was captured running the [Porcupine Activity demo](/demo/android/Activity/) on a Google Pixel 3:

![](/binding/android/android_profiling.gif)
