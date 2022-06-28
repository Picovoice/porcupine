//
// Copyright 2020-2022 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
// file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//
"use strict";

import * as fs from "fs";
import * as path from "path";

import Porcupine from "../src/porcupine";
import {checkWaveFile, getInt16Frames} from "../src/wave_util";
import {WaveFile} from "wavefile";

import {PorcupineInvalidArgumentError, PorcupineInvalidStateError} from "../src/errors";
import {getPlatform} from "../src/platforms";
import {BuiltinKeyword} from "../src/builtin_keywords";

const MODEL_PATH = "./lib/common/porcupine_params.pv";
const MODEL_PATH_DE = "../../lib/common/porcupine_params_de.pv";
const MODEL_PATH_ES = "../../lib/common/porcupine_params_es.pv";
const MODEL_PATH_FR = "../../lib/common/porcupine_params_fr.pv";
const MODEL_PATH_IT = "../../lib/common/porcupine_params_it.pv";
const MODEL_PATH_JA = "../../lib/common/porcupine_params_ja.pv";
const MODEL_PATH_KO = "../../lib/common/porcupine_params_ko.pv";
const MODEL_PATH_PT = "../../lib/common/porcupine_params_pt.pv";

const DEFAULT_SENSITIVITY_ARRAY = [0.5];

const WAV_PATH_PORCUPINE =
  "../../../resources/audio_samples/porcupine.wav";
const WAV_PATH_MULTIPLE_KEYWORDS =
  "../../../resources/audio_samples/multiple_keywords.wav";
const WAV_PATH_SINGLE_KEYWORD_DE =
  "../../../resources/audio_samples/heuschrecke.wav";
const WAV_PATH_MULTIPLE_KEYWORDS_DE =
  "../../../resources/audio_samples/multiple_keywords_de.wav";
const WAV_PATH_SINGLE_KEYWORD_ES =
  "../../../resources/audio_samples/manzana.wav";
const WAV_PATH_MULTIPLE_KEYWORDS_ES =
  "../../../resources/audio_samples/multiple_keywords_es.wav";
const WAV_PATH_SINGLE_KEYWORD_FR =
  "../../../resources/audio_samples/mon_chouchou.wav";
const WAV_PATH_MULTIPLE_KEYWORDS_FR =
  "../../../resources/audio_samples/multiple_keywords_fr.wav";
const WAV_PATH_MULTIPLE_KEYWORDS_IT =
  "../../../resources/audio_samples/multiple_keywords_it.wav";
const WAV_PATH_MULTIPLE_KEYWORDS_JA =
  "../../../resources/audio_samples/multiple_keywords_ja.wav";
const WAV_PATH_MULTIPLE_KEYWORDS_KO =
  "../../../resources/audio_samples/multiple_keywords_ko.wav";
const WAV_PATH_MULTIPLE_KEYWORDS_PT =
  "../../../resources/audio_samples/multiple_keywords_pt.wav";

const WAV_PATH_NON_ASCII =
  "../../../resources/audio_samples/murciélago.wav";
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
const keywordPathsMultipleIt = [
  `../../resources/keyword_files_it/${platform}/cameriere_${platform}.ppn`,
  `../../resources/keyword_files_it/${platform}/espresso_${platform}.ppn`,
  `../../resources/keyword_files_it/${platform}/porcospino_${platform}.ppn`,
];
const keywordPathsMultipleJa = [
  `../../resources/keyword_files_ja/${platform}/bushi_${platform}.ppn`,
  `../../resources/keyword_files_ja/${platform}/ninja_${platform}.ppn`,
  `../../resources/keyword_files_ja/${platform}/ringo_${platform}.ppn`,
];
const keywordPathsMultipleKo = [
  `../../resources/keyword_files_ko/${platform}/aiseukeulim_${platform}.ppn`,
  `../../resources/keyword_files_ko/${platform}/bigseubi_${platform}.ppn`,
  `../../resources/keyword_files_ko/${platform}/koppulso_${platform}.ppn`,
];
const keywordPathsMultiplePt = [
  `../../resources/keyword_files_pt/${platform}/abacaxi_${platform}.ppn`,
  `../../resources/keyword_files_pt/${platform}/fenomeno_${platform}.ppn`,
  `../../resources/keyword_files_pt/${platform}/formiga_${platform}.ppn`,
];
const keywordPathsNonAscii = [
  `../../resources/keyword_files_es/${platform}/murciélago_${platform}.ppn`,
];

const ACCESS_KEY = process.argv.filter((x) => x.startsWith('--access_key='))[0]?.split('--access_key=')[1] ?? "";

