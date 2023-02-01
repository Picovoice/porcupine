//
// Copyright 2020-2023 Picovoice Inc.
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
import { checkWaveFile, getInt16Frames } from "../src/wave_util";
import { WaveFile } from "wavefile";

import { PorcupineInvalidArgumentError } from "../src/errors";
import { BuiltinKeyword } from "../src/builtin_keywords";
import {
  getAudioFileByLanguage,
  getKeywordPathsByLanguage,
  getModelPathByLanguage,
  getMultipleKeywordParameters,
  getSingleKeywordParameters
} from "./test_utils";

const SINGLE_KEYWORD_PARAMETERS = getSingleKeywordParameters();
const MULTIPLE_KEYWORDS_PARAMETERS = getMultipleKeywordParameters();

const ACCESS_KEY = process.argv.filter(x => x.startsWith('--access_key='))[0]?.split('--access_key=')[1] ?? "";


function testPorcupineDetection(
  language: string,
  keywords: string[] | BuiltinKeyword[],
  groundTruth: number[] | number,
  audioFileName: string | null = null): void {
  const keywordPaths = keywords.map(keyword => getKeywordPathsByLanguage(language, keyword)
  );

  const modelPath = getModelPathByLanguage(language);
  const engineInstance = new Porcupine(
    ACCESS_KEY,
    keywordPaths,
    Array(keywordPaths.length).fill(0.5),
    modelPath,
  );

  const waveFilePath = getAudioFileByLanguage(language, audioFileName);
  const waveBuffer = fs.readFileSync(waveFilePath);
  const waveAudioFile = new WaveFile(waveBuffer);

  if (!checkWaveFile(waveAudioFile, engineInstance.sampleRate)) {
    fail(
      "Audio file did not meet requirements. Wave file must be 16KHz, 16-bit, linear PCM (mono)."
    );
  }

  const frames = getInt16Frames(waveAudioFile, engineInstance.frameLength);

  for (let iteration = 0; iteration < 2; iteration++) {
    const results = [];
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
  }

  engineInstance.release();
}

describe("successful keyword detections", () => {
  it.each(SINGLE_KEYWORD_PARAMETERS)(
    'testing single keyword for %p with %p', (language: string, keyword: string, filename: string) => {
      testPorcupineDetection(language, [keyword], [0], filename);
    });

  it.each(MULTIPLE_KEYWORDS_PARAMETERS)(
    'testing multiple for %p with %p', (language: string, keywords: string[], result: number[]) => {
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
    new Porcupine(
      ACCESS_KEY,
      [getKeywordPathsByLanguage('es', 'murciélago')],
      [0.5],
      getModelPathByLanguage('es')
    );
  });
});

describe("basic parameter validation", () => {
  test("num of keywords does not match num of sensitivities", () => {
    expect(() => {
      new Porcupine(
        ACCESS_KEY,
        [getKeywordPathsByLanguage('en', 'porcupine')],
        [0.1, 0.2,]);
    }).toThrow(PorcupineInvalidArgumentError);
  });

  test("keywords is not an array", () => {
    expect(() => {
      new Porcupine(
        ACCESS_KEY,
        // @ts-expect-error
        getKeywordPathsByLanguage('en', 'porcupine'),
        [0.1]);
    }).toThrow(PorcupineInvalidArgumentError);
  });

  test("sensitivity is not in range", () => {
    expect(() => {
      new Porcupine(
        ACCESS_KEY,
        [getKeywordPathsByLanguage('en', 'porcupine')],
        [4.2]);
    }).toThrow(RangeError);
  });

  test("sensitivity is not a number", () => {
    expect(() => {
      new Porcupine(
        ACCESS_KEY,
        [getKeywordPathsByLanguage('en', 'porcupine')],
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
      [getKeywordPathsByLanguage('en', 'porcupine')],
      [0.5]);
    const emptyArray = Array.apply(null, Array(porcupineEngine.frameLength)).map((x, i) => i);
    // @ts-expect-error
    porcupineEngine.process(emptyArray);
    porcupineEngine.release();
  });

  test("mismatched frameLength throws error", () => {
    const porcupineEngine = new Porcupine(
      ACCESS_KEY,
      [getKeywordPathsByLanguage('en', 'porcupine')],
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
      [getKeywordPathsByLanguage('en', 'porcupine')],
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
      [getKeywordPathsByLanguage('en', 'porcupine')],
      [0.5]);
    const floatFrames = Array.from({ length: porcupineEngine.frameLength }).map(
      () => 3.1415
    );
    expect(() => {
      // @ts-expect-error
      porcupineEngine.process(floatFrames);
    }).toThrow(PorcupineInvalidArgumentError);
    porcupineEngine.release();
  });
});
