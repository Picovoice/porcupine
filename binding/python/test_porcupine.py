#
# Copyright 2018 Picovoice Inc.
#
# You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
# file accompanying this source.
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
# an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
# specific language governing permissions and limitations under the License.
#

import os
import sys
import unittest

import soundfile
from porcupine import Porcupine


class PorcupineTestCase(unittest.TestCase):
    def test_process(self):
        porcupine = Porcupine(
            library_path=LIBRARY_PATH,
            model_file_path=MODEL_FILE_PATH,
            keyword_file_path=KEYWORD_FILE_PATHS['porcupine'],
            sensitivity=0.5)

        audio, sample_rate = soundfile.read(
            os.path.join(os.path.dirname(__file__), '../../resources/audio_samples/porcupine.wav'),
            dtype='int16')
        assert sample_rate == porcupine.sample_rate

        num_frames = len(audio) // porcupine.frame_length
        results = []
        for i in range(num_frames):
            frame = audio[i * porcupine.frame_length:(i + 1) * porcupine.frame_length]
            results.append(porcupine.process(frame))

        porcupine.delete()

        self.assertEqual(sum(results), 1)

    def test_process_multiple(self):
        keyword_file_names = \
            ['americano', 'blueberry', 'bumblebee', 'grapefruit', 'grasshopper', 'picovoice', 'porcupine', 'terminator']

        keyword_file_paths = list()
        for name in keyword_file_names:
            keyword_file_paths.append(KEYWORD_FILE_PATHS[name])

        porcupine = Porcupine(
            library_path=LIBRARY_PATH,
            model_file_path=MODEL_FILE_PATH,
            keyword_file_paths=keyword_file_paths,
            sensitivities=[0.5] * len(keyword_file_paths))

        audio, sample_rate = soundfile.read(
            os.path.join(os.path.dirname(__file__), '../../resources/audio_samples/multiple_keywords.wav'),
            dtype='int16')
        assert sample_rate == porcupine.sample_rate

        num_frames = len(audio) // porcupine.frame_length
        results = []
        for i in range(num_frames):
            frame = audio[i * porcupine.frame_length:(i + 1) * porcupine.frame_length]
            result = porcupine.process(frame)
            if result >= 0:
                results.append(result)

        self.assertEqual(results, [6, 0, 1, 2, 3, 4, 5, 6, 7])

        porcupine.delete()


if __name__ == '__main__':
    sys.path.append(os.path.join(os.path.dirname(__file__), '../../resources/util/python'))
    from util import *

    unittest.main()