function porcupineDetectionCounts(engineInstance: Porcupine, relativeWaveFilePath: string): Map<number, number> {
  const waveFilePath = path.join(__dirname, relativeWaveFilePath);
  const waveBuffer = fs.readFileSync(waveFilePath);
  const waveAudioFile = new WaveFile(waveBuffer);

  if (!checkWaveFile(waveAudioFile, engineInstance.sampleRate)) {
    fail(
      "Audio file did not meet requirements. Wave file must be 16KHz, 16-bit, linear PCM (mono)."
    );
  }

  const frames = getInt16Frames(waveAudioFile, engineInstance.frameLength);

  // build a map of keyword indices to detection counts
  const detections = new Map();

  for (let i = 0; i < frames.length; i++) {
    const frame = frames[i];
    const keywordIndex = engineInstance.process(frame);

    if (keywordIndex !== -1) {
      const currentCount = detections.get(keywordIndex);

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
    const porcupineEngine = new Porcupine(
      ACCESS_KEY,
      keywordPathsSinglePorcupine,
      DEFAULT_SENSITIVITY_ARRAY
    );

    const counts = porcupineDetectionCounts(porcupineEngine, WAV_PATH_PORCUPINE);
    const porcupineCount = counts.get(0);
    expect(porcupineCount).toEqual(1);
  });

  test("builtin keyword 'GRASSHOPPER'", () => {
    const porcupineEngine = new Porcupine(
      ACCESS_KEY,
      [BuiltinKeyword.GRASSHOPPER],
      DEFAULT_SENSITIVITY_ARRAY
    );

    const counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_MULTIPLE_KEYWORDS
    );
    const grasshopperCount = counts.get(0);
    expect(grasshopperCount).toEqual(1);
  });

  test("invalid builtin keyword type", () => {
    expect(() => {
      const porcupineEngine = new Porcupine(
        ACCESS_KEY,
        // @ts-expect-error
        [99],
        DEFAULT_SENSITIVITY_ARRAY
      );
    }).toThrow(PorcupineInvalidArgumentError);
  });

  test("single keyword multiple detection", () => {
    const porcupineEngine = new Porcupine(
      ACCESS_KEY,
      keywordPathsSinglePorcupine,
      DEFAULT_SENSITIVITY_ARRAY
    );

    const counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_MULTIPLE_KEYWORDS
    );
    const porcupineCount = counts.get(0);
    expect(porcupineCount).toEqual(2);
  });

  test("multiple keyword multiple detection", () => {
    const sensitivities = new Array(keywordPathsMultipleAmericanoBumblebee.length).fill(0.5);
    const porcupineEngine = new Porcupine(
      ACCESS_KEY,
      keywordPathsMultipleAmericanoBumblebee,
      sensitivities
    );

    const counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_MULTIPLE_KEYWORDS
    );
    const americanoCount = counts.get(0);
    const bumblebeeCount = counts.get(1);
    expect(americanoCount).toEqual(1);
    expect(bumblebeeCount).toEqual(1);
  });
});

describe("manual paths", () => {
  test("manual model path", () => {
    const porcupineEngine = new Porcupine(
      ACCESS_KEY,
      keywordPathsSinglePorcupine,
      DEFAULT_SENSITIVITY_ARRAY,
      MODEL_PATH
    );

    const counts = porcupineDetectionCounts(porcupineEngine, WAV_PATH_PORCUPINE);
    const porcupineCount = counts.get(0);
    expect(porcupineCount).toEqual(1);
  });
});

describe("keyword detection in DE", () => {
  test("single keyword single detection in DE", () => {
    const porcupineEngine = new Porcupine(
      ACCESS_KEY,
      keywordPathsSingleDe,
      DEFAULT_SENSITIVITY_ARRAY,
      MODEL_PATH_DE
    );

    const counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_SINGLE_KEYWORD_DE
    );
    const heuschreckeCount = counts.get(0);
    expect(heuschreckeCount).toEqual(1);
  });

  test("multiple keyword multiple detection in DE", () => {
    const sensitivities = new Array(keywordPathsMultipleDe.length).fill(0.5);
    const porcupineEngine = new Porcupine(
      ACCESS_KEY,
      keywordPathsMultipleDe,
      sensitivities,
      MODEL_PATH_DE
    );

    const counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_MULTIPLE_KEYWORDS_DE
    );
    const ananasCount = counts.get(0);
    const heuschreckeCount = counts.get(1);
    const leguanCount = counts.get(2);
    const stachelschweinCount = counts.get(3);
    expect(ananasCount).toEqual(1);
    expect(heuschreckeCount).toEqual(1);
    expect(leguanCount).toEqual(1);
    expect(stachelschweinCount).toEqual(1);
  });
});

