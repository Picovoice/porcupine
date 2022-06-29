/*
    Copyright 2022 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is
    located in the "LICENSE" file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the
    License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
    express or implied. See the License for the specific language governing permissions and
    limitations under the License.
*/

package ai.picovoice.porcupine.demo;

import static org.junit.Assert.assertTrue;

import androidx.test.ext.junit.runners.AndroidJUnit4;

import org.junit.Assume;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.File;
import java.io.FileInputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import ai.picovoice.porcupine.Porcupine;

@RunWith(AndroidJUnit4.class)
public class PerformanceTest extends BaseTest {

    @Test
    public void testPerformance() throws Exception {
        String iterationString = appContext.getString(R.string.numTestIterations);
        String thresholdString = appContext.getString(R.string.performanceThresholdSec);
        Assume.assumeNotNull(thresholdString);
        Assume.assumeFalse(thresholdString.equals(""));

        int numTestIterations = 100;
        try {
            numTestIterations = Integer.parseInt(iterationString);
        } catch (NumberFormatException ignored) {
        }
        double performanceThresholdSec = Double.parseDouble(thresholdString);

        Porcupine p = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setKeyword(Porcupine.BuiltInKeyword.PORCUPINE)
                .build(appContext);

        File testAudio = new File(testResourcesPath, "audio_samples/multiple_keywords.wav");

        long totalNSec = 0;
        for (int i = 0; i < numTestIterations; i++) {
            FileInputStream audioInputStream = new FileInputStream(testAudio);

            byte[] rawData = new byte[p.getFrameLength() * 2];
            short[] pcm = new short[p.getFrameLength()];
            ByteBuffer pcmBuff = ByteBuffer.wrap(rawData).order(ByteOrder.LITTLE_ENDIAN);

            audioInputStream.skip(44);
            while (audioInputStream.available() > 0) {
                int numRead = audioInputStream.read(pcmBuff.array());
                if (numRead == p.getFrameLength() * 2) {
                    pcmBuff.asShortBuffer().get(pcm);
                    long before = System.nanoTime();
                    p.process(pcm);
                    long after = System.nanoTime();
                    totalNSec += (after - before);
                }
            }
        }
        p.delete();

        double avgNSec = totalNSec / (double) numTestIterations;
        double avgSec = ((double) Math.round(avgNSec * 1e-6)) / 1000.0;
        assertTrue(
                String.format("Expected threshold (%.3fs), process took (%.3fs)", performanceThresholdSec, avgSec),
                avgSec <= performanceThresholdSec
        );
    }
}
