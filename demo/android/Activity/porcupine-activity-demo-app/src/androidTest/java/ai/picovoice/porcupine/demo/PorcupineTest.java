package ai.picovoice.porcupine.demo;

import android.content.Context;
import android.content.res.AssetManager;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.platform.app.InstrumentationRegistry;

import com.microsoft.appcenter.espresso.Factory;
import com.microsoft.appcenter.espresso.ReportHelper;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

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

import ai.picovoice.porcupine.Porcupine;
import ai.picovoice.porcupine.PorcupineException;

import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;


@RunWith(AndroidJUnit4.class)
public class PorcupineTest {
    @Rule
    public ReportHelper reportHelper = Factory.getReportHelper();
    Context testContext;
    Context appContext;
    AssetManager assetManager;
    String testResourcesPath;

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
    }

    @Test
    public void testInitSuccessWithSingleBuiltIn() throws PorcupineException {
        Porcupine p = new Porcupine.Builder()
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
            Porcupine p = new Porcupine.Builder()
                    .setModelPath(modelPath.getAbsolutePath())
                    .setKeywordPath(keywordPath.getAbsolutePath())
                    .build(appContext);
        } catch (PorcupineException e){
            didFail = true;
        }

        assertTrue(didFail);
    }

    @Test
    public void testInitFailWithInvalidModelPath() {
        boolean didFail = false;
        File modelPath = new File(testResourcesPath, "bad_path/bad_path.pv");
        try {
            Porcupine p = new Porcupine.Builder()
                    .setModelPath(modelPath.getAbsolutePath())
                    .setKeyword(Porcupine.BuiltInKeyword.PORCUPINE)
                    .build(appContext);
        } catch (PorcupineException e){
            didFail = true;
        }

        assertTrue(didFail);
    }

    @Test
    public void testInitFailWithInvalidSensitivity() {
        boolean didFail = false;
        try {
            Porcupine p = new Porcupine.Builder()
                    .setKeyword(Porcupine.BuiltInKeyword.PORCUPINE)
                    .setSensitivity(10)
                    .build(appContext);
        } catch (PorcupineException e){
            didFail = true;
        }

        assertTrue(didFail);
    }

    @Test
    public void testInitFailWithMismatchedSensitivityCount() {
        boolean didFail = false;
        try {
            Porcupine p = new Porcupine.Builder()
                    .setKeyword(Porcupine.BuiltInKeyword.PORCUPINE)
                    .setSensitivities(new float[]{0.4f, 0.8f})
                    .build(appContext);
        } catch (PorcupineException e){
            didFail = true;
        }

        assertTrue(didFail);
    }

    @Test
    public void testInitFailWithNoKeywords() {
        boolean didFail = false;
        try {
            Porcupine p = new Porcupine.Builder()
                    .build(appContext);
        } catch (PorcupineException e){
            didFail = true;
        }

        assertTrue(didFail);
    }

    @Test
    public void testInitFailWithInvalidKeywordPath() {
        boolean didFail = false;
        File keywordPath = new File(testResourcesPath, "bad_path/bad_path.ppn");
        try {
            Porcupine p = new Porcupine.Builder()
                    .setKeywordPath(keywordPath.getAbsolutePath())
                    .build(appContext);
        } catch (PorcupineException e){
            didFail = true;
        }

        assertTrue(didFail);
    }

    @Test
    public void testInitFailWithBuiltInAndCustom() {
        File keywordPath = new File(testResourcesPath, "keyword_files/en/hey_barista_android.ppn");
        boolean didFail = false;
        try {
            Porcupine p = new Porcupine.Builder()
                    .setKeyword(Porcupine.BuiltInKeyword.PORCUPINE)
                    .setKeywordPath(keywordPath.getAbsolutePath())
                    .build(appContext);
        } catch (PorcupineException e){
            didFail = true;
        }

        assertTrue(didFail);
    }

    @Test
    public void testInitFailWithWrongKeywordPlatform() {
        boolean didFail = false;
        File keywordPath = new File(testResourcesPath, "keyword_files/en/alexa_linux.ppn");
        try {
            Porcupine p = new Porcupine.Builder()
                    .setKeywordPath(keywordPath.getAbsolutePath())
                    .build(appContext);
        } catch (PorcupineException e){
            didFail = true;
        }

        assertTrue(didFail);
    }

    @Test
    public void testProcSuccessSingleBuiltIn() throws Exception {
        Porcupine p = new Porcupine.Builder()
                .setKeyword(Porcupine.BuiltInKeyword.PORCUPINE)
                .build(appContext);

        File testAudio = new File(testResourcesPath, "audio_samples/porcupine.wav");
        FileInputStream audioInputStream = new FileInputStream(testAudio);

        byte[] rawData = new byte[p.getFrameLength() * 2];
        short[] pcm = new short[p.getFrameLength()];
        ByteBuffer pcmBuff = ByteBuffer.wrap(rawData).order(ByteOrder.LITTLE_ENDIAN);

        audioInputStream.skip(44);

        boolean keywordDetected = false;
        while (audioInputStream.available() > 0) {
            int numRead = audioInputStream.read(pcmBuff.array());
            if (numRead == p.getFrameLength() * 2) {
                pcmBuff.asShortBuffer().get(pcm);
                int keywordIndex = p.process(pcm);
                if (keywordIndex == 0) {
                    keywordDetected = true;
                }
            }
        }
        p.delete();

        assertTrue(keywordDetected);
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
                .setKeywords(inputBuiltInKeywords)
                .build(appContext);

        File testAudio = new File(testResourcesPath, "audio_samples/multiple_keywords.wav");
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
    public void testInitSuccessDE() throws PorcupineException {
        File keywordPath = new File(testResourcesPath, "keyword_files/de/ananas_android.ppn");
        File modelPath = new File(testResourcesPath, "model_files/porcupine_params_de.pv");
        Porcupine p = new Porcupine.Builder()
                .setKeywordPath(keywordPath.getAbsolutePath())
                .setModelPath(modelPath.getAbsolutePath())
                .build(appContext);

        assertTrue(p.getVersion() != null && !p.getVersion().equals(""));
        assertTrue(p.getFrameLength() > 0);
        assertTrue(p.getSampleRate() > 0);

        p.delete();
    }

    @Test
    public void testInitSuccessES() throws PorcupineException {
        File keywordPath = new File(testResourcesPath, "keyword_files/es/emparedado_android.ppn");
        File modelPath = new File(testResourcesPath, "model_files/porcupine_params_es.pv");
        Porcupine p = new Porcupine.Builder()
                .setKeywordPath(keywordPath.getAbsolutePath())
                .setModelPath(modelPath.getAbsolutePath())
                .build(appContext);

        assertTrue(p.getVersion() != null && !p.getVersion().equals(""));
        assertTrue(p.getFrameLength() > 0);
        assertTrue(p.getSampleRate() > 0);

        p.delete();
    }

    @Test
    public void testInitSuccessFR() throws PorcupineException {
        File keywordPath = new File(testResourcesPath, "keyword_files/fr/framboise_android.ppn");
        File modelPath = new File(testResourcesPath, "model_files/porcupine_params_fr.pv");
        Porcupine p = new Porcupine.Builder()
                .setKeywordPath(keywordPath.getAbsolutePath())
                .setModelPath(modelPath.getAbsolutePath())
                .build(appContext);

        assertTrue(p.getVersion() != null && !p.getVersion().equals(""));
        assertTrue(p.getFrameLength() > 0);
        assertTrue(p.getSampleRate() > 0);

        p.delete();
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
}