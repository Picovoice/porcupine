# Porcupine Wake Word Engine Demos

Made in Vancouver, Canada by [Picovoice](https://picovoice.ai)

This package contains .NET Core command line demos for processing real-time audio (i.e. microphone) and audio files
using Porcupine wake word engine.

## Porcupine

Porcupine is a highly-accurate and lightweight wake word engine. It enables building always-listening voice-enabled
applications. It is

- using deep neural networks trained in real-world environments.
- compact and computationally-efficient making it perfect for IoT.
- scalable. It can detect multiple always-listening voice commands with no added CPU/memory footprint.
- self-service. Developers can train custom wake phrases using [Picovoice Console](https://picovoice.ai/console/).

## Compatibility

- TDB
- Runs on Linux (x86_64), MacOS (x86_64) and Windows (x86_64)

javac -d build -cp "junit-platform-console-standalone-1.7.0.jar" "src\ai\picovoice\porcupine\*.java"
java -ea "-Djava.library.path=.\jniLibs\"" -jar "junit-platform-console-standalone-1.7.0.jar" -cp build -c ai.picovoice.porcupine.PorcupineTest
C:\Users\ian\.jdks\openjdk-15-1\bin\java.exe "-javaagent:C:\Program Files\JetBrains\IntelliJ IDEA Community Edition 2020.2.2\lib\idea_rt.jar=56176:C:\Program Files\JetBrains\IntelliJ IDEA Community Edition 2020.2.2\bin" -Dfile.encoding=UTF-8 -classpath C:\Users\ian\work\github\porcupine\demo\java\out\production\porcupinedemo;C:\Users\ian\work\github\porcupine\binding\java\out\artifacts\Porcupine_jar\Porcupine.jar;C:\Users\ian\work\github\porcupine\demo\java\lib\commons-cli-1.4.jar;C:\Users\ian\work\github\porcupine\demo\java\lib\Porcupine.jar ai.picovoice.porcupinedemo.MicDemo -keywords picovoice