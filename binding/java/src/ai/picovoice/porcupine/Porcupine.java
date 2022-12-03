/*
    Copyright 2018-2022 Picovoice Inc.

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
import java.util.stream.Collectors;
import java.util.stream.Stream;

/**
 * Java binding for Porcupine wake word engine. It detects utterances of given keywords within an
 * incoming stream of audio in real-time. It processes incoming audio in consecutive frames and for
 * each frame emits the detection result. The number of samples per frame can be attained by calling
 * {@link #getFrameLength()}. The incoming audio needs to have a sample rate equal to
 * {@link #getSampleRate()} and be 16-bit linearly-encoded. Porcupine operates on single-channel
 * audio.
 */
public class Porcupine {

    public static final String LIBRARY_PATH;
    public static final String MODEL_PATH;
    public static final HashMap<BuiltInKeyword, String> BUILT_IN_KEYWORD_PATHS;

    static {
        LIBRARY_PATH = Utils.getPackagedLibraryPath();
        MODEL_PATH = Utils.getPackagedModelPath();
        BUILT_IN_KEYWORD_PATHS = Utils.getPackagedKeywordPaths();
    }

    private long handle;

    /**
     * Constructor.
     *
     * @param accessKey     AccessKey obtained from Picovoice Console.
     * @param libraryPath   Absolute path to the native Porcupine library.
     * @param modelPath     Absolute path to the file containing model parameters.
     * @param keywordPaths  Absolute paths to keyword model files.
     * @param sensitivities Sensitivities for detecting keywords. Each value should be a number
     *                      within [0, 1]. A higher sensitivity results in fewer misses at the cost
     *                      of increasing the false alarm rate.
     * @throws PorcupineException if there is an error while initializing Porcupine.
     */
    public Porcupine(
            String accessKey,
            String libraryPath,
            String modelPath,
            String[] keywordPaths,
            float[] sensitivities) throws PorcupineException {
        try {
            System.load(libraryPath);
        } catch (Exception exception) {
            throw new PorcupineException(exception);
        }
        handle = PorcupineNative.init(
                accessKey,
                modelPath,
                keywordPaths,
                sensitivities);
    }

    /**
     * Releases resources acquired by Porcupine.
     */
    public void delete() {
        if (handle != 0) {
            PorcupineNative.delete(handle);
            handle = 0;
        }
    }

    /**
     * Processes a frame of the incoming audio stream and emits the detection result.
     *
     * @param pcm A frame of audio samples. The number of samples per frame can be attained by
     *            calling {@link #getFrameLength()}. The incoming audio needs to have a sample rate
     *            equal to {@link #getSampleRate()} and be 16-bit linearly-encoded. Porcupine
     *            operates on single-channel audio.
     * @return Index of observed keyword at the end of the current frame. Indexing is 0-based and
     *         matches the ordering of keyword models provided to the constructor. If no
     *         keyword is detected then it returns -1.
     * @throws PorcupineException if there is an error while processing the audio frame.
     */
    public int process(short[] pcm) throws PorcupineException {
        if (handle == 0) {
            throw new PorcupineException(
                    new IllegalStateException("Attempted to call Porcupine process after delete."));
        }
        if (pcm == null) {
            throw new PorcupineException(
                    new IllegalArgumentException("Passed null frame to Porcupine process."));
        }

        if (pcm.length != getFrameLength()) {
            throw new PorcupineException(
                    new IllegalArgumentException(
                            String.format("Porcupine process requires frames of length %d. " +
                                    "Received frame of size %d.", getFrameLength(), pcm.length)));
        }

        return PorcupineNative.process(handle, pcm);
    }

    /**
     * Getter for version.
     *
     * @return Version.
     */
    public String getVersion() {
        return PorcupineNative.getVersion();
    }

    /**
     * Getter for number of audio samples per frame.
     *
     * @return Number of audio samples per frame.
     */
    public int getFrameLength() {
        return PorcupineNative.getFrameLength();
    }

