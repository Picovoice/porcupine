echo "Copying Porcupine model..."
cp ../../lib/common/porcupine_params.pv ./embedded/lib/common/porcupine_params.pv

echo "Copying Linux lib..."
cp ../../lib/linux/x86_64/libpv_porcupine.so ./embedded/lib/linux/x86_64/libpv_porcupine.so

echo "Copying macOS lib..."
cp ../../lib/mac/x86_64/libpv_porcupine.dylib ./embedded/lib/mac/x86_64/libpv_porcupine.dylib

echo "Copying Windows lib..."
cp ../../lib/windows/amd64/libpv_porcupine.dll ./embedded/lib/windows/amd64/libpv_porcupine.dll

echo "Copying RPi libs..."
cp -rp ../../lib/raspberry-pi/* ./embedded/lib/raspberry-pi

echo "Copying keyword files"
cp -rp ../../resources/keyword_files/windows/* ./embedded/resources/keyword_files/windows
cp -rp ../../resources/keyword_files/mac/* ./embedded/resources/keyword_files/mac
cp -rp ../../resources/keyword_files/linux/* ./embedded/resources/keyword_files/linux
cp -rp ../../resources/keyword_files/raspberry-pi/* ./embedded/resources/keyword_files/raspberry-pi

echo "Copy complete!"