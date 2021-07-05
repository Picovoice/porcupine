#!/usr/bin/env bash

cd ..
set -e
set -x

cd ..

git clone -b stable https://github.com/flutter/flutter.git
export PATH=`pwd`/flutter/bin:$PATH

flutter channel stable
flutter doctor
flutter build apk --debug

mkdir -p android/app/build/outputs/apk/
mv build/app/outputs/apk/debug/app-debug.apk android/app/build/outputs/apk/