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
cp ../../resources/keyword_files/android/alexa_android.ppn ./assets/resources/keyword_files/android/alexa_android.ppn
cp ../../resources/keyword_files/android/americano_android.ppn ./assets/resources/keyword_files/android/americano_android.ppn
cp ../../resources/keyword_files/android/blueberry_android.ppn ./assets/resources/keyword_files/android/blueberry_android.ppn
cp ../../resources/keyword_files/android/bumblebee_android.ppn ./assets/resources/keyword_files/android/bumblebee_android.ppn
cp ../../resources/keyword_files/android/computer_android.ppn ./assets/resources/keyword_files/android/computer_android.ppn
cp ../../resources/keyword_files/android/grapefruit_android.ppn ./assets/resources/keyword_files/android/grapefruit_android.ppn
cp ../../resources/keyword_files/android/grasshopper_android.ppn ./assets/resources/keyword_files/android/grasshopper_android.ppn
cp ../../resources/keyword_files/android/hey\ google_android.ppn ./assets/resources/keyword_files/android/hey\ google_android.ppn
cp ../../resources/keyword_files/android/hey\ siri_android.ppn ./assets/resources/keyword_files/android/hey\ siri_android.ppn
cp ../../resources/keyword_files/android/jarvis_android.ppn ./assets/resources/keyword_files/android/jarvis_android.ppn
cp ../../resources/keyword_files/android/ok\ google_android.ppn ./assets/resources/keyword_files/android/ok\ google_android.ppn
cp ../../resources/keyword_files/android/picovoice_android.ppn ./assets/resources/keyword_files/android/picovoice_android.ppn
cp ../../resources/keyword_files/android/porcupine_android.ppn ./assets/resources/keyword_files/android/porcupine_android.ppn
cp ../../resources/keyword_files/android/terminator_android.ppn ./assets/resources/keyword_files/android/terminator_android.ppn

echo "Creating iOS keyword resources directory..."
if [ ! -d "./assets/resources/keyword_files/ios" ]
then 
    mkdir -p ./assets/resources/keyword_files/ios
fi

echo "Copying iOS keyword files..."
cp ../../resources/keyword_files/ios/alexa_ios.ppn ./assets/resources/keyword_files/ios/alexa_ios.ppn
cp ../../resources/keyword_files/ios/americano_ios.ppn ./assets/resources/keyword_files/ios/americano_ios.ppn
cp ../../resources/keyword_files/ios/blueberry_ios.ppn ./assets/resources/keyword_files/ios/blueberry_ios.ppn
cp ../../resources/keyword_files/ios/bumblebee_ios.ppn ./assets/resources/keyword_files/ios/bumblebee_ios.ppn
cp ../../resources/keyword_files/ios/computer_ios.ppn ./assets/resources/keyword_files/ios/computer_ios.ppn
cp ../../resources/keyword_files/ios/grapefruit_ios.ppn ./assets/resources/keyword_files/ios/grapefruit_ios.ppn
cp ../../resources/keyword_files/ios/grasshopper_ios.ppn ./assets/resources/keyword_files/ios/grasshopper_ios.ppn
cp ../../resources/keyword_files/ios/hey\ google_ios.ppn ./assets/resources/keyword_files/ios/hey\ google_ios.ppn
cp ../../resources/keyword_files/ios/hey\ siri_ios.ppn ./assets/resources/keyword_files/ios/hey\ siri_ios.ppn
cp ../../resources/keyword_files/ios/jarvis_ios.ppn ./assets/resources/keyword_files/ios/jarvis_ios.ppn
cp ../../resources/keyword_files/ios/ok\ google_ios.ppn ./assets/resources/keyword_files/ios/ok\ google_ios.ppn
cp ../../resources/keyword_files/ios/picovoice_ios.ppn ./assets/resources/keyword_files/ios/picovoice_ios.ppn
cp ../../resources/keyword_files/ios/porcupine_ios.ppn ./assets/resources/keyword_files/ios/porcupine_ios.ppn
cp ../../resources/keyword_files/ios/terminator_ios.ppn ./assets/resources/keyword_files/ios/terminator_ios.ppn

echo "Copying license file..."
cp ../../LICENSE ./LICENSE
