/*
    Copyright 2026 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is
    located in the "LICENSE" file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the
    License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
    express or implied. See the License for the specific language governing permissions and
    limitations under the License.
*/

package ai.picovoice.porcupine.testapp;

import static org.junit.Assert.assertTrue;

import org.junit.Test;
import org.junit.runner.RunWith;

import ai.picovoice.porcupine.Porcupine;
import ai.picovoice.porcupine.PorcupineException;

import androidx.test.ext.junit.runners.AndroidJUnit4;

@RunWith(AndroidJUnit4.class)
public class TrainTests extends BaseTest {

    @Test
    public void testTrainModel() throws PorcupineException {
        String outputPath = appContext.getFileStreamPath("custom_phrase_android.ppn").getAbsolutePath();

        Porcupine.trainWakeWordFromPhrase(
                accessKey,
                outputPath,
                "en",
                "custom phrase"
        );

        Porcupine p = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setDevice(device)
                .setKeywordPath(outputPath)
                .build(appContext);

        assertTrue(p.getVersion() != null && !p.getVersion().equals(""));
        assertTrue(p.getFrameLength() > 0);
        assertTrue(p.getSampleRate() > 0);

        p.delete();
    }

    @Test
    public void testTrainModelInvalidSlots() {
        String outputPath = appContext.getFileStreamPath("custom_phrase_android.ppn").getAbsolutePath();

        boolean didFail = false;
        try {
            Porcupine.trainWakeWordFromPhrase(
                    accessKey,
                    outputPath,
                    "en",
                    "ㅁ ㄴ ㅇ ㄹ"
            );
        } catch (PorcupineException e) {
            didFail = true;
        }

        assertTrue(didFail);
    }

}