describe("keyword detection in ES", () => {
  test("single keyword single detection in ES", () => {
    const porcupineEngine = new Porcupine(
      ACCESS_KEY,
      keywordPathsSingleEs,
      DEFAULT_SENSITIVITY_ARRAY,
      MODEL_PATH_ES
    );

    const counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_SINGLE_KEYWORD_ES
    );
    const manzanaCount = counts.get(0);
    expect(manzanaCount).toEqual(1);
  });

  test("multiple keyword multiple detection in ES", () => {
    const sensitivities = new Array(keywordPathsMultipleEs.length).fill(0.5);
    const porcupineEngine = new Porcupine(
      ACCESS_KEY,
      keywordPathsMultipleEs,
      sensitivities,
      MODEL_PATH_ES
    );

    const counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_MULTIPLE_KEYWORDS_ES
    );
    const emparedadoCount = counts.get(0);
    const leopardoCount = counts.get(1);
    const manzanaCount = counts.get(2);
    expect(emparedadoCount).toEqual(1);
    expect(leopardoCount).toEqual(1);
    expect(manzanaCount).toEqual(1);
  });
});

describe("keyword detection in FR", () => {
  test("single keyword single detection in FR", () => {

    const porcupineEngine = new Porcupine(
      ACCESS_KEY,
      keywordPathsSingleFr,
      DEFAULT_SENSITIVITY_ARRAY,
      MODEL_PATH_FR
    );

    const counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_SINGLE_KEYWORD_FR
    );
    const monChouchouCount = counts.get(0);
    expect(monChouchouCount).toEqual(1);
  });

  test("multiple keyword multiple detection in FR", () => {
    const sensitivities = new Array(keywordPathsMultipleFr.length).fill(0.5);
    const porcupineEngine = new Porcupine(
      ACCESS_KEY,
      keywordPathsMultipleFr,
      sensitivities,
      MODEL_PATH_FR
    );

    const counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_MULTIPLE_KEYWORDS_FR
    );
    const framboiseCount = counts.get(0);
    const monChouchouCount = counts.get(1);
    const parapluieCount = counts.get(2);
    expect(framboiseCount).toEqual(2);
    expect(monChouchouCount).toEqual(1);
    expect(parapluieCount).toEqual(1);
  });
});

describe("keyword detection in IT", () => {
  test("multiple keyword multiple detection in IT", () => {
    const sensitivities = new Array(keywordPathsMultipleIt.length).fill(0.5);
    const porcupineEngine = new Porcupine(
      ACCESS_KEY,
      keywordPathsMultipleIt,
      sensitivities,
      MODEL_PATH_IT
    );

    const counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_MULTIPLE_KEYWORDS_IT
    );
    const cameriereCount = counts.get(0);
    const porcospinoCount = counts.get(1);
    const espressoCount = counts.get(2);
    expect(cameriereCount).toEqual(1);
    expect(porcospinoCount).toEqual(1);
    expect(espressoCount).toEqual(1);
  });
});

describe("keyword detection in JA", () => {
  test("multiple keyword multiple detection in JA", () => {
    const sensitivities = new Array(keywordPathsMultipleJa.length).fill(0.5);
    const porcupineEngine = new Porcupine(
      ACCESS_KEY,
      keywordPathsMultipleJa,
      sensitivities,
      MODEL_PATH_JA
    );

    const counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_MULTIPLE_KEYWORDS_JA
    );
    const cameriereCount = counts.get(0);
    const porcospinoCount = counts.get(1);
    const espressoCount = counts.get(2);
    expect(cameriereCount).toEqual(1);
    expect(porcospinoCount).toEqual(1);
    expect(espressoCount).toEqual(1);
  });
});

describe("keyword detection in KO", () => {
  test("multiple keyword multiple detection in KO", () => {
    const sensitivities = new Array(keywordPathsMultipleKo.length).fill(0.5);
    const porcupineEngine = new Porcupine(
      ACCESS_KEY,
      keywordPathsMultipleKo,
      sensitivities,
      MODEL_PATH_KO
    );

    const counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_MULTIPLE_KEYWORDS_KO
    );
    const cameriereCount = counts.get(0);
    const porcospinoCount = counts.get(1);
    const espressoCount = counts.get(2);
    expect(cameriereCount).toEqual(1);
    expect(porcospinoCount).toEqual(1);
    expect(espressoCount).toEqual(1);
  });
});

describe("keyword detection in PT", () => {
  test("multiple keyword multiple detection in PT", () => {
    const sensitivities = new Array(keywordPathsMultiplePt.length).fill(0.5);
    const porcupineEngine = new Porcupine(
      ACCESS_KEY,
      keywordPathsMultiplePt,
      sensitivities,
      MODEL_PATH_PT
    );

    const counts = porcupineDetectionCounts(
      porcupineEngine,
      WAV_PATH_MULTIPLE_KEYWORDS_PT
    );
    const cameriereCount = counts.get(0);
    const porcospinoCount = counts.get(1);
    const espressoCount = counts.get(2);
    expect(cameriereCount).toEqual(1);
    expect(porcospinoCount).toEqual(1);
    expect(espressoCount).toEqual(1);
  });
});


