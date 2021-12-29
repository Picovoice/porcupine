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
        cls.__RASPBERRY_PI_MACHINES = {'arm11', 'cortex-a7', 'cortex-a53', 'cortex-a72', 'cortex-a53-aarch64', 'cortex-a72-aarch64'}
        cls.__JETSON_MACHINES = {'cortex-a57-aarch64'}  
        cls.__PV_SYSTEM, cls.__PV_MACHINE = cls.__pv_platform()
        cls.__ENVIRONMENT = cls.__get_environemnt()

    def setUp(self):
        self.command = ["./build/porcupine_demo_file",
                    "-l", self.__pv_library_path(),
                    "-m", self.__pv_model_path("en"),
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

    @classmethod
    def __pv_linux_machine(cls, machine):
        if machine == 'x86_64':
            return machine
        elif machine == 'aarch64':
            arch_info = '-' + machine
        elif machine in ['armv7l', 'armv6l']:
            arch_info = ''
        else:
            raise NotImplementedError("Unsupported CPU architecture: '%s'" % machine)

        cpu_info = ''
        try:
            cpu_info = subprocess.check_output(['cat', '/proc/cpuinfo']).decode()
            cpu_part_list = [x for x in cpu_info.split('\n') if 'CPU part' in x]
            cpu_part = cpu_part_list[0].split(' ')[-1].lower()
        except Exception as error:
            raise RuntimeError("Failed to identify the CPU with '%s'\nCPU info: %s" % (error, cpu_info))

        if '0xb76' == cpu_part:
            return 'arm11' + arch_info
        elif '0xc07' == cpu_part:
            return 'cortex-a7' + arch_info
        elif '0xd03' == cpu_part:
            return 'cortex-a53' + arch_info
        elif '0xd07' == cpu_part:
            return 'cortex-a57' + arch_info
        elif '0xd08' == cpu_part:
            return 'cortex-a72' + arch_info
        elif '0xc08' == cpu_part:
            return 'beaglebone' + arch_info
        elif machine == 'armv7l':
            log.warning(
                'WARNING: Please be advised that this device (CPU part = %s) is not officially supported by Picovoice. '
                'Falling back to the armv6-based (Raspberry Pi Zero) library. This is not tested nor optimal.' % cpu_part)
            return 'arm11'
        else:
            raise NotImplementedError("Unsupported CPU: '%s'." % cpu_part)

    @classmethod
    def __pv_platform(cls):
        pv_system = platform.system()
        if pv_system not in {'Darwin', 'Linux', 'Windows'}:
            raise ValueError("Unsupported system '%s'." % pv_system)

        if pv_system == 'Linux':
            pv_machine = cls.__pv_linux_machine(platform.machine())
        else:
            pv_machine = platform.machine()

        return pv_system, pv_machine

    @classmethod
    def __pv_library_path(cls):
        _PV_SYSTEM, _PV_MACHINE = cls.__PV_SYSTEM, cls.__PV_MACHINE
        _RASPBERRY_PI_MACHINES, _JETSON_MACHINES = cls.__RASPBERRY_PI_MACHINES, cls.__JETSON_MACHINES        
        relative = "../.."

        if _PV_SYSTEM == 'Darwin':
            if _PV_MACHINE == 'x86_64':
                return os.path.join(os.path.dirname(__file__), relative, 'lib/mac/x86_64/libpv_porcupine.dylib')
            elif _PV_MACHINE == "arm64":
                return os.path.join(os.path.dirname(__file__), relative, 'lib/mac/arm64/libpv_porcupine.dylib')
        elif _PV_SYSTEM == 'Linux':
            if _PV_MACHINE == 'x86_64':
                return os.path.join(os.path.dirname(__file__), relative, 'lib/linux/x86_64/libpv_porcupine.so')
            elif _PV_MACHINE in _JETSON_MACHINES:
                return os.path.join(
                    os.path.dirname(__file__),
                    relative,
                    'lib/jetson/%s/libpv_porcupine.so' % _PV_MACHINE)
            elif _PV_MACHINE in _RASPBERRY_PI_MACHINES:
                return os.path.join(
                    os.path.dirname(__file__),
                    relative,
                    'lib/raspberry-pi/%s/libpv_porcupine.so' % _PV_MACHINE)
            elif _PV_MACHINE == 'beaglebone':
                return os.path.join(os.path.dirname(__file__), relative, 'lib/beaglebone/libpv_porcupine.so')
        elif _PV_SYSTEM == 'Windows':
            return os.path.join(os.path.dirname(__file__), relative, 'lib/windows/amd64/libpv_porcupine.dll')

    @classmethod
    def __get_environemnt(cls):
        system = cls.__PV_SYSTEM
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
