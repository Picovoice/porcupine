package ai.picovoice.porcupine.demo;

import static org.junit.Assert.assertTrue;

import android.content.Context;
import android.content.res.AssetManager;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.platform.app.InstrumentationRegistry;

import com.microsoft.appcenter.espresso.Factory;
import com.microsoft.appcenter.espresso.ReportHelper;

import org.junit.After;
import org.junit.Assume;
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

@RunWith(AndroidJUnit4.class)
public class PerformanceTest {
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

    @Test
    public void testPerformance() throws Exception {
        String iterationString = appContext.getString(R.string.numTestIterations);
        String thresholdString = appContext.getString(R.string.performanceThresholdSec);
        Assume.assumeNotNull(thresholdString);
        Assume.assumeFalse(thresholdString.equals(""));

        int numTestIterations = 100;
        try {
            numTestIterations = Integer.parseInt(iterationString);
        } catch (NumberFormatException ignored) {}
        double performanceThresholdSec = Double.parseDouble(thresholdString);

        Porcupine p = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setKeyword(Porcupine.BuiltInKeyword.PORCUPINE)
                .build(appContext);

        File testAudio = new File(testResourcesPath, "audio_samples/multiple_keywords.wav");

        long totalNSec = 0;
        for (int i = 0; i < numTestIterations; i++) {
            FileInputStream audioInputStream = new FileInputStream(testAudio);

            byte[] rawData = new byte[p.getFrameLength() * 2];
            short[] pcm = new short[p.getFrameLength()];
            ByteBuffer pcmBuff = ByteBuffer.wrap(rawData).order(ByteOrder.LITTLE_ENDIAN);

            audioInputStream.skip(44);
            while (audioInputStream.available() > 0) {
                int numRead = audioInputStream.read(pcmBuff.array());
                if (numRead == p.getFrameLength() * 2) {
                    pcmBuff.asShortBuffer().get(pcm);
                    long before = System.nanoTime();
                    p.process(pcm);
                    long after = System.nanoTime();
                    totalNSec += (after - before);
                }
            }
        }
        p.delete();

        double avgNSec = totalNSec / (double) numTestIterations;
        double avgSec = ((double) Math.round(avgNSec * 1e-6)) / 1000.0;
        assertTrue(
                String.format("Expected threshold (%.3fs), process took (%.3fs)", performanceThresholdSec, avgSec),
                avgSec <= performanceThresholdSec
        );
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
