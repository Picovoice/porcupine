if [ ! -d "./porcupine-activity-demo-app/src/androidTest/assets/test_resources/audio_samples" ]
then 
    echo "Creating test audio samples directory..."
    mkdir -p ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/audio_samples
fi

echo "Copying test audio samples..."
cp ../../../resources/audio_samples/*.wav ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/audio_samples

echo "Creating test keyword files directory..."
mkdir -p ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/de
mkdir -p ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/en
mkdir -p ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/es
mkdir -p ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/fr
mkdir -p ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/it
mkdir -p ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/ko
mkdir -p ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/ja
mkdir -p ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/pt

echo "Copying test keyword files..."
cp ../../../resources/keyword_files/android/hey\ barista_android.ppn ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/en/hey_barista_android.ppn
cp ../../../resources/keyword_files/android/pico\ clock_android.ppn ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/en/pico_clock_android.ppn
cp ../../../resources/keyword_files/linux/alexa_linux.ppn ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/en/alexa_linux.ppn

cp ../../../resources/keyword_files_de/android/*.ppn ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/de/
cp ../../../resources/keyword_files_es/android/*.ppn ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/es/
cp ../../../resources/keyword_files_fr/android/*.ppn ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/fr/
cp ../../../resources/keyword_files_es/android/*.ppn ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/es/
cp ../../../resources/keyword_files_it/android/*.ppn ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/it/
cp ../../../resources/keyword_files_ko/android/*.ppn ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/ko/
cp ../../../resources/keyword_files_ja/android/*.ppn ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/ja/
cp ../../../resources/keyword_files_pt/android/*.ppn ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/pt/

if [ ! -d "./porcupine-activity-demo-app/src/androidTest/assets/test_resources/model_files" ]
then 
    echo "Creating test model files directory..."
    mkdir -p ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/model_files
fi

echo "Copying test model files..."
cp ../../../lib/common/*.pv ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/model_files
