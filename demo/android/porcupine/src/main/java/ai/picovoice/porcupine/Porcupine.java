/*
 * Copyright 2018 Picovoice Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package ai.picovoice.porcupine;


/**
 * Android binding for Picovoice's wake word detection (aka Porcupine) library.
 */
public class Porcupine {
    static {
        System.loadLibrary("pv_porcupine");
    }

    /**
     * Initialize Porcupine.
     * @param modelFilePath Absolute path to file containing model parameters.
     * @param keywordFilePath Absolute path to keyword file containing hyper-parameters.
     * @param sensitivity Sensitivity parameter. A higher sensitivity value lowers miss rate
     * at the cost of increased false alarm rate. For more information regarding this parameter
     * refer to 'include/pv_porcupine.h'.
     * @throws PorcupineException if there is an error while initializing Porcupine.
     */
    public Porcupine(String modelFilePath, String keywordFilePath, float sensitivity)
            throws PorcupineException {
        try {
            init(modelFilePath, keywordFilePath, sensitivity);
        } catch (Exception e) {
            throw new PorcupineException(e);
        }
    }

    /**
     * Number of audio samples per frame expected by C library.
     * @return acceptable number of audio samples per frame.
     */
    public native int getFrameLength();

    /**
     * Audio sample rate accepted by Porcupine library.
     * @return sample rate acceptable by Porcupine.
     */
    public native int getSampleRate();

    /**
     * Monitors incoming audio stream for a given wake word.
     * @param pcm An array of consecutive audio samples. For more information
     * regarding required audio properties (i.e. sample rate, number of channels encoding, and
     * number of samples per frame) please refer to 'include/pv_porcupine.h'.
     * @return True if wake word is detected.
     * @throws PorcupineException if there is an error while processing the audio sample.
     */
    public boolean processFrame(short[] pcm) throws PorcupineException {
        try {
            return process(pcm);
        } catch (Exception e) {
            throw new PorcupineException(e);
        }
    }

    /**
     * Process incoming audio stream for a given wake word.
     * @param pcm An array of consecutive audio samples.
     * @return True if wake word is detected.
     */
    private native boolean process(short[] pcm);

    /**
     * Releases resources acquired by Porcupine's library.
     */
    public native void delete();

    /**
     * Initialize the Porcupine C library.
     * @param modelFilePath Absolute path to file containing model parameters.
     * @param keywordFilePath Absolute path to keyword file containing hyper-parameters.
     * @param sensitivity Sensitivity parameter.
     */
    private native void init(String modelFilePath, String keywordFilePath, float sensitivity);
}


