/*
    Copyright 2018-2022 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is
    located in the "LICENSE" file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the
    License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
    express or implied. See the License for the specific language governing permissions and
    limitations under the License.
*/

package ai.picovoice.porcupine;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.condition.DisabledIf;

import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.UnsupportedAudioFileException;
import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Arrays;


import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

public class PorcupineTest {

    private static final String ENVIRONMENT_NAME;
    private Porcupine porcupine;
    private final String accessKey = System.getProperty("pvTestingAccessKey");
    private double performanceThresholdSec;

    static {
        ENVIRONMENT_NAME = Utils.getEnvironmentName();
    }

    PorcupineTest() {
        try {
            performanceThresholdSec = Double.parseDouble(System.getProperty("performanceThresholdSec"));
        } catch (Exception e) {
            performanceThresholdSec = 0f;
        }
    }

    private static String appendLanguage(String s, String language) {
        if (language.equals("en"))
            return s;
        return s + "_" + language;
    }

    private static String getTestKeywordPath(String language, String keyword) {
        return Paths.get(System.getProperty("user.dir"))
            .resolve("../../resources")
            .resolve(appendLanguage("keyword_files", language))
            .resolve(ENVIRONMENT_NAME)
            .resolve(keyword + "_" + ENVIRONMENT_NAME + ".ppn")
            .toString();
    }

    private static String getTestModelPath(String language) {
        return Paths.get(System.getProperty("user.dir"))
            .resolve("../../lib/common")
            .resolve(appendLanguage("porcupine_params", language)+".pv")
            .toString();
    }

    private static String getTestAudioFilePath(String audioFileName) {
        return Paths.get(System.getProperty("user.dir"))
            .resolve("../../resources/audio_samples")
            .resolve(audioFileName)
            .toString();
    }

    private static String[] getTestKeywordPaths(String language, String[] keywords) {
        String keywordPaths[] = new String[keywords.length];
        for (int i=0; i<keywords.length; i++) {
            keywordPaths[i] = getTestKeywordPath(language, keywords[i]);
        }
        return keywordPaths;
    }

    @AfterEach
    void tearDown() {
        porcupine.delete();
    }

    @Test
    void getVersion() throws PorcupineException {
        porcupine = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setBuiltInKeyword(Porcupine.BuiltInKeyword.PORCUPINE)
                .build();
        assertTrue(porcupine.getVersion() != null && !porcupine.getVersion().equals(""));
    }

    @Test
    void getFrameLength() throws PorcupineException {
        porcupine = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setBuiltInKeyword(Porcupine.BuiltInKeyword.PORCUPINE)
                .build();
        assertTrue(porcupine.getFrameLength() > 0);
    }

    @org.junit.jupiter.api.Test
    void getSampleRate() throws PorcupineException {
        porcupine = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setBuiltInKeyword(Porcupine.BuiltInKeyword.PORCUPINE)
                .build();
        assertTrue(porcupine.getSampleRate() > 0);
    }

    private void runTestCase(String audioFileName, ArrayList<Integer> expectedResults) throws IOException, UnsupportedAudioFileException, PorcupineException { 
        int frameLen = porcupine.getFrameLength();
        String audioFilePath = getTestAudioFilePath(audioFileName);
        File testAudioPath = new File(audioFilePath);

        AudioInputStream audioInputStream = AudioSystem.getAudioInputStream(testAudioPath);
        assertEquals(audioInputStream.getFormat().getFrameRate(), 16000);

        int byteDepth = audioInputStream.getFormat().getFrameSize();
        int bufferSize = frameLen * byteDepth;
        byte[] pcm = new byte[bufferSize];
        short[] porcupineFrame = new short[frameLen];
        int numBytesRead = 0;
        ArrayList<Integer> results = new ArrayList<>();
        while ((numBytesRead = audioInputStream.read(pcm)) != -1) {

            if (numBytesRead / byteDepth == frameLen) {

                ByteBuffer.wrap(pcm).order(ByteOrder.LITTLE_ENDIAN).asShortBuffer().get(porcupineFrame);
                int result = porcupine.process(porcupineFrame);
                assertTrue(result >= -1);
                if (result >= 0) {
                    results.add(result);
                }
            }
        }

        assertEquals(results, expectedResults);        
    }

    @Test
    void testSingleKeyword() throws IOException, UnsupportedAudioFileException, PorcupineException {
        porcupine = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setBuiltInKeyword(Porcupine.BuiltInKeyword.PORCUPINE)
                .build();

        runTestCase(
            "porcupine.wav",
            new ArrayList<>(Arrays.asList(0)));
    }

