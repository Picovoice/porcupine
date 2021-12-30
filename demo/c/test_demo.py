#
# Copyright 2021 Picovoice Inc.
#
# You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
# file accompanying this source.
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
# an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
# specific language governing permissions and limitations under the License.
#

import logging
import os
import platform
import subprocess
import sys
import unittest

from pvporcupine import util

class PorcupineDemoTestCase(unittest.TestCase):
    @staticmethod
    def __has_detected(s):
        return "detected" in s

    @staticmethod
    def __append_language(s, language):
        if language == 'en':
            return s
        return f'{s}_{language}'

    @classmethod
    def setUpClass(cls):
        cls.__ENVIRONMENT = cls.__get_environemnt()

    def setUp(self):
        self.command = ["./build/porcupine_demo_file",
                    "-l", util.pv_library_path('.'),
                    "-m", util.pv_model_path('.'),
                    "-k", "",
                    "-t", "0.5",
                    "-w", "",
                    "-a", sys.argv[1]]

    def test_americano(self):
        self.command[6] = self.__pv_keyword_path("en", "americano")
        self.command[10] = self.__pv_audio_path("multiple_keywords.wav")

        run_demo = subprocess.run(self.command, capture_output=True, text=True)

        self.assertEqual(run_demo.returncode, 0)
        self.assertTrue(self.__has_detected(run_demo.stdout))

    def test_heuschrecke_de(self):
        language = "de"
        self.command[4] = self.__pv_model_path(language)
        self.command[6] = self.__pv_keyword_path(language, "heuschrecke")
        self.command[10] = self.__pv_audio_path("heuschrecke.wav")

        run_demo = subprocess.run(self.command, capture_output=True, text=True)

        self.assertEqual(run_demo.returncode, 0)
        self.assertTrue(self.__has_detected(run_demo.stdout))

    def test_manzana_es(self):
        language = "es"
        self.command[4] = self.__pv_model_path(language)
        self.command[6] = self.__pv_keyword_path(language, "manzana")
        self.command[10] = self.__pv_audio_path("manzana.wav")

        run_demo = subprocess.run(self.command, capture_output=True, text=True)

        self.assertEqual(run_demo.returncode, 0)
        self.assertTrue(self.__has_detected(run_demo.stdout))

    def test_mon_chouchou_fr(self):
        language = "fr"
        self.command[4] = self.__pv_model_path(language)
        self.command[6] = self.__pv_keyword_path(language, "mon chouchou")
        self.command[10] = self.__pv_audio_path("mon_chouchou.wav")

        run_demo = subprocess.run(self.command, capture_output=True, text=True)

        self.assertEqual(run_demo.returncode, 0)
        self.assertTrue(self.__has_detected(run_demo.stdout))

    @staticmethod
    def __get_environemnt():
        system = platform.system()
        if system == 'Darwin':
            return 'mac'
        elif system == 'Windows':
            return 'windows'
        elif system == 'Linux':
            if platform.machine() == 'x86_64':
                return 'linux'
            else:
                cpu_info = ''
                try:
                    cpu_info = subprocess.check_output(['cat', '/proc/cpuinfo']).decode()
                    cpu_part_list = [x for x in cpu_info.split('\n') if 'CPU part' in x]
                    cpu_part = cpu_part_list[0].split(' ')[-1].lower()
                except Exception as error:
                    raise RuntimeError("Failed to identify the CPU with '%s'\nCPU info: %s" % (error, cpu_info))

                if '0xb76' == cpu_part or '0xc07' == cpu_part or '0xd03' == cpu_part or '0xd08' == cpu_part:
                    return 'raspberry-pi'
                elif '0xd07' == cpu_part:
                    return 'jetson'
                elif '0xc08' == cpu_part:
                    return 'beaglebone'
                else:
                    raise NotImplementedError("Unsupported CPU: '%s'." % cpu_part)
        else:
            raise ValueError("Unsupported system '%s'." % system)

    @classmethod
    def __pv_model_path(cls, language):
        model_file = cls.__append_language('porcupine_params', language)
        return os.path.join(
            os.path.dirname(__file__),
            f'../../lib/common/{model_file}.pv')

    @classmethod
    def __pv_keyword_path(cls, language, keyword):
        keyword_files = cls.__append_language('keyword_files', language)
        return os.path.join(
            os.path.dirname(__file__),
            f'../../resources/{keyword_files}/{cls.__ENVIRONMENT}/{keyword}_{cls.__ENVIRONMENT}.ppn')

    @classmethod
    def __pv_audio_path(cls, audio_file_name):
        return os.path.join(
            os.path.dirname(__file__),
            f'../../resources/audio_samples/{audio_file_name}')      


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("usage: test_porcupine.py ${AccessKey}")
        exit(1)

    unittest.main(argv=sys.argv[:1])
