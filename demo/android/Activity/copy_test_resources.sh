LIB_DIR="../../../lib"
RESOURCE_DIR="../../../resources"
ASSETS_DIR="./porcupine-activity-demo-app/src/androidTest/assets/test_resources"

if [ ! -d "${ASSETS_DIR}/audio_samples" ]
then
    echo "Creating test audio samples directory..."
    mkdir -p ${ASSETS_DIR}/audio_samples
fi

echo "Copying test audio samples..."
cp ${RESOURCE_DIR}/audio_samples/*.wav ${ASSETS_DIR}/audio_samples

echo "Copying test keyword files..."
mkdir -p ${ASSETS_DIR}/keyword_files/en
cp ${RESOURCE_DIR}/keyword_files/android/*.ppn ${ASSETS_DIR}/keyword_files/en/
cp ${RESOURCE_DIR}/keyword_files/android/hey\ barista_android.ppn ${ASSETS_DIR}/keyword_files/en/hey_barista_android.ppn
cp ${RESOURCE_DIR}/keyword_files/android/pico\ clock_android.ppn ${ASSETS_DIR}/keyword_files/en/pico_clock_android.ppn
cp ${RESOURCE_DIR}/keyword_files/linux/alexa_linux.ppn ${ASSETS_DIR}/keyword_files/en/alexa_linux.ppn

for d in ${RESOURCE_DIR}/keyword_files_*; do
    LANGUAGE=$(echo "${d}" | cut -d'_' -f3)

    mkdir -p ${ASSETS_DIR}/keyword_files/${LANGUAGE}
    cp ${RESOURCE_DIR}/keyword_files_${LANGUAGE}/android/*.ppn ${ASSETS_DIR}/keyword_files/${LANGUAGE}/
done

if [ ! -d "${ASSETS_DIR}/model_files" ]
then
    echo "Creating test model files directory..."
    mkdir -p ${ASSETS_DIR}/model_files
fi

echo "Copying test model files..."
cp ${LIB_DIR}/common/*.pv ${ASSETS_DIR}/model_files

echo "Copying test data file..."
cp ${RESOURCE_DIR}/.test/test_data.json ${ASSETS_DIR}
