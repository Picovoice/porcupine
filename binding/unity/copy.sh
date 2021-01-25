echo "Copying Android resouces..."
cp -rp ../../lib/android/* ./Assets/Porcupine/Plugins/android

echo "Copying iOS lib..."
cp ../../lib/ios/libpv_porcupine.a ./Assets/Porcupine/Plugins/ios/libpv_porcupine.a

echo "Copying Linux lib..."
cp ../../lib/linux/x86_64/libpv_porcupine.so ./Assets/Porcupine/Plugins/linux/x86_64/libpv_porcupine.so

echo "Copying macOS lib..."
cp ../../lib/mac/x86_64/libpv_porcupine.dylib ./Assets/Porcupine/Plugins/mac/x86_64/libpv_porcupine.dylib

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

echo "Copy complete!"