echo "Copying Android resources..."
cp ../../lib/android/arm64-v8a/libpv_porcupine.so ./Assets/Porcupine/Plugins/android/arm64-v8a/libpv_porcupine.so
cp ../../lib/android/armeabi-v7a/libpv_porcupine.so ./Assets/Porcupine/Plugins/android/armeabi-v7a/libpv_porcupine.so

echo "Copying iOS lib..."
cp -R ../../lib/ios/PvPorcupine.xcframework/ios-arm64/PvPorcupine.framework ./Assets/Porcupine/Plugins/ios

echo "Copying Linux lib..."
cp ../../lib/linux/x86_64/libpv_porcupine.so ./Assets/Porcupine/Plugins/linux/x86_64/libpv_porcupine.so

echo "Copying macOS lib..."
cp ../../lib/mac/x86_64/libpv_porcupine.dylib ./Assets/Porcupine/Plugins/mac/x86_64/libpv_porcupine.dylib

echo "Copying macOS (Apple silicon) lib..."
cp ../../lib/mac/arm64/libpv_porcupine.dylib ./Assets/Porcupine/Plugins/mac/arm64/libpv_porcupine.dylib

echo "Copying Windows lib..."
cp ../../lib/windows/amd64/libpv_porcupine.dll ./Assets/Porcupine/Plugins/windows/amd64/pv_porcupine.dll

echo "Copying Porcupine common lib..."
cp ../../lib/common/porcupine_params.pv ./Assets/StreamingAssets/porcupine_params.pv

echo "Copying built-in keyword files..."
cp -rp ../../resources/keyword_files/android/* ./Assets/StreamingAssets/keyword_files/android
cp -rp ../../resources/keyword_files/ios/* ./Assets/StreamingAssets/keyword_files/ios
cp -rp ../../resources/keyword_files/linux/* ./Assets/StreamingAssets/keyword_files/linux
cp -rp ../../resources/keyword_files/mac/* ./Assets/StreamingAssets/keyword_files/mac
cp -rp ../../resources/keyword_files/windows/* ./Assets/StreamingAssets/keyword_files/windows

echo "Copying demo files..."
if [ ! -d "./Assets/Porcupine/Demo" ]
then
    mkdir -p ./Assets/Porcupine/Demo
fi
cp -rp ../../demo/unity/* ./Assets/Porcupine/Demo

echo "Copy complete!"