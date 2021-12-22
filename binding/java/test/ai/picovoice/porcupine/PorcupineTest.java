/*
    Copyright 2018-2021 Picovoice Inc.

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

import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.UnsupportedAudioFileException;
import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Arrays;

import java.io.FileWriter;   // Import the FileWriter class
import java.nio.file.Path;
import java.nio.file.Paths;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

public class PorcupineTest {

    private Porcupine porcupine;
    private String accessKey = System.getProperty("pvTestingAccessKey");

    private static String append_language(String s, String language) {
        if (language == "en")
            return s;
        return s + "_" + language;
    }

    private static String getKeywordPath(String language, String keyword) {
        return Paths.get(System.getProperty("user.dir"))
            .resolve("../../resources")
            .resolve(append_language("keyword_files", language))
            .resolve(Utils.ENVIRONMENT_NAME)
            .resolve(keyword + "_" + Utils.ENVIRONMENT_NAME + ".ppn")
            .toString();
    }

    private static String getModelPath(String language) {
        return Paths.get(System.getProperty("user.dir"))
            .resolve("../../lib/common")
            .resolve(append_language("porcupine_params", language)+".pv")
            .toString();
    }

    private static String getAudioFilePath(String AuadioFileName) {
        return Paths.get(System.getProperty("user.dir"))
            .resolve("../../resources/audio_samples")
            .resolve(AuadioFileName)
            .toString();
    }

    private static String[] getKeywordPaths(String language, String[] keywords) {
        String keywordPaths[] = new String[keywords.length];
        for (int i=0; i<keywords.length; i++) {
            keywordPaths[i] = getKeywordPath(language, keywords[i]);
        }
        return keywordPaths;
    }

    private void createPorcupineWrapper(String language, String[] keywords) throws PorcupineException{
        float sensitivities[] = new float[keywords.length];
        Arrays.fill(sensitivities, 0.5f);

        porcupine = new Porcupine(
                accessKey,
                Utils.getPackagedLibraryPath(),
                getModelPath(language),
                getKeywordPaths(language, keywords),
                sensitivities);
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

    private void runProcess(String audioFileName, ArrayList<Integer> expectedResults) throws IOException, UnsupportedAudioFileException, PorcupineException { 
        int frameLen = porcupine.getFrameLength();
        String audioFilePath = getAudioFilePath(audioFileName);
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

        runProcess(
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

        runProcess(
            "multiple_keywords.wav",
            new ArrayList<>(Arrays.asList(7, 0, 1, 2, 3, 4, 5, 6, 7, 8)));
    }

    @Test
    void testSingleKeywordDe() throws IOException, UnsupportedAudioFileException, PorcupineException {
        String language = "de";
        String keywords[] = {"heuschrecke"};
        createPorcupineWrapper(language, keywords);

        runProcess(
            "heuschrecke.wav",
            new ArrayList<>(Arrays.asList(0)));
    }

    @Test
    void testMultipleKeywordsDe() throws IOException, UnsupportedAudioFileException, PorcupineException {
        String language = "de";
        String keywords[] = {"ananas", "heuschrecke", "leguan", "stachelschwein"};
        createPorcupineWrapper(language, keywords);

        runProcess(
            "multiple_keywords_de.wav",
            new ArrayList<>(Arrays.asList(0, 1, 2, 3)));
    }

    @Test
    void testSingleKeywordEs() throws IOException, UnsupportedAudioFileException, PorcupineException {
        String language = "es";
        String keywords[] = {"manzana"};
        createPorcupineWrapper(language, keywords);

        runProcess(
            "manzana.wav",
            new ArrayList<>(Arrays.asList(0)));
    }

    @Test
    void testMultipleKeywordsEs() throws IOException, UnsupportedAudioFileException, PorcupineException {
        String language = "es";
        String keywords[] = {"emparedado", "leopardo", "manzana"};
        createPorcupineWrapper(language, keywords);

        runProcess(
            "multiple_keywords_es.wav",
            new ArrayList<>(Arrays.asList(0, 1, 2)));
    }

    @Test
    void testSingleKeywordFr() throws IOException, UnsupportedAudioFileException, PorcupineException {
        String language = "fr";
        String keywords[] = {"mon chouchou"};
        createPorcupineWrapper(language, keywords);

        runProcess(
            "mon_chouchou.wav",
            new ArrayList<>(Arrays.asList(0)));
    }

    @Test
    void testMultipleKeywordsFr() throws IOException, UnsupportedAudioFileException, PorcupineException {
        String language = "fr";
        String keywords[] = {"framboise", "mon chouchou", "parapluie"};
        createPorcupineWrapper(language, keywords);

        runProcess(
            "multiple_keywords_fr.wav",
            new ArrayList<>(Arrays.asList(0, 1, 0, 2)));
    }
}
