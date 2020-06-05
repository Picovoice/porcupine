/*
    Copyright 2018-2020 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

let callback = function callback() {
  postMessage({ status: "ppn-init" });
};
let PorcupineOptions = { callback: callback };

importScripts("pv_porcupine.js");
importScripts("porcupine.js");

onmessage = function (e) {
  switch (e.data.command) {
    case "init":
      init(e.data.keywordIDs, e.data.sensitivities);
      break;
    case "process":
      process(e.data.inputFrame);
      break;
    case "pause":
      paused = true;
      break;
    case "resume":
      paused = false;
      break;
    case "release":
      release();
      break;
  }
};

let keywordIDArray;
let keywords;
let sensitivities;
let paused;
let porcupine = null;

function init(keywordIDs, _sensitivities_) {
  paused = false;
  keywordIDArray = Object.values(keywordIDs);
  keywords = Object.keys(keywordIDs);
  sensitivities = _sensitivities_;
  porcupine = Porcupine.create(keywordIDArray, sensitivities);
}

function process(inputFrame) {
  if (porcupine !== null && !paused) {
    let keywordIndex = porcupine.process(inputFrame);
    if (keywordIndex !== -1) {
      postMessage({
        keyword: keywords[keywordIndex],
      });
    }
  }
}

function release() {
  if (porcupine !== null) {
    porcupine.release();
  }

  porcupine = null;
  close();
}
