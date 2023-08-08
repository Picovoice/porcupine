/*
    Copyright 2023 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is
    located in the "LICENSE" file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the
    License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
    express or implied. See the License for the specific language governing permissions and
    limitations under the License.
*/

package ai.picovoice.porcupine.testapp;


import android.Manifest;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.RelativeLayout;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Objects;

import ai.picovoice.porcupine.Porcupine;
import ai.picovoice.porcupine.PorcupineActivationException;
import ai.picovoice.porcupine.PorcupineActivationLimitException;
import ai.picovoice.porcupine.PorcupineActivationRefusedException;
import ai.picovoice.porcupine.PorcupineActivationThrottledException;
import ai.picovoice.porcupine.PorcupineException;
import ai.picovoice.porcupine.PorcupineInvalidArgumentException;
import ai.picovoice.porcupine.PorcupineManager;
import ai.picovoice.porcupine.PorcupineManagerCallback;


public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    @Override
    protected void onStop() {
        super.onStop();
    }

    public void startTest(View view) {
        Button testButton = findViewById(R.id.test_button);
        runTest();
        testButton.setBackground(ContextCompat.getDrawable(
                getApplicationContext(),
                R.drawable.button_disabled));
    }

    public void runTest() {
        ArrayList<TestResult> results = new ArrayList<>();

        String modelFile = getModelFile();
        String[] keywords = getKeywords();

        Porcupine porcupine;
        try {
             porcupine = new Porcupine.Builder()
                    .setAccessKey("")
                    .setModelPath(modelFile)
                    .setKeywordPaths(keywords)
                    .build(getApplicationContext());
        } catch (PorcupineException e) {
            TestResult result = new TestResult();
            result.success = false;
            result.testName = "Test Init";
            result.errorMessage = String.format("Failed to init porcupine with '%s'", e);
            results.add(result);
        }

        displayTestResults(results);
    }

    private void displayTestResults(ArrayList<TestResult> results) {

    }

    private String getModelFile() {
        String suffix = (!BuildConfig.FLAVOR.equals("en")) ? String.format("_%s", BuildConfig.FLAVOR) : "";
        return String.format("models/porcupine_params%s.ppn", suffix);
    }

    private String[] getKeywords() {
        try {
            return getApplicationContext().getAssets().list("keywords");
        } catch (IOException e) {
            e.printStackTrace();
            return new String[]{};
        }
    }
}
