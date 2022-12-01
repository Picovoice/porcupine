//
// Copyright 2022 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
// file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//

import * as fs from "fs";
import * as path from "path";
import { performance } from "perf_hooks";

import Porcupine from "../src/porcupine";
import { BuiltinKeyword, getInt16Frames } from "../src";
import { WaveFile } from "wavefile";

const WAV_PATH_MULTIPLE_KEYWORDS =
  "../../../resources/audio_samples/multiple_keywords.wav";

const ACCESS_KEY = process.argv.filter(x => x.startsWith('--access_key='))[0]?.split('--access_key=')[1] ?? "";
const NUM_TEST_ITERATIONS = Number(process.argv.filter(x => x.startsWith('--num_test_iterations='))[0]?.split('--num_test_iterations=')[1] ?? 0);
const PERFORMANCE_THRESHOLD_SEC = Number(process.argv.filter(x => x.startsWith('--performance_threshold_sec='))[0]?.split('--performance_threshold_sec=')[1] ?? 0);

describe("performance", () => {
  test("process", () => {
    let porcupineEngine = new Porcupine(
      ACCESS_KEY,
      [BuiltinKeyword.PORCUPINE],
      [0.5]
    );

    const waveFilePath = path.join(__dirname, WAV_PATH_MULTIPLE_KEYWORDS);
    const waveBuffer = fs.readFileSync(waveFilePath);
    const waveAudioFile = new WaveFile(waveBuffer);

    const frames = getInt16Frames(waveAudioFile, porcupineEngine.frameLength);

    let perfResults = [];
    for (let i = 0; i < NUM_TEST_ITERATIONS; i++) {
      let total = 0;
      for (let j = 0; j < frames.length; j++) {
        const before = performance.now();
        porcupineEngine.process(frames[j]);
        total += (performance.now() - before);
      }
      perfResults.push(total);
    }

    porcupineEngine.release();

    let avgPerfMs = perfResults.reduce((acc, a) => acc + a, 0) / NUM_TEST_ITERATIONS;
    let avgPerfSec = Number((avgPerfMs / 1000).toFixed(3));
    // eslint-disable-next-line no-console
    console.log("Average Performance: " + avgPerfSec);
    expect(avgPerfSec).toBeLessThanOrEqual(PERFORMANCE_THRESHOLD_SEC);
  });
});
