/*
    Copyright 2018 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is
    located in the "LICENSE" file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the
    License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
    express or implied. See the License for the specific language governing permissions and
    limitations under the License.
*/

package ai.picovoice.porcupinemanager;


import ai.picovoice.porcupine.Porcupine;
import ai.picovoice.porcupine.PorcupineException;


/**
 * Records audio from microphone, processes it in real-time using Porcupine engine, and notifies the
 * client when the a keyword is detected.
 */
public class PorcupineManager {
    private final AudioRecorder audioRecorder;
    private final Porcupine porcupine;
    private final KeywordCallback keywordCallback;

    int getSampleRate() {
        return porcupine.getSampleRate();
    }

    int getFrameLength() {
        return porcupine.getFrameLength();
    }

    void consume(short[] pcm) throws PorcupineManagerException {
        try {
            final int keyword_index = porcupine.process(pcm);
            if (keyword_index >= 0) {
                keywordCallback.run(keyword_index);
            }
        } catch (PorcupineException e) {
            throw new PorcupineManagerException(e);
        }
    }

    /**
     * Constructor for single keyword use case.
     *
     * @param modelFilePath   Absolute path to file containing model parameters.
     * @param keywordFilePath Absolute path to keyword file containing hyper-parameters.
     * @param sensitivity     Sensitivity parameter. A higher sensitivity value lowers miss rate
     *                        at the cost of increased false alarm rate.
     * @param keywordCallback callback when hte keyword is detected.
     * @throws PorcupineManagerException if there is an error while initializing Porcupine.
     */
    public PorcupineManager(
            String modelFilePath,
            String keywordFilePath,
            float sensitivity,
            KeywordCallback keywordCallback) throws PorcupineManagerException {
        try {
            porcupine = new Porcupine(modelFilePath, keywordFilePath, sensitivity);
        } catch (PorcupineException e) {
            throw new PorcupineManagerException(e);
        }

        this.keywordCallback = keywordCallback;
        audioRecorder = new AudioRecorder(this);
    }

    /**
     * Constructor for multiple keywords use case.
     *
     * @param modelFilePath    Absolute path to file containing model parameters.
     * @param keywordFilePaths Absolute path to keyword files.
     * @param sensitivities    Array of sensitivity parameters for each keyword.
     * @param keywordCallback  Callback when keyword is detected.
     * @throws PorcupineManagerException if there is an error while initializing Porcupine.
     */
    public PorcupineManager(
            String modelFilePath,
            String[] keywordFilePaths,
            float[] sensitivities,
            KeywordCallback keywordCallback) throws PorcupineManagerException {
        try {
            porcupine = new Porcupine(modelFilePath, keywordFilePaths, sensitivities);
        } catch (PorcupineException e) {
            throw new PorcupineManagerException(e);
        }

        this.keywordCallback = keywordCallback;
        audioRecorder = new AudioRecorder(this);
    }

    /**
     * Start recording.
     */
    public void start() {
        audioRecorder.start();
    }

    /**
     * Stop recording and dispose the engine.
     *
     * @throws PorcupineManagerException if the {@link AudioRecorder} throws an exception while it's
     *                                   getting stopped.
     */
    public void stop() throws PorcupineManagerException {
        try {
            audioRecorder.stop();
        } catch (InterruptedException e) {
            throw new PorcupineManagerException(e);
        } finally {
            porcupine.delete();
        }
    }
}
