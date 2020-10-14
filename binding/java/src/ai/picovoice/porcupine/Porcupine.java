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

import java.util.*;

/**
 * Java binding for Porcupine wake word engine. It detects utterances of given keywords within an
 * incoming stream of audio in real-time. It processes incoming audio in consecutive frames and for
 * each frame emits the detection result. The number of samples per frame can be attained by calling
 * {@link #getFrameLength()}. The incoming audio needs to have a sample rate equal to
 * {@link #getSampleRate()} and be 16-bit linearly-encoded. Porcupine operates on single-channel
 * audio.
 */
public class Porcupine {

    public static String MODEL_PATH;
    public static HashMap<String, String> KEYWORD_PATHS;
    public static Set<String> KEYWORDS;

    static {
        System.loadLibrary("pv_porcupine_jni");

        MODEL_PATH = Utils.pvModelPath();
        KEYWORD_PATHS = Utils.pvKeywordPaths();
        KEYWORDS = KEYWORD_PATHS.keySet();
    }

    /**
     * Builder for creating a instance of Porcupine with a mixture of default arguments
     */
    public static class Builder {

        private String modelPath = Porcupine.MODEL_PATH;
        private String[] keywordPaths = null;
        private String[] keywords = null;
        private float[] sensitivities = null;

        public Builder withModelPath(String modelPath){
            this.modelPath = modelPath;
            return this;
        }

        public Builder withKeywordPaths(String[] keywordPaths){
            this.keywordPaths = keywordPaths;
            return this;
        }

        public Builder withKeywords(String[] keywords){
            this.keywords = keywords;
            return this;
        }

        public Builder withKeyword(String keyword){
            this.keywords = new String[]{ keyword };
            return this;
        }

        public Builder withSensitivities(float[] sensitivities){
            this.sensitivities = sensitivities;
            return this;
        }

        public Builder withSensitivity(float sensitivity){
            this.sensitivities = new float[]{ sensitivity };
            return this;
        }

        /**
         * Validates properties and creates an instance of the Porcupine wake word engine.
         *
         * @return An instance of Porcupine wake word engine
         * @throws PorcupineException if there is an error while initializing Porcupine.
         */
        public Porcupine build() throws IllegalArgumentException, PorcupineException{
            if(this.keywordPaths == null){

                if(this.keywords == null){
                    throw new IllegalArgumentException("Either 'keywords' or 'keywordPaths' must be set.");
                }

                if(Porcupine.KEYWORDS.containsAll(Arrays.asList(keywords))){
                    this.keywordPaths = new String[keywords.length];
                    for(int i=0; i<keywords.length; i++){
                        this.keywordPaths[i] = Porcupine.KEYWORD_PATHS.get(keywords[i]);
                    }
                }
                else{
                    throw new IllegalArgumentException("One or more keywords are not available by default. Available default keywords are:\n" +
                            String.join(",", Porcupine.KEYWORDS));
                }
            }

            if(sensitivities == null){
                sensitivities = new float[keywordPaths.length];
                Arrays.fill(sensitivities, 0.5f);
            }

            if(sensitivities.length != keywordPaths.length){
                throw new IllegalArgumentException(String.format("Number of keywords (%d) does not match " +
                        "number of sensitivities (%d)", keywordPaths.length, sensitivities.length));
            }

            return new Porcupine(modelPath, keywordPaths, sensitivities);
        }
    }

    private final long handle;

    /**
     * Constructor.
     *
     * @param modelPath   Absolute path to the file containing model parameters.
     * @param keywordPath Absolute path to keyword model file.
     * @param sensitivity Sensitivity for detecting keyword. Should be a floating point number
     *                    within [0, 1]. A higher sensitivity results in fewer misses at the cost of
     *                    increasing false alarm rate.
     * @throws PorcupineException if there is an error while initializing Porcupine.
     */
    public Porcupine(String modelPath, String keywordPath, float sensitivity) throws PorcupineException {
        try {
            handle = init(modelPath, new String[]{keywordPath}, new float[]{sensitivity});
        } catch (Exception e) {
            throw new PorcupineException(e);
        }
    }

    /**
     * Constructor.
     *
     * @param modelPath     Absolute path to the file containing model parameters.
     * @param keywordPaths  Absolute paths to keyword model files.
     * @param sensitivities Sensitivities for detecting keywords. Each value should be a number
     *                      within [0, 1]. A higher sensitivity results in fewer misses at the cost
     *                      of increasing the false alarm rate.
     * @throws PorcupineException if there is an error while initializing Porcupine.
     */
    public Porcupine(String modelPath, String[] keywordPaths, float[] sensitivities) throws PorcupineException {
        try {
            handle = init(modelPath, keywordPaths, sensitivities);
        } catch (Exception e) {
            throw new PorcupineException(e);
        }
    }

    /**
     * Releases resources acquired by Porcupine.
     */
    public void delete() {
        delete(handle);
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
            return process(handle, pcm);
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
     * Getter for number of audio samples per frame..
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
}