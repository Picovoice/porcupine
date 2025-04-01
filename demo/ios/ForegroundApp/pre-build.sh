#!/bin/sh

mkdir -p "${SRCROOT}/PorcupineForegroundAppDemo/keywords/"
mkdir -p "${SRCROOT}/PorcupineForegroundAppDemo/models/"

rm "${SRCROOT}/PorcupineForegroundAppDemo/keywords/"*
rm "${SRCROOT}/PorcupineForegroundAppDemo/models/"*

if [ $1 != 'en' ];
then
    cp "${SRCROOT}/../../../resources/keyword_files_$1/ios/"* "${SRCROOT}/PorcupineForegroundAppDemo/keywords/"
    cp "${SRCROOT}/../../../lib/common/porcupine_params_$1.pv" "${SRCROOT}/PorcupineForegroundAppDemo/models/"
fi