    @Test
    void testMultipleKeywords() throws IOException, UnsupportedAudioFileException, PorcupineException {
        final Porcupine.BuiltInKeyword[] keywords = new Porcupine.BuiltInKeyword[]{
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

        porcupine = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setBuiltInKeywords(keywords)
                .build();

        runTestCase(
            "multiple_keywords.wav",
            new ArrayList<>(Arrays.asList(7, 0, 1, 2, 3, 4, 5, 6, 7, 8)));
    }

    @Test
    void testSingleKeywordDe() throws IOException, UnsupportedAudioFileException, PorcupineException {
        final String language = "de";
        final String keywords[] = {"heuschrecke"};
        porcupine = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setModelPath(getTestModelPath(language))
                .setKeywordPaths(getTestKeywordPaths(language, keywords))
                .build();
        runTestCase(
            "heuschrecke.wav",
            new ArrayList<>(Arrays.asList(0)));
    }

    @Test
    void testMultipleKeywordsDe() throws IOException, UnsupportedAudioFileException, PorcupineException {
        final String language = "de";
        final String keywords[] = {"ananas", "heuschrecke", "leguan", "stachelschwein"};
        porcupine = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setModelPath(getTestModelPath(language))
                .setKeywordPaths(getTestKeywordPaths(language, keywords))
                .build();

        runTestCase(
            "multiple_keywords_de.wav",
            new ArrayList<>(Arrays.asList(0, 1, 2, 3)));
    }

    @Test
    void testSingleKeywordEs() throws IOException, UnsupportedAudioFileException, PorcupineException {
        final String language = "es";
        final String keywords[] = {"manzana"};
        porcupine = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setModelPath(getTestModelPath(language))
                .setKeywordPaths(getTestKeywordPaths(language, keywords))
                .build();

        runTestCase(
            "manzana.wav",
            new ArrayList<>(Arrays.asList(0)));
    }

    @Test
    void testMultipleKeywordsEs() throws IOException, UnsupportedAudioFileException, PorcupineException {
        final String language = "es";
        final String keywords[] = {"emparedado", "leopardo", "manzana"};
        porcupine = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setModelPath(getTestModelPath(language))
                .setKeywordPaths(getTestKeywordPaths(language, keywords))
                .build();

        runTestCase(
            "multiple_keywords_es.wav",
            new ArrayList<>(Arrays.asList(0, 1, 2)));
    }

    @Test
    void testSingleKeywordFr() throws IOException, UnsupportedAudioFileException, PorcupineException {
        final String language = "fr";
        final String keywords[] = {"mon chouchou"};
        porcupine = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setModelPath(getTestModelPath(language))
                .setKeywordPaths(getTestKeywordPaths(language, keywords))
                .build();

        runTestCase(
            "mon_chouchou.wav",
            new ArrayList<>(Arrays.asList(0)));
    }

    @Test
    void testMultipleKeywordsFr() throws IOException, UnsupportedAudioFileException, PorcupineException {
        final String language = "fr";
        final String keywords[] = {"framboise", "mon chouchou", "parapluie"};
        porcupine = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setModelPath(getTestModelPath(language))
                .setKeywordPaths(getTestKeywordPaths(language, keywords))
                .build();

        runTestCase(
            "multiple_keywords_fr.wav",
            new ArrayList<>(Arrays.asList(0, 1, 0, 2)));
    }

    @Test
    void testWithNonAsciiModelName() throws IOException, UnsupportedAudioFileException, PorcupineException {
        final String language = "es";
        final String keywords[] = {"murciélago"};
        porcupine = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setModelPath(getTestModelPath(language))
                .setKeywordPaths(getTestKeywordPaths(language, keywords))
                .build();

        runTestCase(
            "murciélago.wav",
            new ArrayList<>(Arrays.asList(0, 0)));
    }

    @Test
    @DisabledIf("systemProperty.get('performanceThresholdSec') == null || systemProperty.get('performanceThresholdSec') == ''")
    void testPerformance() throws Exception {
        porcupine = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setModelPath(getTestModelPath("en"))
                .setBuiltInKeyword(Porcupine.BuiltInKeyword.PORCUPINE)
                .build();

        int frameLen = porcupine.getFrameLength();
        String audioFilePath = getTestAudioFilePath("multiple_keywords.wav");
        File testAudioPath = new File(audioFilePath);

        AudioInputStream audioInputStream = AudioSystem.getAudioInputStream(testAudioPath);
        assertEquals(audioInputStream.getFormat().getFrameRate(), 16000);

        int byteDepth = audioInputStream.getFormat().getFrameSize();
        int bufferSize = frameLen * byteDepth;

        byte[] pcm = new byte[bufferSize];
        short[] porcupineFrame = new short[frameLen];
        int numBytesRead;

        long totalNSec = 0;
        while ((numBytesRead = audioInputStream.read(pcm)) != -1) {
            if (numBytesRead / byteDepth == frameLen) {
                ByteBuffer.wrap(pcm).order(ByteOrder.LITTLE_ENDIAN).asShortBuffer().get(porcupineFrame);
                long before = System.nanoTime();
                porcupine.process(porcupineFrame);
                long after = System.nanoTime();
                totalNSec += (after - before);
            }
        }

        double totalSec = Math.round(((double) totalNSec) * 1e-6) / 1000.0;
        assertTrue(
                totalSec <= this.performanceThresholdSec,
                String.format("Expected threshold (%.3fs), process took (%.3fs)", this.performanceThresholdSec, totalSec)
        );
    }
}
