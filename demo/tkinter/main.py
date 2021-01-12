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
import tkinter as tk
from threading import Thread

import pvporcupine
import pyaudio

KEYWORDS = [
    'alexa', 'americano', 'blueberry', 'bumblebee', 'computer', 'grapefruit', 'grasshopper', 'hey google', 'hey siri',
    'jarvis', 'picovoice', 'porcupine'
]


class PorcupineThread(Thread):
    def __init__(self, keyword_var):
        super().__init__()

        self._keyword_var = keyword_var

        self._is_ready = False
        self._stop = False
        self._is_stopped = False

    def run(self):
        ppn = None
        py_audio = None
        audio_stream = None

        try:
            ppn = pvporcupine.create(keywords=KEYWORDS, sensitivities=[0.75] * len(KEYWORDS))

            py_audio = pyaudio.PyAudio()
            audio_stream = py_audio.open(
                rate=ppn.sample_rate,
                channels=1,
                format=pyaudio.paInt16,
                input=True,
                frames_per_buffer=ppn.frame_length)

            self._is_ready = True

            while not self._stop:
                pcm = audio_stream.read(ppn.frame_length)
                pcm = struct.unpack_from("h" * ppn.frame_length, pcm)
                keyword_index = ppn.process(pcm)
                if keyword_index >= 0:
                    self._keyword_var.set(KEYWORDS[keyword_index])
                    print(self._keyword_var.get())
        finally:
            if audio_stream is not None:
                audio_stream.close()
            if py_audio is not None:
                py_audio.terminate()

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
    window = tk.Tk()
    window.title('Porcupine Demo')
    window.minsize(width=300, height=400)

    keyword_var = tk.StringVar(window)

    for x in KEYWORDS:
        tk.Radiobutton(window, text=x, variable=keyword_var, value=x, indicator=0).pack(fill=tk.X, ipady=5)

    porcupine_thread = PorcupineThread(keyword_var)

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
