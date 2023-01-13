LIB_DIR="../../../../lib"
RESOURCE_DIR="../../../../resources"
ASSETS_DIR="./android/app/src/main/assets"

echo "Creating test resources asset directory"
mkdir -p ${ASSETS_DIR}

echo "Copying test audio samples..."
mkdir -p ${ASSETS_DIR}/audio_samples
cp ${RESOURCE_DIR}/audio_samples/*.wav ${ASSETS_DIR}/audio_samples

echo "Copying test keyword files..."
mkdir -p ${ASSETS_DIR}/keyword_files

mkdir -p ${ASSETS_DIR}/keyword_files/en
cp ${RESOURCE_DIR}/keyword_files/android/*.ppn ${ASSETS_DIR}/keyword_files/en/
cp ${RESOURCE_DIR}/keyword_files/ios/*.ppn ${ASSETS_DIR}/keyword_files/en/

for d in ${RESOURCE_DIR}/keyword_files_*; do
    LANGUAGE=$(echo "${d}" | cut -d'_' -f3)

    mkdir -p ${ASSETS_DIR}/keyword_files/${LANGUAGE}
    cp ${RESOURCE_DIR}/keyword_files_${LANGUAGE}/android/*.ppn ${ASSETS_DIR}/keyword_files/${LANGUAGE}
    cp ${RESOURCE_DIR}/keyword_files_${LANGUAGE}/ios/*.ppn ${ASSETS_DIR}/keyword_files/${LANGUAGE}
done

echo "Copying test model files..."
mkdir -p ${ASSETS_DIR}/model_files
cp ${LIB_DIR}/common/*.pv ${ASSETS_DIR}/model_files

echo "Copying test data file..."
cp ${RESOURCE_DIR}/test/test_data.json .
