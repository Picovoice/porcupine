/*
    Copyright 2025 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is
    located in the "LICENSE" file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the
    License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
    express or implied. See the License for the specific language governing permissions and
    limitations under the License.
*/

package ai.picovoice.porcupine.testapp;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;

import androidx.test.ext.junit.runners.AndroidJUnit4;

import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.File;
import java.util.ArrayList;
import java.io.IOException;

import ai.picovoice.porcupine.Porcupine;
import ai.picovoice.porcupine.PorcupineException;


@RunWith(AndroidJUnit4.class)
public class StandardTests extends BaseTest {

    @Test
    public void testInitSuccessWithSingleBuiltIn() throws PorcupineException {
        Porcupine p = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setKeyword(Porcupine.BuiltInKeyword.PORCUPINE)
                .build(appContext);

        assertTrue(p.getVersion() != null && !p.getVersion().isEmpty());
        assertTrue(p.getFrameLength() > 0);
        assertTrue(p.getSampleRate() > 0);

        p.delete();
    }

    @Test
    public void testInitSuccessWithMultipleBuiltIns() throws PorcupineException {
        Porcupine.BuiltInKeyword[] inputBuiltInKeywords = new Porcupine.BuiltInKeyword[]{
                Porcupine.BuiltInKeyword.ALEXA,
                Porcupine.BuiltInKeyword.GRASSHOPPER,
                Porcupine.BuiltInKeyword.PICOVOICE,
                Porcupine.BuiltInKeyword.TERMINATOR
        };

        Porcupine p = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setKeywords(inputBuiltInKeywords)
                .build(appContext);

        assertTrue(p.getVersion() != null && !p.getVersion().isEmpty());
        assertTrue(p.getFrameLength() > 0);
        assertTrue(p.getSampleRate() > 0);

        p.delete();
    }

    @Test
    public void testInitSuccessWithCustomArguments() throws PorcupineException, IOException {
        File keywordPath = new File(getKeywordFilepath("en/hey_barista_android.ppn"));
        File modelPath = new File(getModelFilepath("porcupine_params.pv"));
        Porcupine p = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setModelPath(modelPath.getAbsolutePath())
                .setKeywordPath(keywordPath.getAbsolutePath())
                .setSensitivity(0.25f)
                .build(appContext);

        assertTrue(p.getVersion() != null && !p.getVersion().isEmpty());
        assertTrue(p.getFrameLength() > 0);
        assertTrue(p.getSampleRate() > 0);

        p.delete();
    }

    @Test
    public void testInitSuccessWithMultipleCustomKeywordsAndSensitivities() throws PorcupineException, IOException {
        File keywordPath = new File(getKeywordFilepath("en/hey_barista_android.ppn"));
        File keywordPath2 = new File(getKeywordFilepath("en/pico_clock_android.ppn"));
        String[] keywordPaths = new String[]{keywordPath.getAbsolutePath(), keywordPath2.getAbsolutePath()};
        Porcupine p = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setKeywordPaths(keywordPaths)
                .setSensitivities(new float[]{0.35f, 0.6f})
                .build(appContext);

        assertTrue(p.getVersion() != null && !p.getVersion().isEmpty());
        assertTrue(p.getFrameLength() > 0);
        assertTrue(p.getSampleRate() > 0);

        p.delete();
    }

    @Test
    public void testInitFailWithMismatchedLanguage() throws IOException {
        boolean didFail = false;
        File keywordPath = new File(getKeywordFilepath("de/ananas_android.ppn"));
        File modelPath = new File(getModelFilepath("porcupine_params.pv"));
        try {
            new Porcupine.Builder()
                    .setAccessKey(accessKey)
                    .setModelPath(modelPath.getAbsolutePath())
                    .setKeywordPath(keywordPath.getAbsolutePath())
                    .build(appContext);
        } catch (PorcupineException e) {
            didFail = true;
        }

        assertTrue(didFail);
    }

    @Test
    public void testInitFailWithInvalidModelPath() {
        boolean didFail = false;
        File modelPath = new File(testResourcesPath, "bad_path/bad_path.pv");
        try {
            new Porcupine.Builder()
                    .setAccessKey(accessKey)
                    .setModelPath(modelPath.getAbsolutePath())
                    .setKeyword(Porcupine.BuiltInKeyword.PORCUPINE)
                    .build(appContext);
        } catch (PorcupineException e) {
            didFail = true;
        }

        assertTrue(didFail);
    }

    @Test
    public void testInitFailWithInvalidSensitivity() {
        boolean didFail = false;
        try {
            new Porcupine.Builder()
                    .setAccessKey(accessKey)
                    .setKeyword(Porcupine.BuiltInKeyword.PORCUPINE)
                    .setSensitivity(10)
                    .build(appContext);
        } catch (PorcupineException e) {
            didFail = true;
        }

        assertTrue(didFail);
    }

    @Test
    public void testInitFailWithMismatchedSensitivityCount() {
        boolean didFail = false;
        try {
            new Porcupine.Builder()
                    .setAccessKey(accessKey)
                    .setKeyword(Porcupine.BuiltInKeyword.PORCUPINE)
                    .setSensitivities(new float[]{0.4f, 0.8f})
                    .build(appContext);
        } catch (PorcupineException e) {
            didFail = true;
        }

        assertTrue(didFail);
    }

    @Test
    public void testInitFailWithNoKeywords() {
        boolean didFail = false;
        try {
            new Porcupine.Builder()
                    .setAccessKey(accessKey)
                    .build(appContext);
        } catch (PorcupineException e) {
            didFail = true;
        }

        assertTrue(didFail);
    }

