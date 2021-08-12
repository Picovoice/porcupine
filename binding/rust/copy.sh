echo "Preparing dir ..."
mkdir -p ./data/lib/
mkdir -p ./data/resources/

echo! "Test to trip github actions"

echo "Copying Library Files ..."
cp -r ../../lib/beaglebone ./data/lib/
cp -r ../../lib/common ./data/lib/
cp -r ../../lib/jetson ./data/lib/
cp -r ../../lib/linux ./data/lib/
cp -r ../../lib/mac ./data/lib/
cp -r ../../lib/raspberry-pi ./data/lib/
cp -r ../../lib/windows ./data/lib/

echo "Copying Resource Files ..."
cp -r ../../resources/keyword_files ./data/resources/

echo "Copy complete!"
