#! /bin/bash

echo "Removing old data ..."
rm -rf ./data

echo "Preparing dir ..."
mkdir -p ./data/lib/common
mkdir -p ./data/resources/keyword_files

echo "Copying Model File ..."
cp -r ../../lib/common/porcupine_params.pv ./data/lib/common/

for platform in beaglebone jetson linux mac raspberry-pi windows
do
    echo "Copying Resource & Library Files for $platform ..."
    cp -r ../../lib/$platform ./data/lib/
    cp -r ../../resources/keyword_files/$platform ./data/resources/keyword_files
done

echo "Copy complete!"