describe("Non ascii characters", () => {
  test("single non ascii character in the model name", () => {
    const porcupineEngine = new Porcupine(
      ACCESS_KEY,
      keywordPathsNonAscii,
      DEFAULT_SENSITIVITY_ARRAY,
      MODEL_PATH_ES
    );

    const counts = porcupineDetectionCounts(porcupineEngine, WAV_PATH_NON_ASCII);
    expect(counts.get(0)).toEqual(2);
    porcupineEngine.release();

    expect(() => {
      const count = porcupineDetectionCounts(porcupineEngine, WAV_PATH_NON_ASCII);
    }).toThrow(PorcupineInvalidStateError);
  });
});

describe("basic parameter validation", () => {
  test("num of keywords does not match num of sensitivities", () => {
    expect(() => {
      const porcupineEngine = new Porcupine(ACCESS_KEY, keywordPathsSinglePorcupine, [
        0.1,
        0.2,
      ]);
    }).toThrow(PorcupineInvalidArgumentError);
  });

  test("keywords is not an array", () => {
    try {
      const porcupineEngine = new Porcupine(
        ACCESS_KEY,
        // @ts-expect-error
        `../../resources/keyword_files/${platform}/porcupine_${platform}.ppn`,
        DEFAULT_SENSITIVITY_ARRAY
      );
    } catch (error: any) {
      expect(error.message.includes("array"));
    }
  });

  test("sensitivity is not in range", () => {
    expect(() => {
      const porcupineEngine = new Porcupine(ACCESS_KEY, keywordPathsSinglePorcupine, [4.2]);
    }).toThrow(RangeError);
  });

  test("sensitivity is not a number", () => {
    expect(() => {
      const porcupineEngine = new Porcupine(ACCESS_KEY, keywordPathsSinglePorcupine, [
        // @ts-expect-error
        "steamed hams",
      ]);
    }).toThrow(Error);
  });

  test("invalid keyword path", () => {
    expect(() => {
      const porcupineEngine = new Porcupine(ACCESS_KEY, ["to be or not to be"], [0.5]);
    }).toThrow(Error);
  });
});

describe("frame validation", () => {
  test("accepts non Int16Array if array is valid", () => {
    const porcupineEngine = new Porcupine(ACCESS_KEY, keywordPathsSinglePorcupine, [0.5]);
    const emptyArray = Array.apply(null, Array(porcupineEngine.frameLength)).map((x, i) => i)
    // @ts-expect-error
    porcupineEngine.process(emptyArray);
    porcupineEngine.release();
  });

  test("mismatched frameLength throws error", () => {
    const porcupineEngine = new Porcupine(ACCESS_KEY, keywordPathsSinglePorcupine, [0.5]);
    expect(() => {
      // @ts-expect-error
      porcupineEngine.process([1, 2, 3]);
    }).toThrow(PorcupineInvalidArgumentError);
    porcupineEngine.release();
  });

  test("null/undefined frames throws error", () => {
    const porcupineEngine = new Porcupine(ACCESS_KEY, keywordPathsSinglePorcupine, [0.5]);
    expect(() => {
      // @ts-expect-error
      porcupineEngine.process(null);
    }).toThrow(PorcupineInvalidArgumentError);
    expect(() => {
      // @ts-expect-error
      porcupineEngine.process(undefined);
    }).toThrow(PorcupineInvalidArgumentError);
    porcupineEngine.release();
  });

  test("passing floating point frame values throws PorcupineInvalidArgumentError", () => {
    const porcupineEngine = new Porcupine(ACCESS_KEY, keywordPathsSinglePorcupine, [0.5]);
    const floatFrames = Array.from({length: porcupineEngine.frameLength}).map(
      (x) => 3.1415
    );
    expect(() => {
      // @ts-expect-error
      porcupineEngine.process(floatFrames);
    }).toThrow(PorcupineInvalidArgumentError);
    porcupineEngine.release();
  });
});

describe("invalid state", () => {
  test("process throws PorcupineInvalidStateError if the engine is released", () => {
    const porcupineEngine = new Porcupine(
      ACCESS_KEY,
      keywordPathsSinglePorcupine,
      DEFAULT_SENSITIVITY_ARRAY
    );

    const counts = porcupineDetectionCounts(porcupineEngine, WAV_PATH_PORCUPINE);
    expect(counts.get(0)).toEqual(1);
    porcupineEngine.release();

    expect(() => {
      const counts = porcupineDetectionCounts(porcupineEngine, WAV_PATH_PORCUPINE);
    }).toThrow(PorcupineInvalidStateError);
  });
});
