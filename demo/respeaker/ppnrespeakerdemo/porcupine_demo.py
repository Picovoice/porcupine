#
# Copyright 2020 Picovoice Inc.
#
# You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
# file accompanying this source.
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
# an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
# specific language governing permissions and limitations under the License.
#

import struct
import sys
from threading import Thread

import pvporcupine
import pyaudio
from gpiozero import LED

from .apa102 import APA102

COLORS_RGB = dict(
    blue=(0, 0, 255),
    green=(0, 255, 0),
    orange=(255, 128, 0),
    pink=(255, 51, 153),
    purple=(128, 0, 128),
    red=(255, 0, 0),
    white=(255, 255, 255),
    yellow=(255, 255, 51),
    off=(0, 0, 0),
)

KEYWORDS_COLOR = {
    'alexa': 'yellow',
    'computer': 'white',
    'hey google': 'red',
    'hey siri': 'purple',
    'jarvis': 'pink',
    'picovoice': 'green',
    'porcupine': 'blue',
    'bumblebee': 'orange',
    'terminator': 'off',
}

driver = APA102(num_led=12)
power = LED(5)
power.on()


class PorcupineDemo(Thread):
    def __init__(self, sensitivity):
        super(PorcupineDemo, self).__init__()

        self._keywords = list(KEYWORDS_COLOR.keys())
        self._porcupine = pvporcupine.create(keywords=self._keywords, sensitivities=[sensitivity] * len(KEYWORDS_COLOR))

    @staticmethod
    def _set_color(color):
        for i in range(12):
            driver.set_pixel(i, color[0], color[1], color[2])
        driver.show()

    def run(self):
        pa = None
        audio_stream = None

        try:
            pa = pyaudio.PyAudio()

            audio_stream = pa.open(
                rate=self._porcupine.sample_rate,
                channels=1,
                format=pyaudio.paInt16,
                input=True,
                frames_per_buffer=self._porcupine.frame_length)

            print('[Listening ...]')

            while True:
                pcm = audio_stream.read(self._porcupine.frame_length)
                pcm = struct.unpack_from("h" * self._porcupine.frame_length, pcm)

                keyword_index = self._porcupine.process(pcm)
                print("detected '%s'" % self._keywords[keyword_index])
                if keyword_index >= 0:
                    self._set_color(COLORS_RGB[KEYWORDS_COLOR[self._keywords[keyword_index]]])
        except KeyboardInterrupt:
            sys.stdout.write('\b' * 2)
            print('Stopping ...')
        finally:
            if audio_stream is not None:
                audio_stream.close()

            if pa is not None:
                pa.terminate()

            self._porcupine.delete()


def main():
    o = PorcupineDemo(sensitivity=0.75)
    o.run()


if __name__ == '__main__':
    main()
