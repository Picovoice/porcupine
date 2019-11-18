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

package ai.picovoice.porcupinemanager;


import android.content.Context;
import android.support.test.InstrumentationRegistry;
import android.support.test.runner.AndroidJUnit4;

import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;


import static org.junit.Assert.assertNotNull;

/**
 * Instrumented test for {@link ai.picovoice.porcupinemanager.PorcupineManager}
 */
@RunWith(AndroidJUnit4.class)
public class PorcupineManagerTest {
    private final static Context context = InstrumentationRegistry.getTargetContext();

    private String keywordFilePath = new File(context.getFilesDir(),"porcupine.ppn")
            .getAbsolutePath();
    private String modelFilePath = new File(context.getFilesDir(), "params.pv")
            .getAbsolutePath();

    /**
     * Copy model parameters and porcupine keyword file to the internal storage.
     * @throws IOException if there is an error while reading/writing files.
     */
    @BeforeClass
    public static void copyPorcupineConfigFiles() throws IOException {
        copyFileToInternalStorage("params.pv");
        copyFileToInternalStorage("porcupine.ppn");
    }

    @Test(expected = PorcupineManagerException.class)
    public void initWithInvalidModelPath() throws PorcupineManagerException {
        new PorcupineManager("DummyModel.pv", keywordFilePath, 0.5f, null);
    }

    @Test(expected = PorcupineManagerException.class)
    public void initWithInvalidKeywordFilePath() throws PorcupineManagerException {
        new PorcupineManager(modelFilePath, "DummyKeywordFile.ppn", 0.5f, null);
    }

    @Test(expected = PorcupineManagerException.class)
    public void initWithInvalidSensitivity() throws PorcupineManagerException {
        new PorcupineManager(modelFilePath, keywordFilePath, 2f, null);
    }

    @Test
    public void runPorcupineManager() throws PorcupineManagerException {
        PorcupineManager porcupineManager = new PorcupineManager(modelFilePath, keywordFilePath,
                0.5f, null);
        assertNotNull(porcupineManager);
        porcupineManager.start();
        porcupineManager.stop();
    }

    @Test
    public void runPorcupineManagerMultipleKeywords() throws PorcupineManagerException {
        PorcupineManager porcupineManager = new PorcupineManager(
                modelFilePath,
                new String[]{keywordFilePath},
                new float[]{0.5f},
                null);
        assertNotNull(porcupineManager);
        porcupineManager.start();
        porcupineManager.stop();
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
            ClassLoader classLoader = PorcupineManagerTest.class.getClassLoader();
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
