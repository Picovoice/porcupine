#!/bin/sh

mkdir -p "${SRCROOT}/keywords/"
mkdir -p "${SRCROOT}/models/"

rm "${SRCROOT}/ForegroundApp/keywords/"*
rm "${SRCROOT}/ForegroundApp/models/"*

if [ $1 != 'en' ];
then
    cp "${SRCROOT}/../../../resources/keyword_files_$1/ios/"* "${SRCROOT}/keywords/"
    cp "${SRCROOT}/../../../lib/common/porcupine_params_$1.pv" "${SRCROOT}/models/"
fi
