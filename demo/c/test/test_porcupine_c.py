#
# Copyright 2023 Picovoice Inc.
#
# You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
# file accompanying this source.
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
# an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
# specific language governing permissions and limitations under the License.
#

import os.path
import subprocess
import sys
import unittest

from parameterized import parameterized

from test_util import *

single_keyword_parameters = load_test_data()


class PorcupineCTestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls._access_key = sys.argv[1]
        cls._platform = sys.argv[2]
        cls._arch = "" if len(sys.argv) != 4 else sys.argv[3]
        cls._root_dir = os.path.join(os.path.dirname(__file__), "../../..")

    def _get_library_file(self):
        return os.path.join(
            self._root_dir,
            "lib",
            self._platform,
            self._arch,
            "libpv_porcupine." + get_lib_ext(self._platform)
        )

    def _get_model_path_by_language(self, language):
        model_path_subdir = append_language('lib/common/porcupine_params', language)
        return os.path.join(self._root_dir, '%s.pv' % model_path_subdir)

    def _get_keyword_path_by_language(self, language, keyword):
        keyword_files_root = append_language('resources/keyword_files', language)
        keyword_files_dir = os.path.join(
            self._root_dir,
            keyword_files_root,
            self._platform)

        return os.path.join(keyword_files_dir, "%s_%s.ppn" % (keyword, self._platform))

    def _get_audio_file_by_language(self, language, audio_file_name=None):
        if audio_file_name is None:
            audio_file_name = "%s.wav" % append_language('multiple_keywords', language)
        return os.path.join(self._root_dir, 'resources/audio_samples', audio_file_name)

    def run_porcupine(self, language, keyword, audio_file_name=None):
        args = [
            os.path.join(os.path.dirname(__file__), "../build/porcupine_demo_file"),
            "-a", self._access_key,
            "-l", self._get_library_file(),
            "-m", self._get_model_path_by_language(language),
            "-k", self._get_keyword_path_by_language(language, keyword),
            "-t", "0.5",
            "-w", self._get_audio_file_by_language(language, audio_file_name)
        ]
        process = subprocess.Popen(args, stderr=subprocess.PIPE, stdout=subprocess.PIPE)
        stdout, stderr = process.communicate()
        self.assertEqual(process.poll(), 0)
        self.assertEqual(stderr.decode('utf-8'), '')
        self.assertTrue("detected at" in stdout.decode('utf-8'))

    @parameterized.expand(single_keyword_parameters)
    def test_single_keyword(self, language, keyword, audio_file_name):
        self.run_porcupine(
            language=language,
            keyword=keyword,
            audio_file_name=audio_file_name)

    def test_with_non_ascii_model_name(self):
        self.run_porcupine(
            language='es',
            keyword='murciélago',
            audio_file_name='murciélago.wav')


if __name__ == '__main__':
    if len(sys.argv) < 3 or len(sys.argv) > 4:
        print("usage: test_porcupine_c.py ${AccessKey} ${Platform} [${Arch}]")
        exit(1)
    unittest.main(argv=sys.argv[:1])
