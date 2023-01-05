if [ ! -d "./porcupine-activity-demo-app/src/androidTest/assets/test_resources/audio_samples" ]
then
    echo "Creating test audio samples directory..."
    mkdir -p ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/audio_samples
fi

echo "Copying test audio samples..."
cp ../../../resources/audio_samples/*.wav ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/audio_samples

echo "Copying test keyword files..."
mkdir -p ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/en
cp ../../../resources/keyword_files/android/*.ppn ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/en/
cp ../../../resources/keyword_files/android/hey\ barista_android.ppn ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/en/hey_barista_android.ppn
cp ../../../resources/keyword_files/android/pico\ clock_android.ppn ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/en/pico_clock_android.ppn
cp ../../../resources/keyword_files/linux/alexa_linux.ppn ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/en/alexa_linux.ppn

for d in ../../../resources/keyword_files_*; do
    LANGUAGE=$(echo "${d}" | cut -d'_' -f3)

    mkdir -p ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/${LANGUAGE}
    cp ../../../resources/keyword_files_${LANGUAGE}/android/*.ppn ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/${LANGUAGE}/
done

if [ ! -d "./porcupine-activity-demo-app/src/androidTest/assets/test_resources/model_files" ]
then
    echo "Creating test model files directory..."
    mkdir -p ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/model_files
fi

echo "Copying test model files..."
cp ../../../lib/common/*.pv ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/model_files

echo "Copying test data file..."
cp ../../../resources/test/test_data.json ./porcupine-activity-demo-app/src/androidTest/assets/test_resources
