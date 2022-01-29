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

const Porcupine = require("./index.js");
const fs = require("fs");
const { getInt16Frames, checkWaveFile } = require("./wave_util");
const WaveFile = require("wavefile").WaveFile;

const { PvArgumentError, PvStateError } = require("./errors");
const { getPlatform } = require("./platforms");
const { GRASSHOPPER } = require("./builtin_keywords.js");
const { count } = require("console");

const MODEL_PATH = "./lib/common/porcupine_params.pv";
const MODEL_PATH_DE = "../../lib/common/porcupine_params_de.pv";
const MODEL_PATH_ES = "../../lib/common/porcupine_params_es.pv";
const MODEL_PATH_FR = "../../lib/common/porcupine_params_fr.pv";

const DEFAULT_SENSITIVITY_ARRAY = [0.5];
const WAV_PATH_PORCUPINE =
  "../../resources/audio_samples/porcupine.wav";
const WAV_PATH_MULTIPLE_KEYWORDS =
  "../../resources/audio_samples/multiple_keywords.wav";
const WAV_PATH_SINGLE_KEYWORD_DE =
  "../../resources/audio_samples/heuschrecke.wav";
const WAV_PATH_MULTIPLE_KEYWORDS_DE =
  "../../resources/audio_samples/multiple_keywords_de.wav";
const WAV_PATH_SINGLE_KEYWORD_ES =
  "../../resources/audio_samples/manzana.wav";
const WAV_PATH_MULTIPLE_KEYWORDS_ES =
  "../../resources/audio_samples/multiple_keywords_es.wav";
const WAV_PATH_SINGLE_KEYWORD_FR =
  "../../resources/audio_samples/mon_chouchou.wav";
const WAV_PATH_MULTIPLE_KEYWORDS_FR =
  "../../resources/audio_samples/multiple_keywords_fr.wav";  
  const WAV_PATH_NON_ASCII =
  "../../resources/audio_samples/murciélago.wav";
const platform = getPlatform();

const keywordPathsSinglePorcupine = [
  `../../resources/keyword_files/${platform}/porcupine_${platform}.ppn`,
];
const keywordPathsMultipleAmericanoBumblebee = [
  `../../resources/keyword_files/${platform}/americano_${platform}.ppn`,
  `../../resources/keyword_files/${platform}/bumblebee_${platform}.ppn`,
];
const keywordPathsSingleDe = [
  `../../resources/keyword_files_de/${platform}/heuschrecke_${platform}.ppn`,
];
const keywordPathsMultipleDe = [
  `../../resources/keyword_files_de/${platform}/ananas_${platform}.ppn`,
  `../../resources/keyword_files_de/${platform}/heuschrecke_${platform}.ppn`,
  `../../resources/keyword_files_de/${platform}/leguan_${platform}.ppn`,
  `../../resources/keyword_files_de/${platform}/stachelschwein_${platform}.ppn`,  
];
const keywordPathsSingleEs = [
  `../../resources/keyword_files_es/${platform}/manzana_${platform}.ppn`,
];
const keywordPathsMultipleEs = [
  `../../resources/keyword_files_es/${platform}/emparedado_${platform}.ppn`,
  `../../resources/keyword_files_es/${platform}/leopardo_${platform}.ppn`,
  `../../resources/keyword_files_es/${platform}/manzana_${platform}.ppn`,
];
const keywordPathsSingleFr = [
  `../../resources/keyword_files_fr/${platform}/mon chouchou_${platform}.ppn`,
];
const keywordPathsMultipleFr = [
  `../../resources/keyword_files_fr/${platform}/framboise_${platform}.ppn`,
  `../../resources/keyword_files_fr/${platform}/mon chouchou_${platform}.ppn`,
  `../../resources/keyword_files_fr/${platform}/parapluie_${platform}.ppn`,
];
const keywordPathsNonAscii = [
  `../../resources/keyword_files_es/${platform}/murciélago_${platform}.ppn`,
];

const ACCESS_KEY = process.argv.filter((x) => x.startsWith('--access_key='))[0].split('--access_key=')[1];

