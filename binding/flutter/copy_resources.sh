echo "Creating Android lib directory..."
if [ ! -d "./android/src/main/jniLibs" ]
then 
    mkdir -p ./android/src/main/jniLibs
fi

echo "Copying Android libs..."
cp -rp ../../lib/android/* ./android/src/main/jniLibs

echo "Copying iOS libs..."
cp -R ../../lib/ios/PvPorcupine.xcframework ./ios

echo "Creating model resources directory..."
if [ ! -d "./assets/lib/common" ]
then 
    mkdir -p ./assets/lib/common
fi

echo "Copying default model file..."
cp ../../lib/common/porcupine_params.pv ./assets/lib/common/porcupine_params.pv

echo "Creating Android keyword resources directory..."
if [ ! -d "./assets/resources/keyword_files/android" ]
then 
    mkdir -p ./assets/resources/keyword_files/android
fi

echo "Copying Android keyword files..."
cp -rp ../../resources/keyword_files/android/* ./assets/resources/keyword_files/android

echo "Creating iOS keyword resources directory..."
if [ ! -d "./assets/resources/keyword_files/ios" ]
then 
    mkdir -p ./assets/resources/keyword_files/ios
fi

echo "Copying iOS keyword files..."
cp -rp ../../resources/keyword_files/ios/* ./assets/resources/keyword_files/ios

echo "Copying license file..."
cp ../../LICENSE ./LICENSE
