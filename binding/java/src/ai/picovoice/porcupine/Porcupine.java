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

import java.util.Arrays;
import java.util.HashMap;
import java.util.Set;
import java.util.logging.Logger;

/**
 * Java binding for Porcupine wake word engine. It detects utterances of given keywords within an
 * incoming stream of audio in real-time. It processes incoming audio in consecutive frames and for
 * each frame emits the detection result. The number of samples per frame can be attained by calling
 * {@link #getFrameLength()}. The incoming audio needs to have a sample rate equal to
 * {@link #getSampleRate()} and be 16-bit linearly-encoded. Porcupine operates on single-channel
 * audio.
 */
public class Porcupine {

    private final long libraryHandle;

    public static final String LIBRARY_PATH;
    public static final String MODEL_PATH;
    public static final HashMap<String, String> KEYWORD_PATHS;
    public static final Set<String> KEYWORDS;

    static {
        LIBRARY_PATH = Utils.getPackagedLibraryPath();
        MODEL_PATH = Utils.getPackagedModelPath();
        KEYWORD_PATHS = Utils.getPackagedKeywordPaths();
        KEYWORDS = KEYWORD_PATHS.keySet();
    }

    /**
     * Constructor.
     *
     * @param libraryPath   Absolute path to the native Porcupine library.
     * @param modelPath     Absolute path to the file containing model parameters.
     * @param keywordPaths  Absolute paths to keyword model files.
     * @param sensitivities Sensitivities for detecting keywords. Each value should be a number
     *                      within [0, 1]. A higher sensitivity results in fewer misses at the cost
     *                      of increasing the false alarm rate.
     * @throws PorcupineException if there is an error while initializing Porcupine.
     */
    public Porcupine(String libraryPath, String modelPath, String[] keywordPaths, float[] sensitivities) throws PorcupineException {

        try {
            System.load(libraryPath);
            libraryHandle = init(modelPath, keywordPaths, sensitivities);
        } catch (Exception e) {
            throw new PorcupineException(e);
        }
    }

    /**
     * Releases resources acquired by Porcupine.
     */
    public void delete() {
        delete(libraryHandle);
    }

    /**
     * Processes a frame of the incoming audio stream and emits the detection result.
     *
     * @param pcm A frame of audio samples. The number of samples per frame can be attained by
     *            calling {@link #getFrameLength()}. The incoming audio needs to have a sample rate
     *            equal to {@link #getSampleRate()} and be 16-bit linearly-encoded. Porcupine
     *            operates on single-channel audio.
     * @return Index of observed keyword at the end of the current frame. Indexing is 0-based and
     * matches the ordering of keyword models provided to the constructor. If no keyword is detected
     * then it returns -1.
     * @throws PorcupineException if there is an error while processing the audio frame.
     */
    public int process(short[] pcm) throws PorcupineException {
        try {
            return process(libraryHandle, pcm);
        } catch (Exception e) {
            throw new PorcupineException(e);
        }
    }

    /**
     * Getter for version.
     *
     * @return Version.
     */
    public native String getVersion();

    /**
     * Getter for number of audio samples per frame.
     *
     * @return Number of audio samples per frame.
     */
    public native int getFrameLength();

    /**
     * Getter for audio sample rate accepted by Picovoice.
     *
     * @return Audio sample rate accepted by Picovoice.
     */
    public native int getSampleRate();

    private native long init(String modelPath, String[] keywordPaths, float[] sensitivities);

    private native void delete(long object);

    private native int process(long object, short[] pcm);

    /**
     * Builder for creating an instance of Porcupine with a mixture of default arguments
     */
    public static class Builder {

        private String libraryPath = null;
        private String modelPath = null;
        private String[] keywordPaths = null;
        private String[] keywords = null;
        private float[] sensitivities = null;

        public Builder setLibraryPath(String libraryPath) {
            this.libraryPath = libraryPath;
            return this;
        }

        public Builder setModelPath(String modelPath) {
            this.modelPath = modelPath;
            return this;
        }

        public Builder setKeywordPaths(String[] keywordPaths) {
            this.keywordPaths = keywordPaths;
            return this;
        }

        public Builder setKeywordPath(String keywordPaths) {
            this.keywordPaths = new String[]{keywordPaths};
            return this;
        }

        public Builder setKeywords(String[] keywords) {
            this.keywords = keywords;
            return this;
        }

        public Builder setKeyword(String keyword) {
            this.keywords = new String[]{keyword};
            return this;
        }

        public Builder setSensitivities(float[] sensitivities) {
            this.sensitivities = sensitivities;
            return this;
        }

        public Builder setSensitivity(float sensitivity) {
            this.sensitivities = new float[]{sensitivity};
            return this;
        }

        /**
         * Validates properties and creates an instance of the Porcupine wake word engine.
         *
         * @return An instance of Porcupine wake word engine
         * @throws PorcupineException if there is an error while initializing Porcupine.
         */
        public Porcupine build() throws PorcupineException {

            if (!Utils.isEnvironmentSupported()) {
                throw new PorcupineException(new RuntimeException("Could not initialize Porcupine. " +
                        "Execution environment not currently supported by Porcupine Java."));
            }

            if (libraryPath == null) {
                if (Utils.isResourcesAvailable()) {
                    libraryPath = LIBRARY_PATH;
                } else {
                    throw new PorcupineException(new IllegalArgumentException("Default library unavailable. Please " +
                            "provide a native Porcupine library path (-l <library_path>)."));
                }
            }

            if (modelPath == null) {
                if (Utils.isResourcesAvailable()) {
                    modelPath = MODEL_PATH;
                } else {
                    throw new PorcupineException(new IllegalArgumentException("Default model unavailable. Please provide a " +
                            "valid Porcupine model path (-m <model_path>)."));
                }
            }

            if(this.keywordPaths != null && this.keywords != null){
                throw new PorcupineException(new IllegalArgumentException("Both 'keywords' and 'keywordPaths' were set. " +
                        "Only one of the two arguments may be set for initialization."));
            }

            if (this.keywordPaths == null) {
                if (this.keywords == null) {
                    throw new PorcupineException(new IllegalArgumentException("Either 'keywords' or " +
                            "'keywordPaths' must be set."));
                }

                if (Utils.isResourcesAvailable()) {
                    if (KEYWORDS.containsAll(Arrays.asList(keywords))) {
                        this.keywordPaths = new String[keywords.length];
                        for (int i = 0; i < keywords.length; i++) {
                            this.keywordPaths[i] = KEYWORD_PATHS.get(keywords[i]);
                        }
                    } else {
                        throw new PorcupineException(new IllegalArgumentException("One or more keywords are not " +
                                "available by default. Available default keywords are:\n" +
                                String.join(",", Porcupine.KEYWORDS)));
                    }
                } else {
                    throw new PorcupineException(new IllegalArgumentException("Default keywords unavailable. Please provide " +
                            "a valid Porcupine keyword path (-kp <keyword_path(s)>)."));
                }
            }

            if (sensitivities == null) {
                sensitivities = new float[keywordPaths.length];
                Arrays.fill(sensitivities, 0.5f);
            }

            if (sensitivities.length != keywordPaths.length) {
                throw new PorcupineException(new IllegalArgumentException(String.format("Number of keywords (%d) " +
                        "does not match number of sensitivities (%d)", keywordPaths.length, sensitivities.length)));
            }

            return new Porcupine(libraryPath, modelPath, keywordPaths, sensitivities);
        }
    }
}