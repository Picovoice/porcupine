/*
    Copyright 2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is
    located in the "LICENSE" file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the
    License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
    express or implied. See the License for the specific language governing permissions and
    limitations under the License.
*/

package ai.picovoice.porcupine;


import android.content.Context;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Handler;
import android.os.Looper;
import android.os.Process;
import android.util.Log;

import java.util.concurrent.Callable;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * High-level Android binding for Porcupine wake word engine. It handles recording audio from
 * microphone, processes it in real-time using Porcupine, and notifies the client when any of the
 * given keywords are detected. For detailed information regarding Porcupine refer to
 * ${@link Porcupine}.
 */
public class PorcupineManager {
    private final MicrophoneReader microphoneReader;
    private final Porcupine porcupine;
    private final PorcupineManagerCallback callback;
    private final PorcupineManagerErrorCallback errorCallback;

    /**
     * Private constructor.
     *
     * @param porcupine     An instance of the Porcupine wake word engine.
     * @param callback      A callback function that is invoked upon detection of any of the keywords.
     * @param errorCallback A callback that reports errors encountered while processing audio.
     */
    private PorcupineManager(Porcupine porcupine,
                             PorcupineManagerCallback callback,
                             PorcupineManagerErrorCallback errorCallback) {


        this.porcupine = porcupine;
        this.callback = callback;
        this.errorCallback = errorCallback;
        microphoneReader = new MicrophoneReader();
    }

    /**
     * Releases resources acquired by Porcupine. It should be called when disposing the object.
     */
    public void delete() {
        porcupine.delete();
    }

    /**
     * Starts recording audio from the microphone and monitors it for the utterances of the given
     * set of keywords.
     */
    public void start() {
        microphoneReader.start();
    }

    /**
     * Stops recording audio from the microphone.
     *
     * @throws PorcupineException if the {@link MicrophoneReader} throws an exception while
     *                            it's being stopped.
     */
    public void stop() throws PorcupineException {
        try {
            microphoneReader.stop();
        } catch (InterruptedException e) {
            throw new PorcupineException(e);
        }
    }

    /**
     * Builder for creating an instance of PorcupineManager with a mixture of default arguments.
     */
    public static class Builder {

        private String accessKey = null;
        private String modelPath = null;
        private String[] keywordPaths = null;
        private Porcupine.BuiltInKeyword[] keywords = null;
        private float[] sensitivities = null;
        private PorcupineManagerErrorCallback errorCallback = null;

        public PorcupineManager.Builder setAccessKey(String accessKey) {
            this.accessKey = accessKey;
            return this;
        }

        public PorcupineManager.Builder setModelPath(String modelPath) {
            this.modelPath = modelPath;
            return this;
        }

        public PorcupineManager.Builder setKeywordPaths(String[] keywordPaths) {
            this.keywordPaths = keywordPaths;
            return this;
        }

        public PorcupineManager.Builder setKeywordPath(String keywordPaths) {
            this.keywordPaths = new String[]{keywordPaths};
            return this;
        }

        public PorcupineManager.Builder setKeywords(Porcupine.BuiltInKeyword[] keywords) {
            this.keywords = keywords;
            return this;
        }

        public PorcupineManager.Builder setKeyword(Porcupine.BuiltInKeyword keyword) {
            this.keywords = new Porcupine.BuiltInKeyword[]{keyword};
            return this;
        }

        public PorcupineManager.Builder setSensitivities(float[] sensitivities) {
            this.sensitivities = sensitivities;
            return this;
        }

        public PorcupineManager.Builder setSensitivity(float sensitivity) {
            this.sensitivities = new float[]{sensitivity};
            return this;
        }

        public PorcupineManager.Builder setErrorCallback(PorcupineManagerErrorCallback errorCallback) {
            this.errorCallback = errorCallback;
            return this;
        }

        /**
         * Creates an instance of PorcupineManager.
         *
         * @param context  Android app context (for extracting Porcupine resources)
         * @param callback A callback function that is invoked upon detection of any of the keywords.
         * @return A PorcupineManager instance
         * @throws PorcupineException if there is an error while initializing Porcupine.
         */
        public PorcupineManager build(Context context, PorcupineManagerCallback callback) throws PorcupineException {

            Porcupine porcupine = new Porcupine.Builder()
                    .setAccessKey(accessKey)
                    .setModelPath(modelPath)
                    .setKeywordPaths(keywordPaths)
                    .setKeywords(keywords)
                    .setSensitivities(sensitivities)
                    .build(context);
            return new PorcupineManager(porcupine, callback, errorCallback);
        }
    }

    private class MicrophoneReader {
        private final AtomicBoolean started = new AtomicBoolean(false);
        private final AtomicBoolean stop = new AtomicBoolean(false);
        private final AtomicBoolean stopped = new AtomicBoolean(false);

        private final Handler callbackHandler = new Handler(Looper.getMainLooper());

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

        private void read() {
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
                                callbackHandler.post(new Runnable() {
                                    @Override
                                    public void run() {
                                        callback.invoke(keywordIndex);
                                    }
                                });
                            }
                        } catch (final PorcupineException e) {
                            if (errorCallback != null) {
                                callbackHandler.post(new Runnable() {
                                    @Override
                                    public void run() {
                                        errorCallback.invoke(e);
                                    }
                                });
                            } else {
                                Log.e("PorcupineManager", e.toString());
                            }
                        }
                    }
                }

                audioRecord.stop();
            } catch (IllegalArgumentException | IllegalStateException e) {
                if (errorCallback != null) {
                    callbackHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            errorCallback.invoke(new PorcupineException(e));
                        }
                    });
                } else {
                    Log.e("PorcupineManager", e.toString());
                }
            } finally {
                if (audioRecord != null) {
                    audioRecord.release();
                }

                stopped.set(true);
            }
        }
    }
}
