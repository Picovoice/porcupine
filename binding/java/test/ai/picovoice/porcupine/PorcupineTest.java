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
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.stream.Stream;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.UnsupportedAudioFileException;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

public class PorcupineTest {

    private final String accessKey = System.getProperty("pvTestingAccessKey");
    private Porcupine porcupine;

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
        String audioFilePath = PorcupineTestUtils.getTestAudioFilePath(audioFileName);
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
    void testBuiltinKeywords() throws IOException, UnsupportedAudioFileException, PorcupineException {
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

    @ParameterizedTest(name = "testSingleKeyword for ''{0}''")
    @MethodSource("singleKeywordProvider")
    void testSingleKeyword(String language, String keyword, String audioFileName) throws IOException, UnsupportedAudioFileException, PorcupineException {
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

    private static Stream<Arguments> singleKeywordProvider() {
        return Stream.of(
                Arguments.of("en", "porcupine", "porcupine.wav"),
                Arguments.of("de", "heuschrecke", "heuschrecke.wav"),
                Arguments.of("es", "manzana", "manzana.wav"),
                Arguments.of("fr", "mon chouchou", "mon_chouchou.wav")
        );
    }

    @ParameterizedTest(name = "testMultipleKeywords for ''{0}''")
    @MethodSource("multipleKeywordsProvider")
    void testSingleKeyword(String language, String[] keywords, String audioFileName, ArrayList results) throws IOException, UnsupportedAudioFileException, PorcupineException {
        porcupine = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setModelPath(PorcupineTestUtils.getTestModelPath(language))
                .setKeywordPaths(PorcupineTestUtils.getTestKeywordPaths(language, keywords))
                .build();
        runTestCase(audioFileName, results);
    }

    private static Stream<Arguments> multipleKeywordsProvider() {
        return Stream.of(
                Arguments.of(
                        "de",
                        new String[] {"ananas", "heuschrecke", "leguan", "stachelschwein"},
                        "multiple_keywords_de.wav", new ArrayList<>(Arrays.asList(0, 1, 2, 3))
                ),
                Arguments.of(
                        "es",
                        new String[] {"emparedado", "leopardo", "manzana"},
                        "multiple_keywords_es.wav", new ArrayList<>(Arrays.asList(0, 1, 2))
                ),
                Arguments.of(
                        "fr",
                        new String[] {"framboise", "mon chouchou", "parapluie"},
                        "multiple_keywords_fr.wav", new ArrayList<>(Arrays.asList(0, 1, 0, 2))
                ),
                Arguments.of(
                        "it",
                        new String[] {"espresso", "cameriere", "porcospino"},
                        "multiple_keywords_it.wav", new ArrayList<>(Arrays.asList(2, 0, 1))
                ),
                Arguments.of(
                        "ja",
                        new String[] {"ninja", "bushi", "ringo"},
                        "multiple_keywords_ja.wav", new ArrayList<>(Arrays.asList(2, 1, 0))
                ),
                Arguments.of(
                        "ko",
                        new String[] {"aiseukeulim", "bigseubi", "koppulso"},
                        "multiple_keywords_ko.wav", new ArrayList<>(Arrays.asList(1, 2, 0))
                ),
                Arguments.of(
                        "pt",
                        new String[] {"abacaxi", "fenomeno", "formiga"},
                        "multiple_keywords_pt.wav", new ArrayList<>(Arrays.asList(0, 2, 1))
                )
        );
    }

    @Test
    void testWithNonAsciiModelName() throws IOException, UnsupportedAudioFileException, PorcupineException {
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
}
