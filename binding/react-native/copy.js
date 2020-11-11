const fs = require("fs")
const ncp = require("ncp").ncp

// copy Android resources
fs.copyFileSync('../android/Porcupine/porcupine/src/main/java/ai/picovoice/porcupine/Porcupine.java','./android/src/main/java/ai/picovoice/porcupine/Porcupine.java')
fs.copyFileSync('../android/Porcupine/porcupine/src/main/java/ai/picovoice/porcupine/PorcupineException.java','./android/src/main/java/ai/picovoice/porcupine/PorcupineException.java')
ncp('../../lib/android','./android/src/main/jniLibs')
ncp('../../resources/keyword_files/android', './android/src/main/res/raw')
fs.copyFileSync('../../lib/common/porcupine_params.pv','./android/src/main/res/raw/porcupine_params.pv')

// copy iOS resources
fs.copyFileSync('../../lib/common/porcupine_params.pv','./ios/resources/porcupine_params.pv')
ncp('../../resources/keyword_files/ios', './ios/resources/keyword_files')
fs.copyFileSync('../../lib/ios/libpv_porcupine.a','./ios/pv_porcupine/libpv_porcupine.a')
fs.copyFileSync('../../include/picovoice.h','./ios/pv_porcupine/picovoice.h')
fs.copyFileSync('../../include/pv_porcupine.h','./ios/pv_porcupine/pv_porcupine.h')