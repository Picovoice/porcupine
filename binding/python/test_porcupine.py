#
# Copyright 2018 Picovoice Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import os
import platform
import unittest

import soundfile
from porcupine import Porcupine


class PorcupineTestCase(unittest.TestCase):
    def test_process(self):
        extension = self._keyword_file_extension()
        keyword_file_path = self._abs_path('../../resources/keyword_files/%s/porcupine_%s.ppn' % (extension, extension))

        porcupine = Porcupine(
            library_path=self._library_path(),
            model_file_path=self._abs_path('../../lib/common/porcupine_params.pv'),
            keyword_file_path=keyword_file_path,
            sensitivity=0.5)

        audio, sample_rate = soundfile.read(
            self._abs_path('../../resources/audio_samples/porcupine.wav'),
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
        keyword_file_names = [
            'alexa', 'americano', 'blueberry', 'bumblebee', 'grapefruit', 'grasshopper', 'hey pico', 'picovoice',
            'porcupine', 'terminator'
        ]

        extension = self._keyword_file_extension()
        keyword_file_paths = list()
        for name in keyword_file_names:
            keyword_file_paths.append(
                self._abs_path('../../resources/keyword_files/%s/%s_%s.ppn' % (extension, name, extension)))

        porcupine = Porcupine(
            library_path=self._library_path(),
            model_file_path=self._abs_path('../../lib/common/porcupine_params.pv'),
            keyword_file_paths=keyword_file_paths,
            sensitivities=[0.5] * len(keyword_file_paths))

        audio, sample_rate = soundfile.read(
            self._abs_path('../../resources/audio_samples/multiple_keywords.wav'),
            dtype='int16')
        assert sample_rate == porcupine.sample_rate

        num_frames = len(audio) // porcupine.frame_length
        results = []
        for i in range(num_frames):
            frame = audio[i * porcupine.frame_length:(i + 1) * porcupine.frame_length]
            result = porcupine.process(frame)
            if result >= 0:
                results.append(result)

        self.assertEqual(results, [8, 0, 1, 2, 3, 4, 5, 7, 8, 9])

        porcupine.delete()

    @staticmethod
    def _abs_path(rel_path):
        return os.path.join(os.path.dirname(__file__), rel_path)

    @staticmethod
    def _keyword_file_extension():
        system = platform.system()
        machine = platform.machine()

        if system == 'Linux' and machine == 'x86_64':
            return 'linux'
        elif system == 'Darwin':
            return 'mac'
        elif system == 'Linux' and machine.startswith('arm'):
            return 'raspberrypi'
        elif system == 'Windows':
            return 'windows'

        raise NotImplementedError('Porcupine is not supported on %s/%s yet!' % (system, machine))

    @staticmethod
    def _library_path():
        system = platform.system()
        machine = platform.machine()

        if system == 'Darwin':
            return os.path.join(os.path.dirname(__file__), '../../lib/mac/%s/libpv_porcupine.dylib' % machine)
        elif system == 'Linux':
            if machine == 'x86_64':
                return os.path.join(os.path.dirname(__file__), '../../lib/linux/%s/libpv_porcupine.so' % machine)
            elif machine.startswith('arm'):
                # NOTE: This does not need to be fast. Use the armv6 binary.
                return os.path.join(os.path.dirname(__file__), '../../lib/raspberry-pi/arm11/libpv_porcupine.so')
        elif system == 'Windows':
            return os.path.join(os.path.dirname(__file__), '..\\..\\lib\\windows\\amd64\\libpv_porcupine.dll')

        raise NotImplementedError('Porcupine is not supported on %s/%s yet!' % (system, machine))


if __name__ == '__main__':
    unittest.main()
