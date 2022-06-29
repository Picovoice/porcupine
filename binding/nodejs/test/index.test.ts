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

import Porcupine from "../src/porcupine";
import {checkWaveFile, getInt16Frames} from "../src/wave_util";
import {WaveFile} from "wavefile";

import {PorcupineActivationError, PorcupineInvalidArgumentError, PorcupineInvalidStateError} from "../src/errors";
import {BuiltinKeyword} from "../src/builtin_keywords";
import {
    getAudioFileByLanguage,
    getKeywordPathsByLanguage,
    getModelPathByLanguage} from "./test_utils"

const SINGLE_KEYWORD_PARAMETERS: [string, string[], number[], string][] = [
    ['en', ['porcupine'], [0], 'porcupine.wav'],
    ['es', ['manzana'], [0], 'manzana.wav'],
    ['de', ['heuschrecke'], [0], 'heuschrecke.wav'],
    ['fr', ['mon chouchou'], [0], 'mon_chouchou.wav'],
];

const MULTIPLE_KEYWORDS_PARAMETERS: [string, string[], number[]][] = [
    ['en',
        ['americano', 'blueberry', 'bumblebee', 'grapefruit', 'grasshopper', 'picovoice', 'porcupine', 'terminator'],
        [6, 0, 1, 2, 3, 4, 5, 6, 7]],
    ['es', ['emparedado', 'leopardo', 'manzana'], [0, 1, 2]],
    ['de', ['ananas', 'heuschrecke', 'leguan', 'stachelschwein'], [0, 1, 2, 3]],
    ['fr', ['framboise', 'mon chouchou', 'parapluie'], [0, 1, 0, 2]],
    ['it', ['espresso', 'cameriere', 'porcospino'], [2, 0, 1]],
    ['ja', ['ninja', 'bushi', 'ringo'], [2, 1, 0]],
    ['ko', ['aiseukeulim', 'bigseubi', 'koppulso'], [1, 2, 0]],
    ['pt', ['abacaxi', 'fenomeno', 'formiga'], [0, 2, 1]],
];

