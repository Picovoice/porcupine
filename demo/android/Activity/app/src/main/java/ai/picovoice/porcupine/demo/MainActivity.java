/*
    Copyright 2018 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is
    located in the "LICENSE" file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the
    License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
    express or implied. See the License for the specific language governing permissions and
    limitations under the License.
*/

package ai.picovoice.porcupine.demo;


import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.content.res.Resources;
import android.graphics.Color;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.text.method.LinkMovementMethod;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.RelativeLayout;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import ai.picovoice.porcupinemanager.KeywordCallback;
import ai.picovoice.porcupinemanager.PorcupineManager;
import ai.picovoice.porcupinemanager.PorcupineManagerException;


public class MainActivity extends AppCompatActivity {
    private PorcupineManager porcupineManager = null;

    private MediaPlayer notificationPlayer;

    private RelativeLayout layout;

    private ToggleButton recordButton;

    private boolean hasRecordPermission() {
        return ActivityCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) == PackageManager.PERMISSION_GRANTED;
    }

    private void requestRecordPermission() {
        ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.RECORD_AUDIO}, 0);
    }

    private void copyResourceFile(int resourceID, String filename) throws IOException {
        Resources resources = getResources();
        try (InputStream is = new BufferedInputStream(resources.openRawResource(resourceID), 256); OutputStream os = new BufferedOutputStream(openFileOutput(filename, Context.MODE_PRIVATE), 256)) {
            int r;
            while ((r = is.read()) != -1) {
                os.write(r);
            }
            os.flush();
        }
    }

    private static int[] KEYWORD_FILE_RESOURCE_IDS = {
            R.raw.americano, R.raw.blueberry, R.raw.bumblebee, R.raw.grapefruit,
            R.raw.grasshopper, R.raw.picovoice, R.raw.porcupine, R.raw.terminator,
    };

    private void copyPorcupineResourceFiles() throws IOException {
        Resources resources = getResources();

        for (int keywordFileResourceID : KEYWORD_FILE_RESOURCE_IDS) {
            copyResourceFile(keywordFileResourceID, resources.getResourceEntryName(keywordFileResourceID) + ".ppn");
        }

        copyResourceFile(R.raw.porcupine_params, resources.getResourceEntryName(R.raw.porcupine_params) + ".pv");
    }

    private void showErrorToast() {
        Toast.makeText(this, "Something went wrong", Toast.LENGTH_SHORT).show();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        try {
            copyPorcupineResourceFiles();
        } catch (IOException e) {
            Toast.makeText(this, "Failed to copy resource files", Toast.LENGTH_SHORT).show();
        }

        notificationPlayer = MediaPlayer.create(this, R.raw.notification);

        layout = findViewById(R.id.layout);

        recordButton = findViewById(R.id.record_button);

        TextView footer = findViewById(R.id.footer);
        footer.setMovementMethod(LinkMovementMethod.getInstance());

        configureKeywordSpinner();
    }

    public void process(View view) {
        try {
            if (recordButton.isChecked()) {
                if (hasRecordPermission()) {
                    porcupineManager = initPorcupine();
                    porcupineManager.start();

                } else {
                    requestRecordPermission();
                }
            } else {
                porcupineManager.stop();
            }
        } catch (PorcupineManagerException e) {
            Toast.makeText(this, "Something went wrong", Toast.LENGTH_SHORT).show();
        }
    }

    private PorcupineManager initPorcupine() throws PorcupineManagerException {
        Spinner mySpinner = findViewById(R.id.keyword_spinner);
        String kwd = mySpinner.getSelectedItem().toString();
        // It is assumed that the file name is all lower-case and spaces are replaced with "_".
        String filename = kwd.toLowerCase().replaceAll("\\s+", "_");
        // get the keyword file and model parameter file from internal storage.
        String keywordFilePath = new File(this.getFilesDir(), filename + ".ppn")
                .getAbsolutePath();
        String modelFilePath = new File(this.getFilesDir(), "porcupine_params.pv").getAbsolutePath();
        final int detectedBackgroundColor = getResources()
                .getColor(R.color.colorAccent);
        return new PorcupineManager(modelFilePath, keywordFilePath, 0.5f, new KeywordCallback() {
            @Override
            public void run(int keyword_index) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (!notificationPlayer.isPlaying()) {
                            notificationPlayer.start();
                        }
                        // change the background color for 1 second.
                        layout.setBackgroundColor(detectedBackgroundColor);
                        new CountDownTimer(1000, 100) {

                            @Override
                            public void onTick(long millisUntilFinished) {
                                if (!notificationPlayer.isPlaying()) {
                                    notificationPlayer.start();
                                }
                            }

                            @Override
                            public void onFinish() {
                                layout.setBackgroundColor(Color.TRANSPARENT);
                            }
                        }.start();
                    }
                });
            }
        });
    }

    @Override
    public void onRequestPermissionsResult(
            int requestCode,
            @NonNull String[] permissions,
            @NonNull int[] grantResults) {
        if (grantResults.length == 0 || grantResults[0] == PackageManager.PERMISSION_DENIED) {
            ToggleButton tbtn = findViewById(R.id.record_button);
            tbtn.toggle();
        } else {
            try {
                porcupineManager = initPorcupine();
                porcupineManager.start();
            } catch (PorcupineManagerException e) {
                showErrorToast();
            }
        }
    }

    private void configureKeywordSpinner() {
        Spinner spinner = findViewById(R.id.keyword_spinner);

        ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(
                this,
                R.array.keywords,
                R.layout.keyword_spinner_item);
        adapter.setDropDownViewResource(R.layout.keyword_spinner_item);
        spinner.setAdapter(adapter);

        // Make sure user stopped recording before changing the keyword.
        spinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parentView, View selectedItemView,
                                       int position, long id) {
                if (recordButton.isChecked()) {
                    if (porcupineManager != null) {
                        try {
                            porcupineManager.stop();
                        } catch (PorcupineManagerException e) {
                            showErrorToast();
                        }
                    }
                    recordButton.toggle();
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parentView) {
                // Do nothing.
            }
        });
    }
}
