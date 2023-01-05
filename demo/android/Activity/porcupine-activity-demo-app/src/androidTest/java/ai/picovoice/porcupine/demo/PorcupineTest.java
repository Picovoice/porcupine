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

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;

import com.google.gson.JsonArray;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;

import org.junit.Test;
import org.junit.experimental.runners.Enclosed;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Objects;

import ai.picovoice.porcupine.Porcupine;
import ai.picovoice.porcupine.PorcupineException;


@RunWith(Enclosed.class)
public class PorcupineTest {

    public static class StandardTests extends BaseTest {

        @Test
        public void testInitSuccessWithSingleBuiltIn() throws PorcupineException {
            Porcupine p = new Porcupine.Builder()
                    .setAccessKey(accessKey)
                    .setKeyword(Porcupine.BuiltInKeyword.PORCUPINE)
                    .build(appContext);

            assertTrue(p.getVersion() != null && !p.getVersion().equals(""));
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

            assertTrue(p.getVersion() != null && !p.getVersion().equals(""));
            assertTrue(p.getFrameLength() > 0);
            assertTrue(p.getSampleRate() > 0);

            p.delete();
        }

        @Test
        public void testInitSuccessWithCustomArguments() throws PorcupineException {
            File keywordPath = new File(testResourcesPath, "keyword_files/en/hey_barista_android.ppn");
            File modelPath = new File(testResourcesPath, "model_files/porcupine_params.pv");
            Porcupine p = new Porcupine.Builder()
                    .setAccessKey(accessKey)
                    .setModelPath(modelPath.getAbsolutePath())
                    .setKeywordPath(keywordPath.getAbsolutePath())
                    .setSensitivity(0.25f)
                    .build(appContext);

            assertTrue(p.getVersion() != null && !p.getVersion().equals(""));
            assertTrue(p.getFrameLength() > 0);
            assertTrue(p.getSampleRate() > 0);

            p.delete();
        }

        @Test
        public void testInitSuccessWithMultipleCustomKeywordsAndSensitivities() throws PorcupineException {
            File keywordPath = new File(testResourcesPath, "keyword_files/en/hey_barista_android.ppn");
            File keywordPath2 = new File(testResourcesPath, "keyword_files/en/pico_clock_android.ppn");
            String[] keywordPaths = new String[]{keywordPath.getAbsolutePath(), keywordPath2.getAbsolutePath()};
            Porcupine p = new Porcupine.Builder()
                    .setAccessKey(accessKey)
                    .setKeywordPaths(keywordPaths)
                    .setSensitivities(new float[]{0.35f, 0.6f})
                    .build(appContext);

            assertTrue(p.getVersion() != null && !p.getVersion().equals(""));
            assertTrue(p.getFrameLength() > 0);
            assertTrue(p.getSampleRate() > 0);

            p.delete();
        }

        @Test
        public void testInitFailWithMismatchedLanguage() {
            boolean didFail = false;
            File keywordPath = new File(testResourcesPath, "keyword_files/de/ananas_android.ppn");
            File modelPath = new File(testResourcesPath, "model_files/porcupine_params.pv");
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
        public void testInitFailWithBuiltInAndCustom() {
            File keywordPath = new File(testResourcesPath, "keyword_files/en/hey_barista_android.ppn");
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
        public void testInitFailWithWrongKeywordPlatform() {
            boolean didFail = false;
            File keywordPath = new File(testResourcesPath, "keyword_files/en/alexa_linux.ppn");
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

            File testAudio = new File(testResourcesPath, "audio_samples/porcupine.wav");
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

            File testAudio = new File(testResourcesPath, "audio_samples/multiple_keywords.wav");
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
        public void testInitWithNonAsciiModelName() throws PorcupineException {
            File keywordPath = new File(testResourcesPath, "keyword_files/es/murciélago_android.ppn");
            File modelPath = new File(testResourcesPath, "model_files/porcupine_params_es.pv");
            Porcupine p = new Porcupine.Builder()
                    .setAccessKey(accessKey)
                    .setKeywordPath(keywordPath.getAbsolutePath())
                    .setModelPath(modelPath.getAbsolutePath())
                    .build(appContext);

            assertTrue(p.getVersion() != null && !p.getVersion().equals(""));
            assertTrue(p.getFrameLength() > 0);
            assertTrue(p.getSampleRate() > 0);

            p.delete();
        }
    }

    @RunWith(Parameterized.class)
    public static class LanguageTests extends BaseTest {

        @Parameterized.Parameter(value = 0)
        public String modelFile;

        @Parameterized.Parameter(value = 1)
        public String[] keywordFiles;

        @Parameterized.Parameter(value = 2)
        public String testAudioFile;

        @Parameterized.Parameter(value = 3)
        public int[] expectedResults;

        @Parameterized.Parameters(name = "{0}")
        public static Collection<Object[]> initParameters() throws IOException {
            String testDataJsonString = getTestDataString();

            JsonObject testDataJson = JsonParser.parseString(testDataJsonString).getAsJsonObject();
            JsonArray multipleKeywordDataJson = testDataJson.getAsJsonObject("tests").getAsJsonArray("multipleKeyword");

            List<Object[]> parameters = new ArrayList<>();
            for (int i = 0; i < multipleKeywordDataJson.size(); i++) {
                JsonObject testData = multipleKeywordDataJson.get(i).getAsJsonObject();
                String language = testData.get("language").getAsString();
                JsonArray keywords = testData.getAsJsonArray("wakewords");
                JsonArray groundTruthJson = testData.getAsJsonArray("groundTruth");

                String modelFile = String.format("model_files/porcupine_params_%s.pv", language);
                String[] keywordFiles = new String[keywords.size()];
                for (int j = 0; j < keywords.size(); j++) {
                    String keyword = keywords.get(j).getAsString();
                    keywordFiles[j] = String.format("keyword_files/%s/%s_android.ppn", language, keyword);
                }
                String audioFile = String.format("audio_samples/multiple_keywords_%s.wav", language);
                int[] groundTruth = new int[groundTruthJson.size()];
                for (int j = 0; j < groundTruthJson.size(); j++) {
                    groundTruth[j] = groundTruthJson.get(j).getAsInt();
                }

                if (Objects.equals(language, "en")) {
                    modelFile = "model_files/porcupine_params.pv";
                    audioFile = "audio_samples/multiple_keywords.wav";
                }

                parameters.add(new Object[] {
                        modelFile,
                        keywordFiles,
                        audioFile,
                        groundTruth
                });
            }

            return parameters;
        }


        @Test
        public void testProcess() throws Exception {
            String modelPath = new File(testResourcesPath, modelFile).getAbsolutePath();
            String[] keywordPaths = new String[keywordFiles.length];
            for (int i = 0; i < keywordFiles.length; i++) {
                keywordPaths[i] = new File(testResourcesPath, keywordFiles[i]).getAbsolutePath();

            }
            Porcupine p = new Porcupine.Builder()
                    .setAccessKey(accessKey)
                    .setModelPath(modelPath)
                    .setKeywordPaths(keywordPaths)
                    .build(appContext);

            assertTrue(p.getVersion() != null && !p.getVersion().equals(""));
            assertTrue(p.getFrameLength() > 0);
            assertTrue(p.getSampleRate() > 0);

            File testAudio = new File(testResourcesPath, testAudioFile);
            ArrayList<Integer> detectionResults = processTestAudio(p, testAudio);
            p.delete();

            assertSame(expectedResults.length, detectionResults.size());
            for (int i = 0; i < expectedResults.length; i++) {
                assertSame(detectionResults.get(i), expectedResults[i]);
            }

            p.delete();
        }
    }
}
