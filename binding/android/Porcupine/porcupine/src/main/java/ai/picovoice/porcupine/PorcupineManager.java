/*
    Copyright 2018-2020 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is
    located in the "LICENSE" file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the
    License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
    express or implied. See the License for the specific language governing permissions and
    limitations under the License.
*/

package ai.picovoice.porcupine;


import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Process;

import java.util.concurrent.Callable;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * High-level interface for Porcupine wake word engine. It handles recording audio from microphone,
 * processes it in real-time using Porcupine, and notifies the client when a keyword is detected.
 */
public class PorcupineManager {
    private class MicrophoneReader {
        private AtomicBoolean started = new AtomicBoolean(false);
        private AtomicBoolean stop = new AtomicBoolean(false);
        private AtomicBoolean stopped = new AtomicBoolean(false);

        void start() {
            if (started.get()) {
                return;
            }

            started.set(true);

            Executors.newSingleThreadExecutor().submit(new Callable<Void>() {
                @Override
                public Void call() throws PorcupineException {
                    android.os.Process.setThreadPriority(Process.THREAD_PRIORITY_URGENT_AUDIO);
                    read();
                    return null;
                }
            });
        }

        void stop() throws InterruptedException {
            if (!started.get()) {
                return;
            }

            stop.set(true);

            while (!stopped.get()) {
                Thread.sleep(10);
            }

            started.set(false);
            stop.set(false);
            stopped.set(false);
        }

        private void read() throws PorcupineException {
            final int minBufferSize = AudioRecord.getMinBufferSize(
                    porcupine.getSampleRate(),
                    AudioFormat.CHANNEL_IN_MONO,
                    AudioFormat.ENCODING_PCM_16BIT);
            final int bufferSize = Math.max(porcupine.getSampleRate() / 2, minBufferSize);

            AudioRecord audioRecord = null;

            short[] buffer = new short[porcupine.getFrameLength()];

            try {
                audioRecord = new AudioRecord(
                        MediaRecorder.AudioSource.MIC,
                        porcupine.getSampleRate(),
                        AudioFormat.CHANNEL_IN_MONO,
                        AudioFormat.ENCODING_PCM_16BIT,
                        bufferSize);
                audioRecord.startRecording();

                while (!stop.get()) {
                    if (audioRecord.read(buffer, 0, buffer.length) == buffer.length) {
                        try {
                            final int keywordIndex = porcupine.process(buffer);
                            if (keywordIndex >= 0) {
                                callback.invoke(keywordIndex);
                            }
                        } catch (PorcupineException e) {
                            throw new PorcupineException(e);
                        }
                    }
                }

                audioRecord.stop();
            } catch (IllegalArgumentException | IllegalStateException e) {
                throw new PorcupineException(e);
            } finally {
                if (audioRecord != null) {
                    audioRecord.release();
                }

                stopped.set(true);
            }
        }
    }

    private final MicrophoneReader microphoneReader;
    private final Porcupine porcupine;
    private final PorcupineManagerCallback callback;

    /**
     * Constructor.
     *
     * @param modelPath   Absolute path to the file containing model parameters.
     * @param keywordPath Absolute path to keyword model file.
     * @param sensitivity Sensitivity for detecting keyword. Should be a floating point number
     *                    within [0, 1]. A higher sensitivity results in fewer misses at the cost of
     *                    increasing false alarm rate.
     * @param callback    It is invoked upon detection of the keyword.
     * @throws PorcupineException if there is an error while initializing Porcupine.
     */
    public PorcupineManager(
            String modelPath,
            String keywordPath,
            float sensitivity,
            PorcupineManagerCallback callback) throws PorcupineException {
        try {
            porcupine = new Porcupine(modelPath, keywordPath, sensitivity);
        } catch (PorcupineException e) {
            throw new PorcupineException(e);
        }

        this.callback = callback;
        microphoneReader = new MicrophoneReader();
    }

    /**
     * Constructor.
     *
     * @param modelPath     Absolute path to file containing model parameters.
     * @param keywordPaths  Absolute paths to keyword model files.
     * @param sensitivities Sensitivities for detecting keywords. Each value should be a number
     *                      within [0, 1]. A higher sensitivity results in fewer misses at the cost
     *                      of increasing the false alarm rate.
     * @param callback      It is invoked upon detection of any of the keywords.
     * @throws PorcupineException if there is an error while initializing Porcupine.
     */
    public PorcupineManager(
            String modelPath,
            String[] keywordPaths,
            float[] sensitivities,
            PorcupineManagerCallback callback) throws PorcupineException {
        try {
            porcupine = new Porcupine(modelPath, keywordPaths, sensitivities);
        } catch (PorcupineException e) {
            throw new PorcupineException(e);
        }

        this.callback = callback;
        microphoneReader = new MicrophoneReader();
    }

    /**
     * Releases resources acquired by Porcupine. It should be called when disposing the object.
     */
    public void delete() {
        porcupine.delete();
    }

    /**
     * Starts recording.
     */
    public void start() {
        microphoneReader.start();
    }

    /**
     * Stops recording.
     *
     * @throws PorcupineException if the {@link MicrophoneReader} throws an exception while
     *                                   it's being stopped.
     */
    public void stop() throws PorcupineException {
        try {
            microphoneReader.stop();
        } catch (InterruptedException e) {
            throw new PorcupineException(e);
        }
    }
}
