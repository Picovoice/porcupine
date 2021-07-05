package ai.picovoice.porcupine.demo;

import android.content.Context;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.platform.app.InstrumentationRegistry;

import org.junit.Before;
import org.junit.Rule;
import org.junit.After;
import com.microsoft.appcenter.espresso.Factory;
import com.microsoft.appcenter.espresso.ReportHelper;

import org.junit.Test;
import org.junit.runner.RunWith;

import ai.picovoice.porcupine.Porcupine;
import ai.picovoice.porcupine.PorcupineException;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

/**
 * Instrumentation test, which will execute on an Android device.
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
@RunWith(AndroidJUnit4.class)
public class PorcupineTest {
    @Rule
    public ReportHelper reportHelper = Factory.getReportHelper();

    @After
    public void TearDown() {
        reportHelper.label("Stopping App");
    }

    @Before
    public void Setup() {
        reportHelper.label("Starting App");
    }

    @Test
    public void useAppContext() {
        // Context of the app under test.
        Context appContext = InstrumentationRegistry.getInstrumentation().getTargetContext();
        assertEquals("ai.picovoice.porcupine.demo", appContext.getPackageName());
    }

    @Test
    public void initPorcupine() throws PorcupineException {

        // Context of the app under test.
        Context appContext = InstrumentationRegistry.getInstrumentation().getTargetContext();
        Porcupine p = new Porcupine.Builder()
                .setKeyword(Porcupine.BuiltInKeyword.PORCUPINE)
                .build(appContext);

        assertTrue(p.getVersion() != null && !p.getVersion().equals(""));

        p.delete();
    }
}