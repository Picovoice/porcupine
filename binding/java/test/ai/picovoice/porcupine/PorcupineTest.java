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

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

public class PorcupineTest {

    private Porcupine porcupine;
    private String accessKey = System.getProperty("pvTestingAccessKey");

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

    void runProcess(String audioFilePath, ArrayList<Integer> expectedResults) throws IOException, UnsupportedAudioFileException, PorcupineException { 
        int frameLen = porcupine.getFrameLength();
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
            "../../resources/audio_samples/porcupine.wav",
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
            "../../resources/audio_samples/multiple_keywords.wav",
            new ArrayList<>(Arrays.asList(7, 0, 1, 2, 3, 4, 5, 6, 7, 8)));
    }

}
