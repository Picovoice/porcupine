/*
    Copyright 2021-2023 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is
    located in the "LICENSE" file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the
    License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
    express or implied. See the License for the specific language governing permissions and
    limitations under the License.
*/

package ai.picovoice.porcupine;

import android.content.Context;
import android.util.Log;

import ai.picovoice.android.voiceprocessor.VoiceProcessor;
import ai.picovoice.android.voiceprocessor.VoiceProcessorErrorListener;
import ai.picovoice.android.voiceprocessor.VoiceProcessorException;
import ai.picovoice.android.voiceprocessor.VoiceProcessorFrameListener;

/**
 * High-level Android binding for Porcupine wake word engine. It handles recording audio from
 * microphone, processes it in real-time using Porcupine, and notifies the client when any of the
 * given keywords are detected. For detailed information regarding Porcupine refer to
 * ${@link Porcupine}.
 */
public class PorcupineManager {
    private final Porcupine porcupine;
    private final VoiceProcessor voiceProcessor;
    private final VoiceProcessorFrameListener vpFrameListener;
    private final VoiceProcessorErrorListener vpErrorListener;

    private boolean isListening;

    /**
     * Private constructor.
     *
     * @param porcupine     An instance of the Porcupine wake word engine.
     * @param callback      A callback function that is invoked upon detection of any of the keywords.
     * @param errorCallback A callback that reports errors encountered while processing audio.
     */
    private PorcupineManager(final Porcupine porcupine,
                             final PorcupineManagerCallback callback,
                             final PorcupineManagerErrorCallback errorCallback) {
        this.porcupine = porcupine;
        this.voiceProcessor = VoiceProcessor.getInstance();
        this.vpFrameListener = new VoiceProcessorFrameListener() {
            @Override
            public void onFrame(short[] frame) {
                try {
                    int keywordIndex = porcupine.process(frame);
                    if (keywordIndex >= 0) {
                        callback.invoke(keywordIndex);
                    }
                } catch (PorcupineException e) {
                    if (errorCallback != null) {
                        errorCallback.invoke(e);
                    } else {
                        Log.e("PorcupineManager", e.toString());
                    }
                }
            }
        };
        this.vpErrorListener = new VoiceProcessorErrorListener() {
            @Override
            public void onError(VoiceProcessorException error) {
                if (errorCallback != null) {
                    errorCallback.invoke(new PorcupineException(error));
                } else {
                    Log.e("PorcupineManager", error.toString());
                }
            }
        };
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
    public void start() throws PorcupineException {
        if (isListening) {
            return;
        }

        this.voiceProcessor.addFrameListener(vpFrameListener);
        this.voiceProcessor.addErrorListener(vpErrorListener);

        try {
            voiceProcessor.start(porcupine.getFrameLength(), porcupine.getSampleRate());
        } catch (VoiceProcessorException e) {
            throw new PorcupineException(e);
        }
        isListening = true;
    }

    /**
     * Stops recording audio from the microphone.
     *
     * @throws PorcupineException if there's a problem stopping the recording.
     */
    public void stop() throws PorcupineException {
        if (!isListening) {
            return;
        }
        voiceProcessor.removeErrorListener(vpErrorListener);
        voiceProcessor.removeFrameListener(vpFrameListener);
        if (voiceProcessor.getNumFrameListeners() == 0) {
            try {
                voiceProcessor.stop();
            } catch (VoiceProcessorException e) {
                throw new PorcupineException(e);
            }
        }
        isListening = false;
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
        public PorcupineManager build(
                Context context,
                PorcupineManagerCallback callback) throws PorcupineException {

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
}
