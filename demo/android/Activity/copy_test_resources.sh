if [ ! -d "./porcupine-activity-demo-app/src/androidTest/assets/test_resources/audio_samples" ]
then 
    echo "Creating test audio samples directory..."
    mkdir -p ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/audio_samples
fi

echo "Copying test audio samples..."
cp ../../../resources/audio_samples/porcupine.wav ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/audio_samples/porcupine.wav
cp ../../../resources/audio_samples/multiple_keywords.wav ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/audio_samples/multiple_keywords.wav

if [ ! -d "./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files" ]
then 
    echo "Creating test keyword files directory..."
    mkdir -p ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/de
    mkdir -p ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/en
    mkdir -p ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/es
    mkdir -p ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/fr
fi

echo "Copying test keyword files..."
cp ../../../resources/keyword_files/android/hey\ barista_android.ppn ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/en/hey_barista_android.ppn
cp ../../../resources/keyword_files/android/pico\ clock_android.ppn ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/en/pico_clock_android.ppn
cp ../../../resources/keyword_files/linux/alexa_linux.ppn ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/en/alexa_linux.ppn
cp ../../../resources/keyword_files_de/android/ananas_android.ppn ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/de/ananas_android.ppn
cp ../../../resources/keyword_files_es/android/emparedado_android.ppn ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/es/emparedado_android.ppn
cp ../../../resources/keyword_files_fr/android/framboise_android.ppn ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/keyword_files/fr/framboise_android.ppn

if [ ! -d "./porcupine-activity-demo-app/src/androidTest/assets/test_resources/model_files" ]
then 
    echo "Creating test model files directory..."
    mkdir -p ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/model_files
fi

echo "Copying test model files..."
cp ../../../lib/common/porcupine_params.pv ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/model_files/porcupine_params.pv
cp ../../../lib/common/porcupine_params_de.pv ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/model_files/porcupine_params_de.pv
cp ../../../lib/common/porcupine_params_es.pv ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/model_files/porcupine_params_es.pv
cp ../../../lib/common/porcupine_params_fr.pv ./porcupine-activity-demo-app/src/androidTest/assets/test_resources/model_files/porcupine_params_fr.pv
