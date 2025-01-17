/*
    Copyright 2025 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is
    located in the "LICENSE" file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the
    License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
    express or implied. See the License for the specific language governing permissions and
    limitations under the License.
*/

package ai.picovoice.porcupine.testapp;

import static androidx.test.espresso.Espresso.onView;
import static androidx.test.espresso.action.ViewActions.click;
import static androidx.test.espresso.matcher.ViewMatchers.isAssignableFrom;
import static androidx.test.espresso.matcher.ViewMatchers.withId;

import android.view.View;
import android.widget.TextView;

import androidx.test.espresso.PerformException;
import androidx.test.espresso.UiController;
import androidx.test.espresso.ViewAction;
import androidx.test.espresso.intent.Intents;
import androidx.test.espresso.util.HumanReadables;
import androidx.test.ext.junit.rules.ActivityScenarioRule;
import androidx.test.ext.junit.runners.AndroidJUnit4;

import org.hamcrest.Matcher;
import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.util.concurrent.TimeoutException;

class WaitForTextAction implements ViewAction {
    private final String text;
    private final long timeout;

    public WaitForTextAction(String text, long timeout) {
        this.text = text;
        this.timeout = timeout;
    }

    @Override
    public String getDescription() {
        return String.format(
                "Wait for '%d' milliseconds for the view to have text '%s'",
                this.timeout,
                this.text
        );
    }

    @Override
    public Matcher<View> getConstraints() {
        return isAssignableFrom(TextView.class);
    }

    @Override
    public void perform(UiController uiController, View view) {
        long endTime = System.currentTimeMillis() + this.timeout;

        while (System.currentTimeMillis() < endTime) {
            TextView textView = (TextView) view;
            if (textView.getText().equals(this.text)) {
                return;
            }
            uiController.loopMainThreadForAtLeast(50);
        }

        throw new PerformException.Builder()
                .withActionDescription(this.getDescription())
                .withCause(new TimeoutException(String.format("Waited for '%d' milliseconds", this.timeout)))
                .withViewDescription(HumanReadables.describe(view))
                .build();
    }
}

@RunWith(AndroidJUnit4.class)
public class IntegrationTest {

    @Rule
    public ActivityScenarioRule<MainActivity> activityScenarioRule =
            new ActivityScenarioRule<>(MainActivity.class);

    @Before
    public void intentsInit() {
        Intents.init();
    }

    @After
    public void intentsTeardown() {
        Intents.release();
    }

    @Test
    public void testPorcupine() {
        onView(withId(R.id.testButton)).perform(click());
        onView(withId(R.id.testResult)).perform(waitForText("Passed", 60000));
    }

    private ViewAction waitForText(String text, long timeout) {
        return new WaitForTextAction(text, timeout);
    }
}
