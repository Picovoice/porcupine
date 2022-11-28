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

import argparse
import struct
import sys
from threading import Thread

import pvporcupine
from pvrecorder import PvRecorder
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
    def __init__(self, access_key, device_index, sensitivity):
        super(PorcupineDemo, self).__init__()

        self._device_index = device_index
        self._keywords = list(KEYWORDS_COLOR.keys())
        self._porcupine = pvporcupine.create(
            access_key=access_key,
            keywords=self._keywords,
            sensitivities=[sensitivity] * len(KEYWORDS_COLOR))

    @staticmethod
    def _set_color(color):
        for i in range(12):
            driver.set_pixel(i, color[0], color[1], color[2])
        driver.show()

    def run(self):
        recorder = None

        try:
            recorder = PvRecorder(device_index=self._device_index, frame_length=self._porcupine.frame_length)
            recorder.start()

            print('[Listening ...]')

            while True:
                pcm = recorder.read()
                keyword_index = self._porcupine.process(pcm)
                if keyword_index >= 0:
                    print("detected '%s'" % self._keywords[keyword_index])
                    self._set_color(COLORS_RGB[KEYWORDS_COLOR[self._keywords[keyword_index]]])
        except KeyboardInterrupt:
            sys.stdout.write('\b' * 2)
            print('Stopping ...')
        finally:
            if recorder is not None:
                recorder.delete()

            self._porcupine.delete()


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument('--access_key',
                        help='AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)',
                        required=True)

    parser.add_argument('--audio_device_index', help='Index of input audio device.', type=int, default=-1)

    args = parser.parse_args()

    o = PorcupineDemo(access_key=args.access_key,
                      device_index=args.audio_device_index,
                      sensitivity=0.6)
    o.run()


if __name__ == '__main__':
    main()
