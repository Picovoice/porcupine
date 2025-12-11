#
# Copyright 2022-2025 Picovoice Inc.
#
# You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
# file accompanying this source.
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
# an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
# specific language governing permissions and limitations under the License.
#

import argparse
import sys
import time
import unittest

from _porcupine import Porcupine
from _util import pv_library_path
from test_util import *


class PorcupinePerformanceTestCase(unittest.TestCase):

    access_key: str
    device: str

    num_test_iterations: int
    performance_threshold_sec: float

    def test_performance(self):
        relative = '../../'
        language = 'en'
        keyword_paths = get_keyword_paths_by_language(relative, language, ['porcupine'])

        porcupine = Porcupine(
            access_key=self.access_key,
            library_path=pv_library_path(relative),
            model_path=get_model_path_by_language(relative, language),
            device=self.device,
            keyword_paths=keyword_paths,
            sensitivities=[0.5] * len(keyword_paths))

        audio = read_wav_file(
            get_audio_file_by_language(relative, language),
            porcupine.sample_rate)

        num_frames = len(audio) // porcupine.frame_length

        perf_results = []
        for _ in range(self.num_test_iterations):
            proc_time = 0
            for j in range(num_frames):
                frame = audio[j * porcupine.frame_length:(j + 1) * porcupine.frame_length]
                start = time.perf_counter()
                porcupine.process(frame)
                proc_time += time.perf_counter() - start
            perf_results.append(proc_time)

        porcupine.delete()

        avg_perf = sum(perf_results) / self.num_test_iterations
        print("Average performance: %s" % avg_perf)
        self.assertLess(avg_perf, self.performance_threshold_sec)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--access-key", required=True)
    parser.add_argument("--device", required=True)
    parser.add_argument("--num-test-iterations", type=int, required=True)
    parser.add_argument("--performance-threshold-sec", type=float, required=True)
    args = parser.parse_args()

    PorcupinePerformanceTestCase.access_key = args.access_key
    PorcupinePerformanceTestCase.device = args.device
    PorcupinePerformanceTestCase.num_test_iterations = args.num_test_iterations
    PorcupinePerformanceTestCase.performance_threshold_sec = args.performance_threshold_sec

    unittest.main(argv=sys.argv[:1])
