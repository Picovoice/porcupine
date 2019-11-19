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


import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Process;
import android.util.Log;

import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicBoolean;


/**
 * Record the audio data from microphone and pass the raw PCM data to {@link AudioConsumer}.
 */
class AudioRecorder {
    private static final String TAG = AudioRecorder.class.getName();

    private final AudioConsumer audioConsumer;
    private final int sampleRate;
    private final int frameLength;

    private AtomicBoolean started = new AtomicBoolean(false);
    private AtomicBoolean stop = new AtomicBoolean(false);
    private AtomicBoolean stopped = new AtomicBoolean(false);

    /**
     * A task to record audio and send the audio samples to Porcupine library for processing.
     */
    private class RecordTask implements Callable<Void> {
        /**
         * Record audio.
         * @return return null that is needed by the {@link Callable} interface.
         * @throws PorcupineManagerException An exception is thrown if {@link AudioRecord} or
         * {@link ai.picovoice.porcupine} throws an error.
         */
        @Override
        public Void call() throws PorcupineManagerException {
            // Set the priority of this thread.
            android.os.Process.setThreadPriority(Process.THREAD_PRIORITY_URGENT_AUDIO);
            record();
            return null;
        }
    }

    /**
     * Initialize AudioRecorder.
     * @param audioConsumer Consumer for the audio samples recorded by {@link AudioRecorder}.
     */
    AudioRecorder(AudioConsumer audioConsumer) {
        this.audioConsumer = audioConsumer;
        this.sampleRate = audioConsumer.getSampleRate();
        this.frameLength = audioConsumer.getFrameLength();
    }

    /**
     * Start recording in a worker thread.
     */
    void start() {
        if (started.get()) {
            return;
        }
        started.set(true);
        RecordTask recordTask = new RecordTask();
        ExecutorService recordExecutor = Executors.newSingleThreadExecutor();
        recordExecutor.submit(recordTask);
    }

    /**
     * Stop the recorder gracefully.
     * @throws InterruptedException if the thread is interrupted.
     */
    void stop() throws InterruptedException{
        if (!started.get()) {
            return;
        }
        stop.set(true);
        while (!stopped.get()) {
            Thread.sleep(10);
        }
        started.set(false);
    }

    /***
     * Record the audio and call the {@link AudioConsumer} to consume the raw PCM data.
     * @throws PorcupineManagerException exception is thrown if {@link AudioConsumer} throws an error or
     * {@link AudioRecord} throws an error.
     */
    private void record() throws PorcupineManagerException {
        int bufferSize = Math.max(sampleRate / 2,
                AudioRecord.getMinBufferSize(sampleRate, AudioFormat.CHANNEL_IN_MONO,
                        AudioFormat.ENCODING_PCM_16BIT));

        // use short to hold 16-bit PCM encoding
        short[] buffer = new short[frameLength];
        AudioRecord record = null;
        try {
            record = new AudioRecord(MediaRecorder.AudioSource.MIC, sampleRate,
                    AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT, bufferSize);
            record.startRecording();

            while (!stop.get()) {
                int r = record.read(buffer, 0, buffer.length);
                //if there are enough audio samples pass it to the consumer.
                if (r == buffer.length) {
                    audioConsumer.consume(buffer);
                } else {
                    Log.d(TAG, "Not enough samples for the audio consumer.");
                }
            }
            record.stop();

        } catch (IllegalArgumentException | IllegalStateException e) {
            throw new PorcupineManagerException(e);
        } finally {
            if (record != null) {
                record.release();
            }
            stopped.set(true);
        }
    }
}
