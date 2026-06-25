#
# Copyright 2026 Picovoice Inc.
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
import uuid

from _porcupine import Porcupine
from test_util import *
from _util import *


class PorcupineTrainTestCase(unittest.TestCase):

    ACCESS_KEY = sys.argv[1]

    def test_train_model(self):
        relative_path = '../..'

        for platform in ['mac', None]:
            output_path = f'{str(uuid.uuid4())}.rhn'
            pv_train_model(
                sys.argv[1],
                output_path,
                'es',
                'prueba',
                platform=platform)

            self.assertTrue(os.path.exists(output_path))

            if platform is None:
                porcupine = Porcupine(
                    access_key=sys.argv[1],
                    library_path=pv_library_path(relative_path),
                    model_path=get_model_path_by_language(relative_path, 'es'),
                    device="cpu:1",
                    keyword_paths=[output_path],
                    sensitivities=[0.5])
                porcupine.delete()

            if os.path.exists(output_path):
                os.remove(output_path)

    def test_train_model_invalid_phrase(self):
        output_path = f'{str(uuid.uuid4())}.rhn'

        with self.assertRaises(RuntimeError):
            pv_train_model(
                sys.argv[1],
                output_path,
                'es',
                'ㅁㄴㅇㄹ')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("usage: test_porcupine_train.py ${ACCESS_KEY}")
        exit(1)

    unittest.main(argv=sys.argv[:1])