// const MODEL_PATH = "./lib/common/porcupine_params.pv";
// const MODEL_PATH_DE = "../../lib/common/porcupine_params_de.pv";
// const MODEL_PATH_ES = "../../lib/common/porcupine_params_es.pv";
// const MODEL_PATH_FR = "../../lib/common/porcupine_params_fr.pv";
// const MODEL_PATH_IT = "../../lib/common/porcupine_params_it.pv";
// const MODEL_PATH_JA = "../../lib/common/porcupine_params_ja.pv";
// const MODEL_PATH_KO = "../../lib/common/porcupine_params_ko.pv";
// const MODEL_PATH_PT = "../../lib/common/porcupine_params_pt.pv";
//
// const DEFAULT_SENSITIVITY_ARRAY = [0.5];
//
// const WAV_PATH_PORCUPINE =
//   "../../../resources/audio_samples/porcupine.wav";
// const WAV_PATH_MULTIPLE_KEYWORDS =
//   "../../../resources/audio_samples/multiple_keywords.wav";
// const WAV_PATH_SINGLE_KEYWORD_DE =
//   "../../../resources/audio_samples/heuschrecke.wav";
// const WAV_PATH_MULTIPLE_KEYWORDS_DE =
//   "../../../resources/audio_samples/multiple_keywords_de.wav";
// const WAV_PATH_SINGLE_KEYWORD_ES =
//   "../../../resources/audio_samples/manzana.wav";
// const WAV_PATH_MULTIPLE_KEYWORDS_ES =
//   "../../../resources/audio_samples/multiple_keywords_es.wav";
// const WAV_PATH_SINGLE_KEYWORD_FR =
//   "../../../resources/audio_samples/mon_chouchou.wav";
// const WAV_PATH_MULTIPLE_KEYWORDS_FR =
//   "../../../resources/audio_samples/multiple_keywords_fr.wav";
// const WAV_PATH_MULTIPLE_KEYWORDS_IT =
//   "../../../resources/audio_samples/multiple_keywords_it.wav";
// const WAV_PATH_MULTIPLE_KEYWORDS_JA =
//   "../../../resources/audio_samples/multiple_keywords_ja.wav";
// const WAV_PATH_MULTIPLE_KEYWORDS_KO =
//   "../../../resources/audio_samples/multiple_keywords_ko.wav";
// const WAV_PATH_MULTIPLE_KEYWORDS_PT =
//   "../../../resources/audio_samples/multiple_keywords_pt.wav";
//
// const WAV_PATH_NON_ASCII =
//   "../../../resources/audio_samples/murciélago.wav";
// const platform = getPlatform();
//
// const keywordPathsSinglePorcupine = [
//   `../../resources/keyword_files/${platform}/porcupine_${platform}.ppn`,
// ];
// const keywordPathsMultipleAmericanoBumblebee = [
//   `../../resources/keyword_files/${platform}/americano_${platform}.ppn`,
//   `../../resources/keyword_files/${platform}/bumblebee_${platform}.ppn`,
// ];
// const keywordPathsSingleDe = [
//   `../../resources/keyword_files_de/${platform}/heuschrecke_${platform}.ppn`,
// ];
// const keywordPathsMultipleDe = [
//   `../../resources/keyword_files_de/${platform}/ananas_${platform}.ppn`,
//   `../../resources/keyword_files_de/${platform}/heuschrecke_${platform}.ppn`,
//   `../../resources/keyword_files_de/${platform}/leguan_${platform}.ppn`,
//   `../../resources/keyword_files_de/${platform}/stachelschwein_${platform}.ppn`,
// ];
// const keywordPathsSingleEs = [
//   `../../resources/keyword_files_es/${platform}/manzana_${platform}.ppn`,
// ];
// const keywordPathsMultipleEs = [
//   `../../resources/keyword_files_es/${platform}/emparedado_${platform}.ppn`,
//   `../../resources/keyword_files_es/${platform}/leopardo_${platform}.ppn`,
//   `../../resources/keyword_files_es/${platform}/manzana_${platform}.ppn`,
// ];
// const keywordPathsSingleFr = [
//   `../../resources/keyword_files_fr/${platform}/mon chouchou_${platform}.ppn`,
// ];
// const keywordPathsMultipleFr = [
//   `../../resources/keyword_files_fr/${platform}/framboise_${platform}.ppn`,
//   `../../resources/keyword_files_fr/${platform}/mon chouchou_${platform}.ppn`,
//   `../../resources/keyword_files_fr/${platform}/parapluie_${platform}.ppn`,
// ];
// const keywordPathsMultipleIt = [
//   `../../resources/keyword_files_it/${platform}/cameriere_${platform}.ppn`,
//   `../../resources/keyword_files_it/${platform}/espresso_${platform}.ppn`,
//   `../../resources/keyword_files_it/${platform}/porcospino_${platform}.ppn`,
// ];
// const keywordPathsMultipleJa = [
//   `../../resources/keyword_files_ja/${platform}/bushi_${platform}.ppn`,
//   `../../resources/keyword_files_ja/${platform}/ninja_${platform}.ppn`,
//   `../../resources/keyword_files_ja/${platform}/ringo_${platform}.ppn`,
// ];
// const keywordPathsMultipleKo = [
//   `../../resources/keyword_files_ko/${platform}/aiseukeulim_${platform}.ppn`,
//   `../../resources/keyword_files_ko/${platform}/bigseubi_${platform}.ppn`,
//   `../../resources/keyword_files_ko/${platform}/koppulso_${platform}.ppn`,
// ];
// const keywordPathsMultiplePt = [
//   `../../resources/keyword_files_pt/${platform}/abacaxi_${platform}.ppn`,
//   `../../resources/keyword_files_pt/${platform}/fenomeno_${platform}.ppn`,
//   `../../resources/keyword_files_pt/${platform}/formiga_${platform}.ppn`,
// ];
// const keywordPathsNonAscii = [
//   `../../resources/keyword_files_es/${platform}/murciélago_${platform}.ppn`,
// ];

const ACCESS_KEY = process.argv.filter((x) => x.startsWith('--access_key='))[0]?.split('--access_key=')[1] ?? "";

// function porcupineDetectionCounts(engineInstance: Porcupine, relativeWaveFilePath: string): Map<number, number> {
//   const waveFilePath = path.join(__dirname, relativeWaveFilePath);
//   const waveBuffer = fs.readFileSync(waveFilePath);
//   const waveAudioFile = new WaveFile(waveBuffer);
//
//   if (!checkWaveFile(waveAudioFile, engineInstance.sampleRate)) {
//     fail(
//       "Audio file did not meet requirements. Wave file must be 16KHz, 16-bit, linear PCM (mono)."
//     );
//   }
//
//   const frames = getInt16Frames(waveAudioFile, engineInstance.frameLength);
//
//   // build a map of keyword indices to detection counts
//   const detections = new Map();
//
//   for (let i = 0; i < frames.length; i++) {
//     const frame = frames[i];
//     const keywordIndex = engineInstance.process(frame);
//
//     if (keywordIndex !== -1) {
//       const currentCount = detections.get(keywordIndex);
//
//       if (currentCount === undefined) {
//         detections.set(keywordIndex, 1);
//       } else {
//         detections.set(keywordIndex, currentCount + 1);
//       }
//     }
//   }
//   return detections;
// }

