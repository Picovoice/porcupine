/*
 * Copyright 2018 Picovoice Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package ai.picovoice.porcupine;

import android.content.Context;
import android.support.test.InstrumentationRegistry;
import android.support.test.runner.AndroidJUnit4;

import org.junit.After;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 * Instrumented test, for Porcupine library.
 */
@RunWith(AndroidJUnit4.class)
public class PorcupineTest {
    private final static Context context = InstrumentationRegistry.getTargetContext();
    private Porcupine porcupine;

    /**
     * Copy model parameters, porcupine keyword file, and sample WAV file to the internal storage.
     * @throws IOException if there is an error while reading/writing files.
     */
    @BeforeClass
    public static void copyPorcupineConfigFiles() throws IOException {
        copyFileToInternalStorage("params.pv");
        copyFileToInternalStorage("porcupine.ppn");
        copyFileToInternalStorage("porcupine.wav");
    }

    /**
     * Initialize the Porcupine library before each test.
     * @throws PorcupineException if there is an error while initializing Porcupine library.
     */
    @Before
    public void initPorcupine() throws PorcupineException {
        // get the keyword file and model parameter file from internal storage.
        String keywordFilePath = new File(context.getFilesDir(),"porcupine.ppn")
                .getAbsolutePath();
        String modelFilePath = new File(context.getFilesDir(), "params.pv").getAbsolutePath();

        porcupine = new Porcupine(modelFilePath, keywordFilePath, 0.5f);

        assertNotNull(porcupine);
        assertEquals(512, porcupine.getFrameLength());
        assertEquals(16000, porcupine.getSampleRate());
    }

    /**
     * Release the Porcupine library resources after each test.
     */
    @After
    public void releasePorcupine() {
        porcupine.delete();
    }

    /**
     * Process the porcupine sample WAV file located in the resources directory and process it using
     * Porcupine library. There should be one occurrence of the the Porcupine in the sample file.
     * @throws PorcupineException if there is an error while processing the raw PCM data in the
     * Porcupine library.
     * @throws IOException if there is an error while reading the WAV file.
     */
    @Test
    public void processPorcupine() throws PorcupineException, IOException {
        int frameLength = porcupine.getFrameLength();
        File audioFile = new File(context.getFilesDir(),"porcupine.wav");

        WavReader wavReader = null;
        short[] shortBuffer = new short[frameLength];
        int count = 0;
        try {
            wavReader = new WavReader(audioFile);
            while (wavReader.readFrames(shortBuffer, frameLength) == frameLength) {
                if (porcupine.process(shortBuffer)) {
                    count += 1;
                }
            }
        } finally {
            if (wavReader != null) {
                wavReader.close();
            }
        }
        assertEquals(1, count);
    }

    /**
     * Copy a file from the resources directory to internal storage.
     * @param filename The name of the file.
     * @throws IOException if there is an error in the IO operations.
     */
    private static void copyFileToInternalStorage(String filename) throws IOException {
        InputStream is = null;
        OutputStream os = null;
        try {
            ClassLoader classLoader = PorcupineTest.class.getClassLoader();
            is = new BufferedInputStream(classLoader.getResourceAsStream(filename), 256);
            os = new BufferedOutputStream(context.openFileOutput(filename, Context.MODE_PRIVATE),
                    256);
            int r;
            while ((r = is.read()) != -1) {
                os.write(r);
            }
            os.flush();
        } finally {
            if (is != null) {
                is.close();
            }
            if (os != null) {
                os.close();
            }
        }
    }
}
