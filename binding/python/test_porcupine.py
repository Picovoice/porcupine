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
    _porcupine = None

    def setUp(self):
        self._porcupine = Porcupine(
            library_path=self._abs_path('../../lib/%s/%s/libpv_porcupine.%s' % (self._system(), self._machine(),
                                                                                self._dylib_extension())),
            model_file_path=self._abs_path('../../lib/common/porcupine_params.pv'),
            keyword_file_path=self._abs_path('../../resources/keyword_files/porcupine_%s.ppn' % (self._system())),
            sensitivity=0.5)

    def tearDown(self):
        self._porcupine.delete()

    def test_process(self):
        audio, sample_rate = soundfile.read(
            self._abs_path('../../resources/audio_samples/porcupine.wav'),
            dtype='int16')
        assert sample_rate == self._porcupine.sample_rate

        num_frames = len(audio) // self._porcupine.frame_length
        results = []
        for i in range(num_frames):
            frame = audio[i * self._porcupine.frame_length:(i + 1) * self._porcupine.frame_length]
            results.append(self._porcupine.process(frame))

        self.assertEqual(sum(results), 1)

    @staticmethod
    def _abs_path(rel_path):
        return os.path.join(os.path.dirname(__file__), rel_path)

    @staticmethod
    def _system():
        x = platform.system()
        if x == 'Linux':
            return 'linux'
        elif x == 'Darwin':
            return 'mac'
        else:
            raise NotImplementedError('Porcupine is not supported on %s yet!' % x)

    @staticmethod
    def _machine():
        x = platform.machine()
        if x == 'x86_64' or x == 'i386':
            return x
        else:
            raise NotImplementedError('Porcupine is not supported on %s yet!' % x)

    @staticmethod
    def _dylib_extension():
        s = platform.system()
        if s == 'Linux':
            return 'so'

        if s == 'Darwin':
            return 'dylib'

        raise NotImplementedError('Porcupine is not supported on %s yet!' % s)


if __name__ == '__main__':
    unittest.main()