function testPorcupineDetection(
    language: string,
    keywords: string[] | BuiltinKeyword[],
    groundTruth: number[] | number,
    audioFileName: string | null = null): void {

    const keywordPaths = keywords.map((keyword) => {
            return getKeywordPathsByLanguage("../../../", language, keyword);
        }
    );

    const modelPath = getModelPathByLanguage("../../../", language);
    const engineInstance = new Porcupine(
        ACCESS_KEY,
        keywordPaths,
        Array(keywordPaths.length).fill(0.5),
        modelPath,
    );

    const waveFilePath = getAudioFileByLanguage("../../../", language, audioFileName);
    const waveBuffer = fs.readFileSync(waveFilePath);
    const waveAudioFile = new WaveFile(waveBuffer);

    if (!checkWaveFile(waveAudioFile, engineInstance.sampleRate)) {
        fail(
            "Audio file did not meet requirements. Wave file must be 16KHz, 16-bit, linear PCM (mono)."
        );
    }

    const frames = getInt16Frames(waveAudioFile, engineInstance.frameLength);

    const results = []
    for (let i = 0; i < frames.length; i++) {
        const frame = frames[i];
        const keywordIndex = engineInstance.process(frame);

        if (keywordIndex !== -1) {
            results.push(keywordIndex);
        }
    }

    if (Array.isArray(groundTruth)) {
        expect(results).toEqual(groundTruth);
    } else {
        expect(results.length).toBe(groundTruth);
    }

    engineInstance.release();
}

describe("successful keyword detections", () => {
        it.each(SINGLE_KEYWORD_PARAMETERS)(
            'testing single keyword for %p with %p', (language, keywords, result, filename) => {
                testPorcupineDetection(language, keywords, result, filename);
            });

        it.each(MULTIPLE_KEYWORDS_PARAMETERS)(
            'testing multiple for %p with %p', (language, keywords, result) => {
                testPorcupineDetection(language, keywords, result);
            });

        test("builtin keyword 'GRASSHOPPER'", () => {
            testPorcupineDetection("en", [BuiltinKeyword.GRASSHOPPER], 1);
        });

        test("single keyword multiple detection", () => {
            testPorcupineDetection("en", ['porcupine'], 2);
        });
    }
);

describe("Non ascii characters", () => {
    test("single non ascii character in the model name", () => {
        const porcupineEngine = new Porcupine(
            ACCESS_KEY,
            [getKeywordPathsByLanguage("../../../", 'es', 'murciélago')],
            [0.5],
            getModelPathByLanguage("../../../", 'es')
        );
    });
});

describe("basic parameter validation", () => {
    test("num of keywords does not match num of sensitivities", () => {
        expect(() => {
            new Porcupine(
                ACCESS_KEY,
                [getKeywordPathsByLanguage("../../../", 'en', 'porcupine')],
                [0.1, 0.2,]);
        }).toThrow(PorcupineInvalidArgumentError);
    });

    test("keywords is not an array", () => {
        expect(() => {
            new Porcupine(
                ACCESS_KEY,
                // @ts-expect-error
                getKeywordPathsByLanguage("../../../", 'en', 'porcupine'),
                [0.1]);
        }).toThrow(PorcupineInvalidArgumentError);
    });

    test("sensitivity is not in range", () => {
        expect(() => {
            new Porcupine(
                ACCESS_KEY,
                [getKeywordPathsByLanguage("../../../", 'en', 'porcupine')],
                [4.2]);
        }).toThrow(RangeError);
    });

    test("sensitivity is not a number", () => {
        expect(() => {
            new Porcupine(
                ACCESS_KEY,
                [getKeywordPathsByLanguage("../../../", 'en', 'porcupine')],
                // @ts-expect-error
                "porcupine");
        }).toThrow(RangeError);
    });

    test("invalid keyword path", () => {
        expect(() => {
            new Porcupine(
                ACCESS_KEY,
                ["to be or not to be"],
                [0.5]);
        }).toThrow(PorcupineInvalidArgumentError);
    });
});

describe("frame validation", () => {
    test("accepts non Int16Array if array is valid", () => {
        const porcupineEngine = new Porcupine(
            ACCESS_KEY,
            [getKeywordPathsByLanguage("../../../", 'en', 'porcupine')],
            [0.5]);
        const emptyArray = Array.apply(null, Array(porcupineEngine.frameLength)).map((x, i) => i)
        // @ts-expect-error
        porcupineEngine.process(emptyArray);
        porcupineEngine.release();
    });

    test("mismatched frameLength throws error", () => {
        const porcupineEngine = new Porcupine(
            ACCESS_KEY,
            [getKeywordPathsByLanguage("../../../", 'en', 'porcupine')],
            [0.5]);
        expect(() => {
            // @ts-expect-error
            porcupineEngine.process([1, 2, 3]);
        }).toThrow(PorcupineInvalidArgumentError);
        porcupineEngine.release();
    });

    test("null/undefined frames throws error", () => {
        const porcupineEngine = new Porcupine(
            ACCESS_KEY,
            [getKeywordPathsByLanguage("../../../", 'en', 'porcupine')],
            [0.5]);
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
        const porcupineEngine = new Porcupine(
            ACCESS_KEY,
            [getKeywordPathsByLanguage("../../../", 'en', 'porcupine')],
            [0.5]);
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