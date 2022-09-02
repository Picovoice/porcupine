#
# Copyright 2020-2022 Picovoice Inc.
#
# You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
# file accompanying this source.
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
# an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
# specific language governing permissions and limitations under the License.
#

import argparse
import tkinter as tk
from threading import Thread

import pvporcupine
from pvrecorder import PvRecorder

KEYWORDS = [
    'alexa', 'americano', 'blueberry', 'bumblebee', 'computer', 'grapefruit', 'grasshopper', 'hey google', 'hey siri',
    'jarvis', 'picovoice', 'porcupine'
]


class PorcupineThread(Thread):
    def __init__(self, access_key, device_index, keyword_var):
        super().__init__()

        self._access_key = access_key
        self._device_index = device_index
        self._keyword_var = keyword_var

        self._is_ready = False
        self._stop = False
        self._is_stopped = False

    def run(self):
        ppn = None
        recorder = None

        try:
            ppn = \
                pvporcupine.create(access_key=self._access_key, keywords=KEYWORDS, sensitivities=[0.75] * len(KEYWORDS))

            recorder = PvRecorder(device_index=self._device_index, frame_length=ppn.frame_length)
            recorder.start()

            self._is_ready = True

            while not self._stop:
                pcm = recorder.read()
                keyword_index = ppn.process(pcm)
                if keyword_index >= 0:
                    self._keyword_var.set(KEYWORDS[keyword_index])
                    print(self._keyword_var.get())
        finally:
            if recorder is not None:
                recorder.delete()

            if ppn is not None:
                ppn.delete()

        self._is_stopped = True

    def is_ready(self):
        return self._is_ready

    def stop(self):
        self._stop = True

    def is_stopped(self):
        return self._is_stopped


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument('--access_key',
                        help='AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)',
                        required=True)

    parser.add_argument('--audio_device_index', help='Index of input audio device.', type=int, default=-1)

    args = parser.parse_args()

    window = tk.Tk()
    window.title('Porcupine Demo')
    window.minsize(width=300, height=400)

    keyword_var = tk.StringVar(window)

    for x in KEYWORDS:
        tk.Radiobutton(window, text=x, variable=keyword_var, value=x, indicatoron=False).pack(fill=tk.X, ipady=5)

    porcupine_thread = PorcupineThread(access_key=args.access_key,
                                       device_index=args.audio_device_index,
                                       keyword_var=keyword_var)

    def on_close():
        porcupine_thread.stop()
        while not porcupine_thread.is_stopped():
            pass
        window.destroy()

    window.protocol('WM_DELETE_WINDOW', on_close)

    porcupine_thread.start()
    while not porcupine_thread.is_ready():
        pass

    window.mainloop()


if __name__ == '__main__':
    main()
