echo "Creating Android lib directory..."
if [ ! -d "./android/src/main/jniLibs" ]
then 
    mkdir ./android/src/main/jniLibs
fi

echo "Copying Android libs..."
cp -rp ../../lib/android/* ./android/src/main/jniLibs

echo "Copying iOS libs..."
cp ../../lib/ios/libpv_porcupine.a ./ios/pv_porcupine/libpv_porcupine.a
cp ../../include/picovoice.h ./ios/pv_porcupine/picovoice.h
cp ../../include/pv_porcupine.h ./ios/pv_porcupine/pv_porcupine.h