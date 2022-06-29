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

import android.content.Context;
import android.content.res.AssetManager;

import androidx.test.platform.app.InstrumentationRegistry;

import com.microsoft.appcenter.espresso.Factory;
import com.microsoft.appcenter.espresso.ReportHelper;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.experimental.runners.Enclosed;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;

import ai.picovoice.porcupine.Porcupine;
import ai.picovoice.porcupine.PorcupineException;

class BaseTest {

    @Rule
    public ReportHelper reportHelper = Factory.getReportHelper();

    Context testContext;
    Context appContext;
    AssetManager assetManager;
    String testResourcesPath;
    String accessKey;

    @After
    public void TearDown() {
        reportHelper.label("Stopping App");
    }

    @Before
    public void Setup() throws IOException {
        testContext = InstrumentationRegistry.getInstrumentation().getContext();
        appContext = InstrumentationRegistry.getInstrumentation().getTargetContext();
        assetManager = testContext.getAssets();
        extractAssetsRecursively("test_resources");
        testResourcesPath = new File(appContext.getFilesDir(), "test_resources").getAbsolutePath();

        accessKey = appContext.getString(R.string.pvTestingAccessKey);
    }

    private void extractAssetsRecursively(String path) throws IOException {

        String[] list = assetManager.list(path);
        if (list.length > 0) {
            File outputFile = new File(appContext.getFilesDir(), path);
            if (!outputFile.exists()) {
                outputFile.mkdirs();
            }

            for (String file : list) {
                String filepath = path + "/" + file;
                extractAssetsRecursively(filepath);
            }
        } else {
            extractTestFile(path);
        }
    }

    private void extractTestFile(String filepath) throws IOException {

        InputStream is = new BufferedInputStream(assetManager.open(filepath), 256);
        File absPath = new File(appContext.getFilesDir(), filepath);
        OutputStream os = new BufferedOutputStream(new FileOutputStream(absPath), 256);
        int r;
        while ((r = is.read()) != -1) {
            os.write(r);
        }
        os.flush();

        is.close();
        os.close();
    }

    ArrayList<Integer> processTestAudio(Porcupine p, File testAudio) throws Exception {
        FileInputStream audioInputStream = new FileInputStream(testAudio);

        byte[] rawData = new byte[p.getFrameLength() * 2];
        short[] pcm = new short[p.getFrameLength()];
        ByteBuffer pcmBuff = ByteBuffer.wrap(rawData).order(ByteOrder.LITTLE_ENDIAN);

        audioInputStream.skip(44);

        ArrayList<Integer> detectionResults = new ArrayList<>();
        while (audioInputStream.available() > 0) {
            int numRead = audioInputStream.read(pcmBuff.array());
            if (numRead == p.getFrameLength() * 2) {
                pcmBuff.asShortBuffer().get(pcm);
                int keywordIndex = p.process(pcm);
                if (keywordIndex >= 0) {
                    detectionResults.add(keywordIndex);
                }
            }
        }
        return detectionResults;
    }
}

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
        public static Collection<Object[]> initParameters() {
            return Arrays.asList(new Object[][]{
                    {
                            "model_files/porcupine_params_de.pv",
                            new String[]{
                                    "keyword_files/de/ananas_android.ppn",
                                    "keyword_files/de/heuschrecke_android.ppn",
                                    "keyword_files/de/himbeere_android.ppn",
                                    "keyword_files/de/leguan_android.ppn",
                                    "keyword_files/de/stachelschwein_android.ppn"
                            },
                            "audio_samples/multiple_keywords_de.wav",
                            new int[]{0, 1, 2, 3, 4}
                    },
                    {
                            "model_files/porcupine_params_es.pv",
                            new String[]{
                                    "keyword_files/es/emparedado_android.ppn",
                                    "keyword_files/es/leopardo_android.ppn",
                                    "keyword_files/es/manzana_android.ppn",
                                    "keyword_files/es/murciélago_android.ppn"
                            },
                            "audio_samples/multiple_keywords_es.wav",
                            new int[]{0, 1, 2, 3}
                    },
                    {
                            "model_files/porcupine_params_fr.pv",
                            new String[]{
                                    "keyword_files/fr/framboise_android.ppn",
                                    "keyword_files/fr/mon chouchou_android.ppn",
                                    "keyword_files/fr/parapluie_android.ppn"
                            },
                            "audio_samples/multiple_keywords_fr.wav",
                            new int[]{0, 1, 0, 2}
                    },
                    {
                            "model_files/porcupine_params_it.pv",
                            new String[]{
                                    "keyword_files/it/cameriere_android.ppn",
                                    "keyword_files/it/espresso_android.ppn",
                                    "keyword_files/it/porcospino_android.ppn",
                                    "keyword_files/it/silenzio_bruno_android.ppn"
                            },
                            "audio_samples/multiple_keywords_it.wav",
                            new int[]{2, 1, 0}
                    },
                    {
                            "model_files/porcupine_params_ja.pv",
                            new String[]{
                                    "keyword_files/ja/bushi_android.ppn",
                                    "keyword_files/ja/ninja_android.ppn",
                                    "keyword_files/ja/ringo_android.ppn",
                            },
                            "audio_samples/multiple_keywords_ja.wav",
                            new int[]{2, 0, 1}
                    },
                    {
                            "model_files/porcupine_params_ko.pv",
                            new String[]{
                                    "keyword_files/ko/aiseukeulim_android.ppn",
                                    "keyword_files/ko/bigseubi_android.ppn",
                                    "keyword_files/ko/koppulso_android.ppn",
                            },
                            "audio_samples/multiple_keywords_ko.wav",
                            new int[]{1, 2, 0}
                    },
                    {
                            "model_files/porcupine_params_pt.pv",
                            new String[]{
                                    "keyword_files/pt/abacaxi_android.ppn",
                                    "keyword_files/pt/fenomeno_android.ppn",
                                    "keyword_files/pt/formiga_android.ppn",
                                    "keyword_files/pt/porco_espinho_android.ppn",
                            },
                            "audio_samples/multiple_keywords_pt.wav",
                            new int[]{3, 0, 2, 1}
                    }
            });
        }


        @Test
        public void testInitSuccess() throws Exception {
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