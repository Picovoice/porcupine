    # Prerequisites

Install the latest version of [Android Studio](https://developer.android.com/studio/index.html).

# Building and Running Tests

Using Android Studio open [binding/android](/binding/android) as an Android project and build it. The build creates an 
[Android Archive](https://developer.android.com/studio/projects/android-library.html) (AAR) at
`binding/android/porcupine/build/outputs/aar` that can be used as a dependency of your Android app module.


Using Android Studio navigate to [androidTest](/binding/android/porcupine/src/androidTest/java/ai/picovoice/porcupine),
right click, and select `Run Tests in ai.picovoice.porcupine` from drop down menu.

## Note for Building on Windows

This repository is using [symlinks](https://en.wikipedia.org/wiki/Symbolic_link). On Windows symlinks do not work as
expected. You need to copy-paste the resources pointed to by all symlinks manually. For Android binding JNI shared objects
are symlinks. You need to copy the content under [lib/android](/lib/android) and paste it under
[binding/android/porcupine/src/main/jniLibs](/binding/android/porcupine/src/main/jniLibs).

# Binding Class
 
[Porcupine](/binding/android/porcupine/src/main/java/ai/picovoice/porcupine/Porcupine.java) provides a binding for android
using [JNI](https://en.wikipedia.org/wiki/Java_Native_Interface). For an example usage please refer to
[Android demo application](/demo/android).
