/*
    Copyright 2018-2020 Picovoice Inc.

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

    @AfterEach
    void tearDown() {
        porcupine.delete();
    }

    @Test
    void getVersion() throws PorcupineException {
        porcupine = new Porcupine.Builder()
                .setKeyword("porcupine")
                .build();
        assertTrue(porcupine.getVersion() != null && !porcupine.getVersion().equals(""));
    }

    @Test
    void getFrameLength() throws PorcupineException {
        porcupine = new Porcupine.Builder()
                .setKeyword("porcupine")
                .build();
        assertTrue(porcupine.getFrameLength() > 0);
    }

    @org.junit.jupiter.api.Test
    void getSampleRate() throws PorcupineException {
        porcupine = new Porcupine.Builder()
                .setKeyword("porcupine")
                .build();
        assertTrue(porcupine.getSampleRate() > 0);
    }

    @Test
    void testProcess() throws IOException, UnsupportedAudioFileException, PorcupineException {
        porcupine = new Porcupine.Builder()
                .setKeyword("porcupine")
                .build();

        int frameLen = porcupine.getFrameLength();
        File testAudioPath = new File("../../resources/audio_samples/porcupine.wav");

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
                assertTrue(result == -1 || result == 0);
                if (result >= 0) {
                    results.add(result);
                }
            }
        }

        assertTrue(results.size() == 1 && results.get(0) == 0);
    }

    @Test
    void testProcessMultiple() throws IOException, UnsupportedAudioFileException, PorcupineException {
        final String[] keywords = new String[]{ "alexa", "americano", "blueberry", "bumblebee", "grapefruit",
                "grasshopper", "picovoice", "porcupine", "terminator"};

        porcupine = new Porcupine.Builder()
                .setKeywords(keywords)
                .build();

        int frameLen = porcupine.getFrameLength();
        File testAudioPath = new File("../../resources/audio_samples/multiple_keywords.wav");

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
                assertTrue(result >= -1 && result < keywords.length);
                if (result >= 0) {
                    results.add(result);
                }
            }
        }

        ArrayList<Integer> expectedResults = new ArrayList<>(Arrays.asList(7, 0, 1, 2, 3, 4, 5, 6, 7, 8));
        assertEquals(results, expectedResults);
    }
}