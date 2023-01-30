/*
    Copyright 2018-2023 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is
    located in the "LICENSE" file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the
    License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
    express or implied. See the License for the specific language governing permissions and
    limitations under the License.
*/

package ai.picovoice.porcupine;

import com.google.gson.Gson;
import com.google.gson.JsonArray;
import com.google.gson.JsonObject;
import com.google.gson.reflect.TypeToken;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.UnsupportedAudioFileException;
import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.stream.Stream;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;


public class PorcupineTest {

    private final String accessKey = System.getProperty("pvTestingAccessKey");
    private Porcupine porcupine;

    private static Stream<Arguments> singleKeywordProvider() throws IOException {
        final JsonObject testDataJson = PorcupineTestUtils.loadTestData();
        final JsonArray singleKeywordData = testDataJson
                .getAsJsonObject("tests")
                .getAsJsonArray("singleKeyword");

        final ArrayList<Arguments> testArgs = new ArrayList<>();
        for (int i = 0; i < singleKeywordData.size(); i++) {
            final JsonObject testData = singleKeywordData.get(i).getAsJsonObject();
            final String language = testData.get("language").getAsString();
            final String keyword = testData.get("wakeword").getAsString();
            final String testAudio = keyword.replace(' ', '_') + ".wav";
            testArgs.add(Arguments.of(
                    language,
                    keyword,
                    testAudio)
            );
        }
        return testArgs.stream();
    }

    private static Stream<Arguments> multipleKeywordsProvider() throws IOException {
        final JsonObject testDataJson = PorcupineTestUtils.loadTestData();
        final JsonArray multipleKeywordData = testDataJson
                .getAsJsonObject("tests")
                .getAsJsonArray("multipleKeyword");

        final Gson gson = new Gson();
        final ArrayList<Arguments> testArgs = new ArrayList<>();
        for (int i = 0; i < multipleKeywordData.size(); i++) {
            final JsonObject testData = multipleKeywordData.get(i).getAsJsonObject();
            final String language = testData.get("language").getAsString();
            final String[] keywords = gson.fromJson(testData.get("wakewords"), String[].class);
            final String testAudio = PorcupineTestUtils.appendLanguage("multiple_keywords", language) + ".wav";
            final ArrayList<Integer> groundTruth = gson.fromJson(
                    testData.get("groundTruth"),
                    new TypeToken<ArrayList<Integer>>() {
                    }.getType());
            testArgs.add(Arguments.of(
                    language,
                    keywords,
                    testAudio,
                    groundTruth)
            );

        }
        return testArgs.stream();
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

    private void runTestCase(String audioFileName, ArrayList<Integer> expectedResults)
            throws IOException, UnsupportedAudioFileException, PorcupineException {
        int frameLen = porcupine.getFrameLength();
        String audioFilePath = PorcupineTestUtils.getTestAudioFilePath(audioFileName);
        File testAudioPath = new File(audioFilePath);

        AudioInputStream audioInputStream = AudioSystem.getAudioInputStream(testAudioPath);
        assertEquals(audioInputStream.getFormat().getFrameRate(), 16000);

        int byteDepth = audioInputStream.getFormat().getFrameSize();
        int bufferSize = frameLen * byteDepth;
        byte[] pcm = new byte[bufferSize];
        short[] porcupineFrame = new short[frameLen];
        int numBytesRead;
        ArrayList<Integer> results = new ArrayList<>();
        while ((numBytesRead = audioInputStream.read(pcm)) != -1) {

            if (numBytesRead / byteDepth == frameLen) {

                ByteBuffer
                        .wrap(pcm)
                        .order(ByteOrder.LITTLE_ENDIAN)
                        .asShortBuffer()
                        .get(porcupineFrame);
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
    void testBuiltinKeywords()
            throws IOException, UnsupportedAudioFileException, PorcupineException {
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
    void testWithNonAsciiModelName()
            throws IOException, UnsupportedAudioFileException, PorcupineException {
        final String language = "es";
        final String[] keywords = {"murciélago"};
        porcupine = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setModelPath(PorcupineTestUtils.getTestModelPath(language))
                .setKeywordPaths(PorcupineTestUtils.getTestKeywordPaths(language, keywords))
                .build();

        runTestCase(
                "murciélago.wav",
                new ArrayList<>(Arrays.asList(0, 0)));
    }

    @ParameterizedTest(name = "testSingleKeyword for ''{0}''")
    @MethodSource("singleKeywordProvider")
    void testSingleKeyword(String language, String keyword, String audioFileName)
            throws IOException, UnsupportedAudioFileException, PorcupineException {
        final String[] keywords = {keyword};
        porcupine = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setModelPath(PorcupineTestUtils.getTestModelPath(language))
                .setKeywordPaths(PorcupineTestUtils.getTestKeywordPaths(language, keywords))
                .build();
        runTestCase(
                audioFileName,
                new ArrayList<>(Collections.singletonList(0)));
    }

    @ParameterizedTest(name = "testMultipleKeywords for ''{0}''")
    @MethodSource("multipleKeywordsProvider")
    void testSingleKeyword(
            String language,
            String[] keywords,
            String audioFileName,
            ArrayList<Integer> results)
            throws IOException, UnsupportedAudioFileException, PorcupineException {
        porcupine = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setModelPath(PorcupineTestUtils.getTestModelPath(language))
                .setKeywordPaths(PorcupineTestUtils.getTestKeywordPaths(language, keywords))
                .build();
        runTestCase(audioFileName, results);
    }
}