    @Test
    public void testInitFailWithInvalidKeywordPath() {
        boolean didFail = false;
        File keywordPath = new File(testResourcesPath, "bad_path/bad_path.ppn");
        try {
            new Porcupine.Builder()
                    .setAccessKey(accessKey)
                    .setKeywordPath(keywordPath.getAbsolutePath())
                    .build(appContext);
        } catch (PorcupineException e) {
            didFail = true;
        }

        assertTrue(didFail);
    }

    @Test
    public void testInitFailWithBuiltInAndCustom() throws IOException {
        File keywordPath = new File(getKeywordFilepath("en/hey_barista_android.ppn"));
        boolean didFail = false;
        try {
            new Porcupine.Builder()
                    .setAccessKey(accessKey)
                    .setKeyword(Porcupine.BuiltInKeyword.PORCUPINE)
                    .setKeywordPath(keywordPath.getAbsolutePath())
                    .build(appContext);
        } catch (PorcupineException e) {
            didFail = true;
        }

        assertTrue(didFail);
    }

    @Test
    public void testInitFailWithWrongKeywordPlatform() throws IOException {
        boolean didFail = false;
        File keywordPath = new File(getKeywordFilepath("en/alexa_linux.ppn"));
        try {
            new Porcupine.Builder()
                    .setAccessKey(accessKey)
                    .setKeywordPath(keywordPath.getAbsolutePath())
                    .build(appContext);
        } catch (PorcupineException e) {
            didFail = true;
        }

        assertTrue(didFail);
    }

    @Test
    public void testProcSuccessSingleBuiltIn() throws Exception {
        Porcupine p = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setKeyword(Porcupine.BuiltInKeyword.PORCUPINE)
                .build(appContext);

        File testAudio = new File(getAudioFilepath("porcupine.wav"));
        ArrayList<Integer> detectionResults = processTestAudio(p, testAudio);
        p.delete();

        assertEquals(1, detectionResults.size());
        assertEquals(0, (int) detectionResults.get(0));
    }

    @Test
    public void testProcSuccessMultipleBuiltIn() throws Exception {
        Porcupine.BuiltInKeyword[] inputBuiltInKeywords = new Porcupine.BuiltInKeyword[]{
                Porcupine.BuiltInKeyword.ALEXA,
                Porcupine.BuiltInKeyword.AMERICANO,
                Porcupine.BuiltInKeyword.BLUEBERRY,
                Porcupine.BuiltInKeyword.BUMBLEBEE,
                Porcupine.BuiltInKeyword.GRAPEFRUIT,
                Porcupine.BuiltInKeyword.GRASSHOPPER,
                Porcupine.BuiltInKeyword.PICOVOICE,
                Porcupine.BuiltInKeyword.PORCUPINE,
                Porcupine.BuiltInKeyword.TERMINATOR
        };

        Porcupine.BuiltInKeyword[] expectedResults = new Porcupine.BuiltInKeyword[]{
                Porcupine.BuiltInKeyword.PORCUPINE,
                Porcupine.BuiltInKeyword.ALEXA,
                Porcupine.BuiltInKeyword.AMERICANO,
                Porcupine.BuiltInKeyword.BLUEBERRY,
                Porcupine.BuiltInKeyword.BUMBLEBEE,
                Porcupine.BuiltInKeyword.GRAPEFRUIT,
                Porcupine.BuiltInKeyword.GRASSHOPPER,
                Porcupine.BuiltInKeyword.PICOVOICE,
                Porcupine.BuiltInKeyword.PORCUPINE,
                Porcupine.BuiltInKeyword.TERMINATOR
        };

        Porcupine p = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setKeywords(inputBuiltInKeywords)
                .build(appContext);

        File testAudio = new File(getAudioFilepath("multiple_keywords.wav"));
        ArrayList<Integer> detectionResults = processTestAudio(p, testAudio);
        p.delete();

        assertSame(expectedResults.length, detectionResults.size());
        for (int i = 0; i < expectedResults.length; i++) {
            Porcupine.BuiltInKeyword expectedKeyword = expectedResults[i];
            assertTrue(detectionResults.get(i) < inputBuiltInKeywords.length);
            Porcupine.BuiltInKeyword keywordDetected = inputBuiltInKeywords[detectionResults.get(i)];
            assertSame(expectedKeyword, keywordDetected);
        }
    }

    @Test
    public void testInitWithNonAsciiModelName() throws PorcupineException, IOException {
        File keywordPath = new File(getKeywordFilepath("es/murciélago_android.ppn"));
        File modelPath = new File(getModelFilepath("porcupine_params_es.pv"));
        Porcupine p = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setKeywordPath(keywordPath.getAbsolutePath())
                .setModelPath(modelPath.getAbsolutePath())
                .build(appContext);

        assertTrue(p.getVersion() != null && !p.getVersion().isEmpty());
        assertTrue(p.getFrameLength() > 0);
        assertTrue(p.getSampleRate() > 0);

        p.delete();
    }

    @Test
    public void testErrorStack() {
        String[] error = {};
        try {
            new Porcupine.Builder()
                    .setAccessKey("invalid")
                    .setKeyword(Porcupine.BuiltInKeyword.PORCUPINE)
                    .build(appContext);
        } catch (PorcupineException e) {
            error = e.getMessageStack();
        }

        assertTrue(0 < error.length);
        assertTrue(error.length <= 8);

        try {
            new Porcupine.Builder()
                    .setAccessKey("invalid")
                    .setKeyword(Porcupine.BuiltInKeyword.PORCUPINE)
                    .build(appContext);
        } catch (PorcupineException e) {
            for (int i = 0; i < error.length; i++) {
                assertEquals(e.getMessageStack()[i], error[i]);
            }
        }
    }
}
