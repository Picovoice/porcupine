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

/**
 * AudioConsumer process the raw PCM data.
 */
interface AudioConsumer {
    /**
     * Consume the raw PCM data.
     * @throws PorcupineManagerException An exception is thrown if there is an error while processing
     * the PCM data.
     */
    void consume(short[] pcm) throws PorcupineManagerException;

    /**
     * Number of audio samples per frame.
     * @return Number of samples per frame.
     */
    int getFrameLength();

    /**
     * Number of audio samples per second.
     * @return Sample rate of the audio data.
     */
    int getSampleRate();
}
