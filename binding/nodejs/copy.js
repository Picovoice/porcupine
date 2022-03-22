//
// Copyright 2020-2021 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
// file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//
"use strict";

const mkdirp = require("mkdirp");
const ncp = require("ncp").ncp;

console.log("Copying library files...");

// Library & Model
mkdirp.sync("./lib/common");
ncp(
  "../../lib/common/porcupine_params.pv",
  "./lib/common/porcupine_params.pv",
  function (err) {
    if (err) {
      return console.error(err);
    }
    console.log("../../lib/common copied.");
  }
);

ncp(
  "../../lib/node",
  "./lib",
  function (err) {
    if (err) {
      return console.error(err);
    }
    console.log("../../lib/node copied.");
  }
);

let platforms = ["beaglebone", "jetson", "linux", "mac", "raspberry-pi",  "windows"];

// Keywords (resources)
// Only ship keywords that work on every platform (LCD)
let lcdKeywords = [
  "alexa",
  "americano",
  "blueberry",
  "bumblebee",
  "computer",
  "grapefruit",
  "grasshopper",
  "hey google",
  "hey siri",
  "jarvis",
  "ok google",
  "picovoice",
  "porcupine",
  "terminator",
];

console.log("Copying keyword files...");

mkdirp.sync("./resources/keyword_files");

for (let platform of platforms) {
  mkdirp.sync(`./resources/keyword_files/${platform}`);

  for (let keyword of lcdKeywords) {
    ncp(
      `../../resources/keyword_files/${platform}/${keyword}_${platform}.ppn`,
      `./resources/keyword_files/${platform}/${keyword}_${platform}.ppn`,
      function (err) {
        if (err) {
          return console.error(err);
        }
        console.log(
          `../../resources/keyword_files/${platform}/${keyword}_${platform}.ppn.`
        );
      }
    );
  }
}
