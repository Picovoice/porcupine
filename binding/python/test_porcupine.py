#
# Copyright 2018-2023 Picovoice Inc.
#
# You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
# file accompanying this source.
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
# an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
# specific language governing permissions and limitations under the License.
#

import sys
import unittest

from parameterized import parameterized

from _porcupine import Porcupine
from test_util import *


single_keyword_parameters, multiple_keywords_parameters = load_test_data()


class PorcupineTestCase(unittest.TestCase):

    def run_porcupine(self, language, keywords, ground_truth, audio_file_name=None):
        relative_path = '../..'

        keyword_paths = get_keyword_paths_by_language(relative_path, language, keywords)

        porcupine = Porcupine(
            access_key=sys.argv[1],
            library_path=pv_library_path(relative_path),
            model_path=get_model_path_by_language(relative_path, language),
            keyword_paths=keyword_paths,
            sensitivities=[0.5] * len(keyword_paths))

        audio_file = get_audio_file_by_language(relative_path, language, audio_file_name)
        audio = read_wav_file(
            audio_file,
            porcupine.sample_rate)

        num_frames = len(audio) // porcupine.frame_length
        results = []
        for i in range(num_frames):
            frame = audio[i * porcupine.frame_length:(i + 1) * porcupine.frame_length]
            result = porcupine.process(frame)
            if result >= 0:
                results.append(result)

        porcupine.delete()

        self.assertEqual(results, ground_truth)

    @parameterized.expand(single_keyword_parameters)
    def test_single_keyword(self, language, keywords, audio_file_name):
        self.run_porcupine(
            language=language,
            keywords=keywords,
            ground_truth=[0],
            audio_file_name=audio_file_name)

    @parameterized.expand(multiple_keywords_parameters)
    def test_multiple_keywords(self, language, keywords, ground_truth):
        self.run_porcupine(
            language=language,
            keywords=keywords,
            ground_truth=ground_truth)

    def test_with_non_ascii_model_name(self):
        self.run_porcupine(
            language='es',
            keywords=['murciélago'],
            ground_truth=[0, 0],
            audio_file_name='murciélago.wav')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("usage: test_porcupine.py ${AccessKey}")
        exit(1)

    unittest.main(argv=sys.argv[:1])
