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

import java.io.File;
import java.io.FileInputStream;
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
    public void Setup() {
        testContext = InstrumentationRegistry.getInstrumentation().getContext();
        appContext = InstrumentationRegistry.getInstrumentation().getTargetContext();
        assetManager = testContext.getAssets();
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

        ArrayList<Long> results = new ArrayList<>();
        for (int i = 0; i < numTestIterations; i++) {
            FileInputStream audioInputStream = new FileInputStream(testAudio);

            byte[] rawData = new byte[p.getFrameLength() * 2];
            short[] pcm = new short[p.getFrameLength()];
            ByteBuffer pcmBuff = ByteBuffer.wrap(rawData).order(ByteOrder.LITTLE_ENDIAN);

            audioInputStream.skip(44);

            long totalNSec = 0;
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
            results.add(totalNSec);
        }
        p.delete();

        double avgNSec = results.stream().mapToDouble(val -> val).average().orElse(0.0);
        double avgSec = ((double) Math.round(avgNSec * 1e-6)) / 1000.0;
        assertTrue(
                String.format("Expected threshold (%.3fs), process took (%.3fs)", performanceThresholdSec, avgSec),
                avgSec <= performanceThresholdSec
        );
    }
}