function porcupineDetectionCounts(engineInstance, relativeWaveFilePath) {
  const path = require("path");
  const waveFilePath = path.join(__dirname, relativeWaveFilePath);
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
      ACCESS_KEY,
      keywordPathsSinglePorcupine,
      DEFAULT_SENSITIVITY_ARRAY
    );

    let counts = porcupineDetectionCounts(porcupineEngine, WAV_PATH_PORCUPINE);
    let porcupineCount = counts.get(0);
    expect(porcupineCount).toEqual(1);
  });

  test("builtin keyword 'GRASSHOPPER'", () => {
    let porcupineEngine = new Porcupine(
        ACCESS_KEY,
        [GRASSHOPPER], 
        DEFAULT_SENSITIVITY_ARRAY
    );

    let counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_MULTIPLE_KEYWORDS
    );
    let grasshopperCount = counts.get(0);
    expect(grasshopperCount).toEqual(1);
  });

  test("invalid builtin keyword type", () => {
    expect(() => {
      let porcupineEngine = new Porcupine(
        ACCESS_KEY,
        [99], 
        DEFAULT_SENSITIVITY_ARRAY
      );
    }).toThrow(PvArgumentError);
  });

  test("single keyword multiple detection", () => {
    let porcupineEngine = new Porcupine(
      ACCESS_KEY,
      keywordPathsSinglePorcupine,
      DEFAULT_SENSITIVITY_ARRAY
    );

    let counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_MULTIPLE_KEYWORDS
    );
    let porcupineCount = counts.get(0);
    expect(porcupineCount).toEqual(2);
  });

  test("multiple keyword multiple detection", () => {
    const sensitivities = new Array(keywordPathsMultipleAmericanoBumblebee.length).fill(0.5);
    let porcupineEngine = new Porcupine(
      ACCESS_KEY,
      keywordPathsMultipleAmericanoBumblebee,
      sensitivities
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
      ACCESS_KEY,  
      keywordPathsSinglePorcupine,
      DEFAULT_SENSITIVITY_ARRAY,
      MODEL_PATH
    );

    let counts = porcupineDetectionCounts(porcupineEngine, WAV_PATH_PORCUPINE);
    let porcupineCount = counts.get(0);
    expect(porcupineCount).toEqual(1);
  });
});

describe("keyword detection in DE", () => {
  test("single keyword single detection in DE", () => {
    let porcupineEngine = new Porcupine(
      ACCESS_KEY,  
      keywordPathsSingleDe,
      DEFAULT_SENSITIVITY_ARRAY,
      MODEL_PATH_DE
    );

    let counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_SINGLE_KEYWORD_DE
    );
    let heuschreckeCount = counts.get(0);
    expect(heuschreckeCount).toEqual(1);
  });

  test("multiple keyword multiple detection in DE", () => {
    const sensitivities = new Array(keywordPathsMultipleDe.length).fill(0.5);
    let porcupineEngine = new Porcupine(
      ACCESS_KEY,
      keywordPathsMultipleDe,
      sensitivities,
      MODEL_PATH_DE
    );

    let counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_MULTIPLE_KEYWORDS_DE
    );
    let ananasCount = counts.get(0);
    let heuschreckeCount = counts.get(1);
    let leguanCount = counts.get(2);
    let stachelschweinCount = counts.get(3);
    expect(ananasCount).toEqual(1);
    expect(heuschreckeCount).toEqual(1);
    expect(leguanCount).toEqual(1);
    expect(stachelschweinCount).toEqual(1);
  });  
});

describe("keyword detection in ES", () => {
  test("single keyword single detection in ES", () => {
    let porcupineEngine = new Porcupine(
      ACCESS_KEY,  
      keywordPathsSingleEs,
      DEFAULT_SENSITIVITY_ARRAY,
      MODEL_PATH_ES
    );

    let counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_SINGLE_KEYWORD_ES
    );
    let manzanaCount = counts.get(0);
    expect(manzanaCount).toEqual(1);
  });

  test("multiple keyword multiple detection in ES", () => {
    const sensitivities = new Array(keywordPathsMultipleEs.length).fill(0.5);
    let porcupineEngine = new Porcupine(
      ACCESS_KEY,
      keywordPathsMultipleEs,
      sensitivities,
      MODEL_PATH_ES
    );

    let counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_MULTIPLE_KEYWORDS_ES
    );
    let emparedadoCount = counts.get(0);
    let leopardoCount = counts.get(1);
    let manzanaCount = counts.get(2);
    expect(emparedadoCount).toEqual(1);
    expect(leopardoCount).toEqual(1);
    expect(manzanaCount).toEqual(1);
  });  
});

