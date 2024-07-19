#!/bin/bash

KEYWORDS=(alexa americano blueberry bumblebee computer grapefruit grasshopper "hey google"
 "hey siri" jarvis "ok google" picovoice porcupine terminator)

echo "Copying Porcupine model..."
cp ../../lib/common/porcupine_params.pv ./embedded/lib/common/porcupine_params.pv

echo "Copying Linux lib..."
cp ../../lib/linux/x86_64/libpv_porcupine.so ./embedded/lib/linux/x86_64/libpv_porcupine.so

echo "Copying macOS libs..."
cp ../../lib/mac/x86_64/libpv_porcupine.dylib ./embedded/lib/mac/x86_64/libpv_porcupine.dylib
cp ../../lib/mac/arm64/libpv_porcupine.dylib ./embedded/lib/mac/arm64/libpv_porcupine.dylib

echo "Copying Windows lib..."
cp ../../lib/windows/amd64/libpv_porcupine.dll ./embedded/lib/windows/amd64/libpv_porcupine.dll

echo "Copying RPi libs..."
cp -rp ../../lib/raspberry-pi/* ./embedded/lib/raspberry-pi

echo "Copying keyword files"
for keyword in "${KEYWORDS[@]}"; do
    cp ../../resources/keyword_files/windows/"$keyword"_windows.ppn ./embedded/resources/keyword_files/windows/
    cp ../../resources/keyword_files/mac/"$keyword"_mac.ppn ./embedded/resources/keyword_files/mac/
    cp ../../resources/keyword_files/linux/"$keyword"_linux.ppn ./embedded/resources/keyword_files/linux/
    cp ../../resources/keyword_files/raspberry-pi/"$keyword"_raspberry-pi.ppn ./embedded/resources/keyword_files/raspberry-pi/
done

echo "Copy complete!"
