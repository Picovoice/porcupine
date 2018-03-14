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

package ai.picovoice.porcupinemanager;


import ai.picovoice.porcupine.Porcupine;
import ai.picovoice.porcupine.PorcupineException;


/**
 * PorcupineManager records the raw audio, processes it in a stream fashion using Porcupine library,
 * and notifies the client when the keyword is spotted.
 */
public class PorcupineManager {
    private final AudioRecorder audioRecorder;
    private final Porcupine porcupine;

    /**
     * PorcupineAudioConsumer process the raw PCM data returned by {@link AudioRecorder} and
     * notifies the user by using {@link KeywordCallback}.
     */
    private class PorcupineAudioConsumer implements AudioConsumer {
        private final KeywordCallback keywordCallback;

        /**
         * Initialize PorcupineAudioConsumer.
         * @param keywordCallback Callback to use when the keyword is detected.
         */
        PorcupineAudioConsumer(KeywordCallback keywordCallback) {
            this.keywordCallback = keywordCallback;
        }
        /**
         * Consume the raw PCM data and notify user by using {@link KeywordCallback}.
         * @throws PorcupineException An exception is thrown if there is an error while processing
         * the PCM data by Porcupine library.
         */
        @Override
        public void consume(short[] pcm) throws PorcupineException {
            boolean seen = porcupine.process(pcm);
            if (seen) {
                keywordCallback.run();
            }
        }

        /**
         * Number of audio samples per frame.
         * @return Number of samples per frame.
         */
        @Override
        public int getFrameLength() {
            return porcupine.getFrameLength();
        }

        /**
         * Number of audio samples per second.
         * @return Sample rate of the audio data.
         */
        @Override
        public int getSampleRate() {
            return porcupine.getSampleRate();
        }
    }

    /**
     * Initialize Porcupine.
     * @param modelFilePath Absolute path to file containing model parameters.
     * @param keywordFilePath Absolute path to keyword file containing hyper-parameters.
     * @param sensitivity Sensitivity parameter. A higher sensitivity value lowers miss rate
     * at the cost of increased false alarm rate. For more information regarding this parameter
     * refer to 'include/pv_porcupine.h'.
     * @param keywordCallback callback when hte keyword is detected.
     * @throws PorcupineException if there is an error while initializing Porcupine.
     */
    public PorcupineManager(String modelFilePath, String keywordFilePath, float sensitivity,
                            KeywordCallback keywordCallback) throws PorcupineException {
        porcupine = new Porcupine(modelFilePath, keywordFilePath, sensitivity);
        AudioConsumer audioConsumer = new PorcupineAudioConsumer(keywordCallback);
        audioRecorder = new AudioRecorder(audioConsumer);
    }

    /**
     * Start recording.
     * @throws PorcupineException if AudioRecorder throws an exception while recording audio.
     * audio.
     */
    public void start() throws PorcupineException {
        audioRecorder.start();
    }

    /**
     * Stop recording and dispose the engine.
     * @throws PorcupineException if the {@link AudioRecorder} throws an exception while it's
     * getting stopped.
     */
    public void stop() throws PorcupineException {
        try {
            audioRecorder.stop();
        } catch (InterruptedException e) {
            throw new PorcupineException(e);
        } finally {
            porcupine.delete();
        }
    }
}
