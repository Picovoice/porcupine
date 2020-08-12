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

const Porcupine = require("./index.js");
const fs = require("fs");
const { getInt16Frames, checkWaveFile } = require("./wave_util");
const WaveFile = require("wavefile").WaveFile;

const { PvArgumentError, PvStateError } = require("./errors");
const { getPlatform } = require("./platforms");
const { GRASSHOPPER } = require("./builtin_keywords.js");

const MODEL_PATH = "./lib/common/porcupine_params.pv";

const SENSITIVITIES_1 = [0.5];
const SENSITIVITIES_2 = [0.5, 0.5];
const WAV_PATH_PORCUPINE = "../../resources/audio_samples/porcupine.wav";
const WAV_PATH_MULTIPLE_KEYWORDS =
  "../../resources/audio_samples/multiple_keywords.wav";

const platform = getPlatform();

const keywordPathsSinglePorcupine = [
  `../../resources/keyword_files/${platform}/porcupine_${platform}.ppn`,
];
const keywordPathsMultipleAmericanoBumblebee = [
  `../../resources/keyword_files/${platform}/americano_${platform}.ppn`,
  `../../resources/keyword_files/${platform}/bumblebee_${platform}.ppn`,
];

function porcupineDetectionCounts(engineInstance, waveFilePath) {
  const waveBuffer = fs.readFileSync(waveFilePath);
  const waveAudioFile = new WaveFile(waveBuffer);

  if (!checkWaveFile(waveAudioFile, engineInstance.sampleRate)) {
    console.error(
      "Audio file did not meet requirements. Wave file must be 16KHz, 16-bit, linear PCM (mono)."
    );
    return null;
  }

  const frames = getInt16Frames(waveAudioFile, engineInstance.frameLength);

  // build a map of keyword indices to detection counts
  let detections = new Map();

  for (let i = 0; i < frames.length; i++) {
    const frame = frames[i];
    const keywordIndex = engineInstance.process(frame);

    if (keywordIndex !== -1) {
      let currentCount = detections.get(keywordIndex);

      if (currentCount === undefined) {
        detections.set(keywordIndex, 1);
      } else {
        detections.set(keywordIndex, currentCount + 1);
      }
    }
  }
  return detections;
}

describe("successful keyword detections", () => {
  test("single keyword single detection", () => {
    let porcupineEngine = new Porcupine(
      keywordPathsSinglePorcupine,
      SENSITIVITIES_1
    );

    let counts = porcupineDetectionCounts(porcupineEngine, WAV_PATH_PORCUPINE);
    let porcupineCount = counts.get(0);
    expect(porcupineCount).toEqual(1);
  });

  test("builtin keyword 'GRASSHOPPER'", () => {
    let porcupineEngine = new Porcupine([GRASSHOPPER], SENSITIVITIES_1);

    let counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_MULTIPLE_KEYWORDS
    );
    let grasshopperCount = counts.get(0);
    expect(grasshopperCount).toEqual(1);
  });

  test("invalid builtin keyword type", () => {
    expect(() => {
      let porcupineEngine = new Porcupine([99], SENSITIVITIES_1);
    }).toThrow(PvArgumentError);
  });

  test("single keyword multiple detection", () => {
    let porcupineEngine = new Porcupine(
      keywordPathsSinglePorcupine,
      SENSITIVITIES_1
    );

    let counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_MULTIPLE_KEYWORDS
    );
    let porcupineCount = counts.get(0);
    expect(porcupineCount).toEqual(2);
  });

  test("multiple keyword multiple detection", () => {
    let porcupineEngine = new Porcupine(
      keywordPathsMultipleAmericanoBumblebee,
      SENSITIVITIES_2
    );

    let counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_MULTIPLE_KEYWORDS
    );
    let americanoCount = counts.get(0);
    let bumblebeeCount = counts.get(1);
    expect(americanoCount).toEqual(1);
    expect(bumblebeeCount).toEqual(1);
  });
});

describe("manual paths", () => {
  test("manual model path", () => {
    let porcupineEngine = new Porcupine(
      keywordPathsSinglePorcupine,
      SENSITIVITIES_1,
      MODEL_PATH
    );

    let counts = porcupineDetectionCounts(porcupineEngine, WAV_PATH_PORCUPINE);
    let porcupineCount = counts.get(0);
    expect(porcupineCount).toEqual(1);
  });
});

describe("basic parameter validation", () => {
  test("num of keywords does not match num of sensitivities", () => {
    expect(() => {
      let porcupineEngine = new Porcupine(keywordPathsSinglePorcupine, [
        0.1,
        0.2,
      ]);
    }).toThrow(PvArgumentError);
  });

  test("keywords is not an array", () => {
    try {
      let porcupineEngine = new Porcupine(
        `../../resources/keyword_files/${platform}/porcupine_${platform}.ppn`,
        SENSITIVITIES_1
      );
    } catch (error) {
      expect(error.message.includes("array"));
    }
  });

  test("sensitivity is not in range", () => {
    expect(() => {
      let porcupineEngine = new Porcupine(keywordPathsSinglePorcupine, [4.2]);
    }).toThrow(RangeError);
  });

  test("sensitivity is not a number", () => {
    expect(() => {
      let porcupineEngine = new Porcupine(keywordPathsSinglePorcupine, [
        "steamed hams",
      ]);
    }).toThrow(Error);
  });

  test("invalid keyword path", () => {
    expect(() => {
      let porcupineEngine = new Porcupine(["to be or not to be"], [0.5]);
    }).toThrow(Error);
  });
});

describe("frame validation", () => {
  test("mismatched frameLength throws error", () => {
    let porcupineEngine = new Porcupine(keywordPathsSinglePorcupine, [0.5]);
    expect(() => {
      porcupineEngine.process([1, 2, 3]);
    }).toThrow(PvArgumentError);
    porcupineEngine.release();
  });

  test("null/undefined frames throws error", () => {
    let porcupineEngine = new Porcupine(keywordPathsSinglePorcupine, [0.5]);
    expect(() => {
      porcupineEngine.process(null);
    }).toThrow(PvArgumentError);
    expect(() => {
      porcupineEngine.process(undefined);
    }).toThrow(PvArgumentError);
    porcupineEngine.release();
  });

  test("passing floating point frame values throws PvArgumentError", () => {
    let porcupineEngine = new Porcupine(keywordPathsSinglePorcupine, [0.5]);
    let floatFrames = Array.from({ length: porcupineEngine.frameLength }).map(
      (x) => 3.1415
    );
    expect(() => {
      porcupineEngine.process(floatFrames);
    }).toThrow(PvArgumentError);
    porcupineEngine.release();
  });
});

describe("invalid state", () => {
  test("process throws PvStateError if the engine is released", () => {
    let porcupineEngine = new Porcupine(
      keywordPathsSinglePorcupine,
      SENSITIVITIES_1
    );

    let counts = porcupineDetectionCounts(porcupineEngine, WAV_PATH_PORCUPINE);
    expect(counts.get(0)).toEqual(1);
    porcupineEngine.release();

    expect(() => {
      count = porcupineDetectionCounts(porcupineEngine, WAV_PATH_PORCUPINE);
    }).toThrow(PvStateError);
  });
});
