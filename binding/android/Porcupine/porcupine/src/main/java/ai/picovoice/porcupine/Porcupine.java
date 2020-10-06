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


/**
 * Android binding for Porcupine wake word engine. It detects utterances of given keywords within an
 * incoming stream of audio in real-time. It processes incoming audio in consecutive frames and for
 * each frame emits the detection result. The number of samples per frame can be attained by calling
 * {@link #getFrameLength()}. The incoming audio needs to have a sample rate equal to
 * {@link #getSampleRate()} and be 16-bit linearly-encoded. Porcupine operates on single-channel
 * audio.
 */
public class Porcupine {
    static {
        System.loadLibrary("pv_porcupine");
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
     * @throws PorcupineException If there is an error while initializing Porcupine.
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
     * @throws PorcupineException If there is an error while initializing Porcupine.
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
     * @throws PorcupineException if there is an error while processing the audio sample.
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