describe("keyword detection in FR", () => {
  test("single keyword single detection in FR", () => {
    
    let porcupineEngine = new Porcupine(
      ACCESS_KEY,  
      keywordPathsSingleFr,
      DEFAULT_SENSITIVITY_ARRAY,
      MODEL_PATH_FR
    );

    let counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_SINGLE_KEYWORD_FR
    );
    let monChouchouCount = counts.get(0);
    expect(monChouchouCount).toEqual(1);
  });

  test("multiple keyword multiple detection in FR", () => {
    const sensitivities = new Array(keywordPathsMultipleFr.length).fill(0.5);
    let porcupineEngine = new Porcupine(
      ACCESS_KEY,
      keywordPathsMultipleFr,
      sensitivities,
      MODEL_PATH_FR
    );

    let counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_MULTIPLE_KEYWORDS_FR
    );
    let framboiseCount = counts.get(0);
    let monChouchouCount = counts.get(1);
    let parapluieCount = counts.get(2);
    expect(framboiseCount).toEqual(2);
    expect(monChouchouCount).toEqual(1);
    expect(parapluieCount).toEqual(1);
  });  
});

describe("Non ascii characters", () => {
  test("single non ascii character in the model name", () => {
    let porcupineEngine = new Porcupine(
      ACCESS_KEY,
      keywordPathsNonAscii,
      DEFAULT_SENSITIVITY_ARRAY,
      MODEL_PATH_ES
    );

    let counts = porcupineDetectionCounts(porcupineEngine, WAV_PATH_NON_ASCII);
    expect(counts.get(0)).toEqual(2);
    porcupineEngine.release();

    expect(() => {
      count = porcupineDetectionCounts(porcupineEngine, WAV_PATH_NON_ASCII);
    }).toThrow(PvStateError);
  });
});

describe("basic parameter validation", () => {
  test("num of keywords does not match num of sensitivities", () => {
    expect(() => {
      let porcupineEngine = new Porcupine(ACCESS_KEY, keywordPathsSinglePorcupine, [
        0.1,
        0.2,
      ]);
    }).toThrow(PvArgumentError);
  });

  test("keywords is not an array", () => {
    try {
      let porcupineEngine = new Porcupine(
        ACCESS_KEY,  
        `../../resources/keyword_files/${platform}/porcupine_${platform}.ppn`,
        DEFAULT_SENSITIVITY_ARRAY
      );
    } catch (error) {
      expect(error.message.includes("array"));
    }
  });

  test("sensitivity is not in range", () => {
    expect(() => {
      let porcupineEngine = new Porcupine(ACCESS_KEY, keywordPathsSinglePorcupine, [4.2]);
    }).toThrow(RangeError);
  });

  test("sensitivity is not a number", () => {
    expect(() => {
      let porcupineEngine = new Porcupine(ACCESS_KEY, keywordPathsSinglePorcupine, [
        "steamed hams",
      ]);
    }).toThrow(Error);
  });

  test("invalid keyword path", () => {
    expect(() => {
      let porcupineEngine = new Porcupine(ACCESS_KEY, ["to be or not to be"], [0.5]);
    }).toThrow(Error);
  });
});

describe("frame validation", () => {
  test("mismatched frameLength throws error", () => {
    let porcupineEngine = new Porcupine(ACCESS_KEY, keywordPathsSinglePorcupine, [0.5]);
    expect(() => {
      porcupineEngine.process([1, 2, 3]);
    }).toThrow(PvArgumentError);
    porcupineEngine.release();
  });

  test("null/undefined frames throws error", () => {
    let porcupineEngine = new Porcupine(ACCESS_KEY, keywordPathsSinglePorcupine, [0.5]);
    expect(() => {
      porcupineEngine.process(null);
    }).toThrow(PvArgumentError);
    expect(() => {
      porcupineEngine.process(undefined);
    }).toThrow(PvArgumentError);
    porcupineEngine.release();
  });

  test("passing floating point frame values throws PvArgumentError", () => {
    let porcupineEngine = new Porcupine(ACCESS_KEY, keywordPathsSinglePorcupine, [0.5]);
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
      ACCESS_KEY,
      keywordPathsSinglePorcupine,
      DEFAULT_SENSITIVITY_ARRAY
    );

    let counts = porcupineDetectionCounts(porcupineEngine, WAV_PATH_PORCUPINE);
    expect(counts.get(0)).toEqual(1);
    porcupineEngine.release();

    expect(() => {
      count = porcupineDetectionCounts(porcupineEngine, WAV_PATH_PORCUPINE);
    }).toThrow(PvStateError);
  });
});
