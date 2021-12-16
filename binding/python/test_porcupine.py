#
# Copyright 2018-2020 Picovoice Inc.
#
# You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
# file accompanying this source.
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
# an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
# specific languageuage governing permissions and limitations under the License.
#

import sys
import unittest

import soundfile
from porcupine import Porcupine
from test_utils import *


class PorcupineTestCase(unittest.TestCase):
    def run_porcupine(self, language, audio_file_name, keywords, ground_truth) :
        keyword_paths = list()
        for x in keywords:
            keyword_paths.append(pv_keyword_paths('../..', language)[x])

        porcupine = Porcupine(
            access_key=sys.argv[1],
            library_path=pv_library_path('../..'),
            model_path=pv_model_path('../..', language),
            keyword_paths=keyword_paths,
            sensitivities=[0.5] * len(keyword_paths))

        audio, sample_rate = soundfile.read(
            os.path.join(os.path.dirname(__file__), '../../resources/audio_samples/', audio_file_name),
            dtype='int16')
        assert sample_rate == porcupine.sample_rate

        num_frames = len(audio) // porcupine.frame_length
        results = []
        for i in range(num_frames):
            frame = audio[i * porcupine.frame_length:(i + 1) * porcupine.frame_length]
            result = porcupine.process(frame)
            if result >= 0:
                results.append(result)

        porcupine.delete()

        self.assertEqual(results, ground_truth)   

    def test_en_0(self):
        self.run_porcupine(language = 'en',
            audio_file_name = 'porcupine.wav',
            keywords = ['porcupine'],
            ground_truth = [0])

    def test_en_1(self):
        self.run_porcupine(language = 'en',
            audio_file_name = 'multiple_keywords.wav',
            keywords = ['americano', 'blueberry', 'bumblebee', 'grapefruit', 'grasshopper', 'picovoice', 'porcupine', 'terminator'],
            ground_truth = [6, 0, 1, 2, 3, 4, 5, 6, 7])

    def test_es_0(self):
        self.run_porcupine(language = 'es',
            audio_file_name = 'multiple_keywords_es.wav',
            keywords = ['emparedado', 'leopardo', 'manzana'],
            ground_truth = [0, 1, 2])           

    def test_de_0(self):
        self.run_porcupine(language = 'de',
            audio_file_name = 'multiple_keywords_de.wav',
            keywords = ['ananas', 'heuschrecke', 'leguan', 'stachelschwein'],
            ground_truth = [0, 1, 2, 3])

    def test_fr_0(self):
        self.run_porcupine(language = 'fr',
            audio_file_name = 'multiple_keywords_fr.wav',
            keywords = ['framboise', 'mon chouchou', 'parapluie'],
            ground_truth = [0, 1, 0, 2])


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("usage: test_porcupine.py ${AccessKey}")
        exit(1)

    unittest.main(argv=sys.argv[:1])
