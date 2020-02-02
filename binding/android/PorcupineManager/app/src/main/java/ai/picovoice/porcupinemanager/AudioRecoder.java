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


import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Process;
import android.util.Log;

import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicBoolean;


class AudioRecorder {
    private static final String TAG = AudioRecorder.class.getName();

    private final PorcupineManager porcupineManager;
    private final int sampleRate;
    private final int frameLength;

    private AtomicBoolean started = new AtomicBoolean(false);
    private AtomicBoolean stop = new AtomicBoolean(false);
    private AtomicBoolean stopped = new AtomicBoolean(false);

    private class RecordTask implements Callable<Void> {
        @Override
        public Void call() throws PorcupineManagerException {
            // Set the priority of this thread.
            android.os.Process.setThreadPriority(Process.THREAD_PRIORITY_URGENT_AUDIO);
            record();
            return null;
        }
    }

    AudioRecorder(PorcupineManager porcupineManager) {
        this.porcupineManager = porcupineManager;
        this.sampleRate = porcupineManager.getSampleRate();
        this.frameLength = porcupineManager.getFrameLength();
    }

    void start() {
        if (started.get()) {
            return;
        }
        started.set(true);
        RecordTask recordTask = new RecordTask();
        ExecutorService recordExecutor = Executors.newSingleThreadExecutor();
        recordExecutor.submit(recordTask);
    }

    void stop() throws InterruptedException {
        if (!started.get()) {
            return;
        }
        stop.set(true);
        while (!stopped.get()) {
            Thread.sleep(10);
        }
        started.set(false);
    }

    private void record() throws PorcupineManagerException {
        int bufferSize = Math.max(sampleRate / 2,
                AudioRecord.getMinBufferSize(sampleRate, AudioFormat.CHANNEL_IN_MONO,
                        AudioFormat.ENCODING_PCM_16BIT));

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
                    porcupineManager.consume(buffer);
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
