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

        self._stop = False
        self._is_stopped = False

    def run(self):
        porcupine = None
        py_audio = None
        audio_stream = None

        try:
            porcupine = pvporcupine.create(keywords=KEYWORDS)

            py_audio = pyaudio.PyAudio()
            audio_stream = py_audio.open(
                rate=porcupine.sample_rate,
                channels=1,
                format=pyaudio.paInt16,
                input=True,
                frames_per_buffer=porcupine.frame_length)

            while not self._stop:
                pcm = audio_stream.read(porcupine.frame_length)
                pcm = struct.unpack_from("h" * porcupine.frame_length, pcm)
                result = porcupine.process(pcm)
                if result >= 0:
                    self._keyword_var.set(KEYWORDS[result])
                    print(self._keyword_var.get())
        except KeyboardInterrupt:
            print('Stopping ...')
        finally:
            if audio_stream is not None:
                audio_stream.close()

            if py_audio is not None:
                py_audio.terminate()

            if porcupine is not None:
                porcupine.delete()

        self._is_stopped = True

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
    window.mainloop()


if __name__ == '__main__':
    main()
