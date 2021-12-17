#
# Copyright 2018-2021 Picovoice Inc.
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

import soundfile

from porcupine import Porcupine
from util import *


class PorcupineTestCase(unittest.TestCase):
    @staticmethod
    def __append_language(s, language):
        if language == 'en':
            return s
        return f'{s}_{language}'

    @classmethod
    def __pv_model_path_by_language(cls, relative, language):
        model_path_subdir = cls.__append_language('lib/common/porcupine_params', language)
        return os.path.join(os.path.dirname(__file__), relative, f'{model_path_subdir}.pv')

    @classmethod
    def __pv_keyword_paths_by_language(cls, relative, language):
        keyword_files_root = cls.__append_language('resources/keyword_files', language)
        keyword_files_dir = \
            os.path.join(os.path.dirname(__file__), relative, keyword_files_root, pv_keyword_files_subdir())

        res = dict()
        for x in os.listdir(keyword_files_dir):
            res[x.rsplit('_')[0]] = os.path.join(keyword_files_dir, x)

        return res

    def run_porcupine(self, language, keywords, ground_truth, audio_file_name=None):
        if audio_file_name is None:
            _audio_file_name_prefix = self.__append_language('multiple_keywords', language)
            audio_file_name = f'{_audio_file_name_prefix}.wav'
        keyword_paths = list()
        for x in keywords:
            keyword_paths.append(self.__pv_keyword_paths_by_language('../..', language)[x])

        porcupine = Porcupine(
            access_key=sys.argv[1],
            library_path=pv_library_path('../..'),
            model_path=self.__pv_model_path_by_language('../..', language),
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

    def test_single_keyword(self):
        self.run_porcupine(
            language='en',
            keywords=['porcupine'],
            ground_truth=[0],
            audio_file_name='porcupine.wav')

    def test_multiple_keywords(self):
        keywords = [
            'americano', 'blueberry', 'bumblebee', 'grapefruit',
            'grasshopper', 'picovoice', 'porcupine', 'terminator'
        ]
        self.run_porcupine(
            language='en',
            keywords=keywords,
            ground_truth=[6, 0, 1, 2, 3, 4, 5, 6, 7])

    def test_single_keyword_es(self):
        self.run_porcupine(
            language='es',
            keywords=['manzana'],
            ground_truth=[0],
            audio_file_name='manzana.wav')

    def test_multiple_keywords_es(self):
        self.run_porcupine(
            language='es',
            keywords=['emparedado', 'leopardo', 'manzana'],
            ground_truth=[0, 1, 2])

    def test_single_keyword_de(self):
        self.run_porcupine(
            language='de',
            keywords=['heuschrecke'],
            ground_truth=[0],
            audio_file_name='heuschrecke.wav')

    def test_multiple_keywords_de(self):
        self.run_porcupine(
            language='de',
            keywords=['ananas', 'heuschrecke', 'leguan', 'stachelschwein'],
            ground_truth=[0, 1, 2, 3])

    def test_single_keyword_fr(self):
        self.run_porcupine(
            language='fr',
            keywords=['mon chouchou'],
            ground_truth=[0],
            audio_file_name='mon_chouchou.wav')

    def test_multiple_keywords_fr(self):
        self.run_porcupine(
            language='fr',
            keywords=['framboise', 'mon chouchou', 'parapluie'],
            ground_truth=[0, 1, 0, 2])


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("usage: test_porcupine.py ${AccessKey}")
        exit(1)

    unittest.main(argv=sys.argv[:1])
