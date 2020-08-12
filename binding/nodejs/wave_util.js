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

function chunkArray(array, size) {
  return Array.from({ length: Math.ceil(array.length / size) }, (v, index) =>
    array.slice(index * size, index * size + size)
  );
}

function checkWaveFile(waveFile, engineSampleRate) {
  let valid = true;

  if (waveFile.bitDepth !== "16") {
    console.error("Audio bit depth must be 16-bit");
    valid = false;
  }

  if (waveFile.fmt.numChannels !== 1) {
    console.error("Audio must be single channel");
    valid = false;
  }

  if (waveFile.fmt.sampleRate !== engineSampleRate) {
    console.error(
      `Audio sample rate ${waveFile.sampleRate} does not match Porcupine sample rate ${engineSampleRate}`
    );
    valid = false;
  }

  return valid;
}

function getInt16Frames(waveFile, frameLength) {
  const samples = waveFile.getSamples(false, Int16Array);

  let frames = chunkArray(samples, frameLength);

  // throw out remainder / partial frame
  if (frames[frames.length - 1].length !== frameLength) {
    frames.pop();
  }

  return frames;
}

exports.checkWaveFile = checkWaveFile;
exports.getInt16Frames = getInt16Frames;
