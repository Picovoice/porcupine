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


import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;

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
import java.util.HashMap;

import ai.picovoice.porcupine.Porcupine;
import ai.picovoice.porcupine.PorcupineException;


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
        Button testButton = findViewById(R.id.testButton);
        testButton.setBackground(ContextCompat.getDrawable(
                getApplicationContext(),
                R.drawable.button_disabled));
        runTest();

        testButton.setBackground(ContextCompat.getDrawable(
                getApplicationContext(),
                R.drawable.button_background));
    }

    public void runTest() {
        String accessKey = getApplicationContext().getString(R.string.pvTestingAccessKey);

        ArrayList<TestResult> results = new ArrayList<>();

        String modelFile = getModelFile();
        String[] keywords = getKeywords();
        String[] keywordPaths = new String[keywords.length];
        for (int i = 0; i < keywords.length; i++) {
            keywordPaths[i] = String.format("keywords/%s", keywords[i]);
        }

        TestResult result = new TestResult();
        result.testName = "Test Init";
        Porcupine porcupine = null;
        try {
            porcupine = new Porcupine.Builder()
                    .setAccessKey(accessKey)
                    .setModelPath(modelFile)
                    .setKeywordPaths(keywordPaths)
                    .build(getApplicationContext());
            result.success = true;
        } catch (PorcupineException e) {
            result.success = false;
            result.errorMessage = String.format("Failed to init porcupine with '%s'", e);
        } finally {
            results.add(result);
        }

        result = new TestResult();
        result.testName = "Test Process";
        try {
            String suffix = "_" + BuildConfig.FLAVOR;
            if (BuildConfig.FLAVOR == "en") {
                suffix = "";
            }

            String audioPath = "audio_samples/multiple_keywords" + suffix + ".wav";

            ArrayList<Integer> processResult = processTestAudio(porcupine, audioPath);
            if (processResult.size() > 0) {
                result.success = true;
            } else {
                result.success = false;
                result.errorMessage = "Process returned invalid result.";
            }
        } catch (Exception e) {
            result.success = false;
            result.errorMessage = String.format("Failed to process with '%s'", e);
        } finally {
            results.add(result);
        }

        result = new TestResult();
        result.testName = "Test Exception";
        try {
            new Porcupine.Builder()
                    .setAccessKey("")
                    .setModelPath(modelFile)
                    .setKeywordPaths(keywordPaths)
                    .build(getApplicationContext());
            result.success = false;
            result.errorMessage = "Init should have throw an exception";
        } catch (PorcupineException e) {
            result.success = true;
        } finally {
            results.add(result);
        }

        displayTestResults(results);
    }

    private void displayTestResults(ArrayList<TestResult> results) {
        ListView resultList = findViewById(R.id.resultList);

        int passed = 0;
        int failed = 0;

        ArrayList<HashMap<String, String>> list = new ArrayList<>();
        for (TestResult result : results) {
            HashMap<String, String> map = new HashMap<>();
            map.put("testName", result.testName);

            String message;
            if (result.success) {
                message = "Test Passed";
                passed += 1;
            } else {
                message = String.format("Test Failed: %s", result.errorMessage);
                failed += 1;
            }

            map.put("testMessage", message);
            list.add(map);
        }

        SimpleAdapter adapter = new SimpleAdapter(
                getApplicationContext(),
                list,
                R.layout.list_view,
                new String[]{"testName", "testMessage"},
                new int[]{R.id.testName, R.id.testMessage});

        resultList.setAdapter(adapter);

        TextView passedView = findViewById(R.id.testNumPassed);
        TextView failedView = findViewById(R.id.testNumFailed);

        passedView.setText(String.valueOf(passed));
        failedView.setText(String.valueOf(failed));

        TextView resultView = findViewById(R.id.testResult);
        if (passed == 0 || failed > 0) {
            resultView.setText("Failed");
        } else {
            resultView.setText("Passed");
        }
    }

    private String getModelFile() {
        String suffix = (!BuildConfig.FLAVOR.equals("en")) ? String.format("_%s", BuildConfig.FLAVOR) : "";
        return String.format("models/porcupine_params%s.pv", suffix);
    }

    private String[] getKeywords() {
        try {
            return getApplicationContext().getAssets().list("keywords");
        } catch (IOException e) {
            e.printStackTrace();
            return new String[]{};
        }
    }

    private ArrayList<Integer> processTestAudio(@NonNull Porcupine p, String audioPath) throws Exception {
        File testAudio = new File(getApplicationContext().getFilesDir(), audioPath);

        if (!testAudio.exists()) {
            testAudio.getParentFile().mkdirs();
            extractFile(audioPath);
        }

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
        return detectionResults;
    }

    private void extractFile(String filepath) throws IOException {
        System.out.println(filepath);
        InputStream is = new BufferedInputStream(getAssets().open(filepath), 256);
        File absPath = new File(getApplicationContext().getFilesDir(), filepath);
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
