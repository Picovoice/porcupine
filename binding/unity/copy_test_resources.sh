LIB_DIR="../../lib"
RESOURCE_DIR="../../resources"
ASSETS_DIR="./Assets/StreamingAssets/test"

echo "Creating test resources asset directory"
mkdir -p ${ASSETS_DIR}

echo "Copying test audio samples..."
mkdir -p ${ASSETS_DIR}/audio_samples
cp ${RESOURCE_DIR}/audio_samples/*.wav ${ASSETS_DIR}/audio_samples

echo "Copying test keyword files..."
mkdir -p ${ASSETS_DIR}/keyword_files
cp -r ${RESOURCE_DIR}/keyword_files/android ${ASSETS_DIR}/keyword_files/
cp -r ${RESOURCE_DIR}/keyword_files/ios ${ASSETS_DIR}/keyword_files/
cp -r ${RESOURCE_DIR}/keyword_files/linux ${ASSETS_DIR}/keyword_files/
cp -r ${RESOURCE_DIR}/keyword_files/mac ${ASSETS_DIR}/keyword_files/
cp -r ${RESOURCE_DIR}/keyword_files/windows ${ASSETS_DIR}/keyword_files/

for d in ${RESOURCE_DIR}/keyword_files_*; do
    LANGUAGE=$(echo "${d}" | cut -d'_' -f3)

    mkdir -p ${ASSETS_DIR}/keyword_files_${LANGUAGE}
    cp -r ${RESOURCE_DIR}/keyword_files_${LANGUAGE}/android ${ASSETS_DIR}/keyword_files_${LANGUAGE}/
    cp -r ${RESOURCE_DIR}/keyword_files_${LANGUAGE}/ios ${ASSETS_DIR}/keyword_files_${LANGUAGE}/
    cp -r ${RESOURCE_DIR}/keyword_files_${LANGUAGE}/linux ${ASSETS_DIR}/keyword_files_${LANGUAGE}/
    cp -r ${RESOURCE_DIR}/keyword_files_${LANGUAGE}/mac ${ASSETS_DIR}/keyword_files_${LANGUAGE}/
    cp -r ${RESOURCE_DIR}/keyword_files_${LANGUAGE}/windows ${ASSETS_DIR}/keyword_files_${LANGUAGE}/
done

echo "Copying test model files..."
mkdir -p ${ASSETS_DIR}/model_files
cp ${LIB_DIR}/common/*.pv ${ASSETS_DIR}/model_files

echo "Copying test data file..."
cp ${RESOURCE_DIR}/.test/test_data.json ${ASSETS_DIR}