    /**
     * Getter for audio sample rate accepted by Picovoice.
     *
     * @return Audio sample rate accepted by Picovoice.
     */
    public int getSampleRate() {
        return PorcupineNative.getSampleRate();
    }

    /**
     * BuiltInKeyword Enum.
     */
    public enum BuiltInKeyword {
        ALEXA,
        AMERICANO,
        BLUEBERRY,
        BUMBLEBEE,
        COMPUTER,
        GRAPEFRUIT,
        GRASSHOPPER,
        HEY_GOOGLE,
        HEY_SIRI,
        JARVIS,
        OK_GOOGLE,
        PICOVOICE,
        PORCUPINE,
        TERMINATOR;

        public static Stream<BuiltInKeyword> stream() {
            return Stream.of(BuiltInKeyword.values());
        }

        public static String options() {
            return BuiltInKeyword.stream().map((v) -> v.name()).collect(Collectors.joining(","));
        }
    }

    /**
     * Builder for creating an instance of Porcupine with a mixture of default arguments.
     */
    public static class Builder {

        private String accessKey = null;
        private String libraryPath = null;
        private String modelPath = null;
        private String[] keywordPaths = null;
        private BuiltInKeyword[] keywords = null;
        private float[] sensitivities = null;

        public Builder setAccessKey(String accessKey) {
            this.accessKey = accessKey;
            return this;
        }

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

        public Builder setBuiltInKeywords(BuiltInKeyword[] keywords) {
            this.keywords = keywords;
            return this;
        }

        public Builder setBuiltInKeyword(BuiltInKeyword keyword) {
            this.keywords = new BuiltInKeyword[]{keyword};
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
                throw new PorcupineRuntimeException("Could not initialize Porcupine. " +
                        "Execution environment not currently supported by Porcupine Java.");
            }

            if (accessKey == null) {
                throw new PorcupineInvalidArgumentException(
                        "AccessKey is required for Porcupine initialization.");
            }

            if (libraryPath == null) {
                if (Utils.isResourcesAvailable()) {
                    libraryPath = LIBRARY_PATH;
                } else {
                    throw new PorcupineInvalidArgumentException("Default library unavailable. " +
                            "Please provide a native Porcupine library path (-l <library_path>).");
                }
            }

            if (modelPath == null) {
                if (Utils.isResourcesAvailable()) {
                    modelPath = MODEL_PATH;
                } else {
                    throw new PorcupineInvalidArgumentException("Default model unavailable. " +
                            "Please provide a valid Porcupine model path (-m <model_path>).");
                }
            }

            if (this.keywordPaths != null && this.keywords != null) {
                throw new PorcupineInvalidArgumentException("Both 'keywords' and 'keywordPaths' " +
                        "were set. Only one of the two arguments may be set for initialization.");
            }

            if (this.keywordPaths == null) {
                if (this.keywords == null) {
                    throw new PorcupineInvalidArgumentException(
                            "Either 'keywords' or 'keywordPaths' must be set.");
                }

                if (Utils.isResourcesAvailable()) {
                    this.keywordPaths = new String[keywords.length];
                    for (int i = 0; i < keywords.length; i++) {
                        this.keywordPaths[i] = BUILT_IN_KEYWORD_PATHS.get(keywords[i]);
                    }
                } else {
                    throw new PorcupineInvalidArgumentException("BuiltIn keywords unavailable. " +
                            "Please provide a valid Porcupine keyword path.");
                }
            }

            if (sensitivities == null) {
                sensitivities = new float[keywordPaths.length];
                Arrays.fill(sensitivities, 0.5f);
            }

            if (sensitivities.length != keywordPaths.length) {
                throw new PorcupineInvalidArgumentException(
                        String.format(
                                "Number of keywords (%d) does not match number of " +
                                        "sensitivities (%d)",
                                keywordPaths.length,
                                sensitivities.length));
            }

            return new Porcupine(accessKey, libraryPath, modelPath, keywordPaths, sensitivities);
        }
    }
}
