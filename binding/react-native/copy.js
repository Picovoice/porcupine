//
// Copyright 2020 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
// file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//
"use strict";

const fs = require("fs")
const ncp = require("ncp").ncp
const mkdirp = require("mkdirp");

// copy Android resources
fs.copyFileSync('../android/Porcupine/porcupine/src/main/java/ai/picovoice/porcupine/Porcupine.java','./android/src/main/java/ai/picovoice/reactnative/porcupine/Porcupine.java')
fs.copyFileSync('../android/Porcupine/porcupine/src/main/java/ai/picovoice/porcupine/PorcupineException.java','./android/src/main/java/ai/picovoice/reactnative/porcupine/PorcupineException.java')
mkdirp.sync("./android/src/main/jniLibs")
ncp('../../lib/android','./android/src/main/jniLibs')
mkdirp.sync("./android/src/main/res/raw")
ncp('../../resources/keyword_files/android', './android/src/main/res/raw')
fs.copyFileSync('../../lib/common/porcupine_params.pv','./android/src/main/res/raw/porcupine_params.pv')

// copy iOS resources
mkdirp.sync("./ios/resources/keyword_files")
fs.copyFileSync('../../lib/common/porcupine_params.pv','./ios/resources/porcupine_params.pv')
ncp('../../resources/keyword_files/ios', './ios/resources/keyword_files')
fs.copyFileSync('../../lib/ios/libpv_porcupine.a','./ios/pv_porcupine/libpv_porcupine.a')
fs.copyFileSync('../../include/picovoice.h','./ios/pv_porcupine/picovoice.h')
fs.copyFileSync('../../include/pv_porcupine.h','./ios/pv_porcupine/pv_porcupine.h')