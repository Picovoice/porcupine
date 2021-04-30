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

// copy iOS resources
mkdirp.sync("./ios/resources/keyword_files")
fs.copyFileSync('../../lib/common/porcupine_params.pv','./ios/resources/porcupine_params.pv')
ncp('../../resources/keyword_files/ios', './ios/resources/keyword_files')
ncp('../../lib/ios/PvPorcupine.xcframework', './ios/PvPorcupine.xcframework')