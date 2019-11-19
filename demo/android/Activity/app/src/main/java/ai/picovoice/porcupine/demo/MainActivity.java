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

    private void copyPorcupineConfigFiles() {
        int[] resIds = {
                R.raw.americano, R.raw.blueberry, R.raw.bumblebee, R.raw.grapefruit,
                R.raw.grasshopper, R.raw.picovoice, R.raw.porcupine, R.raw.hey_pico,
                R.raw.terminator, R.raw.params
        };
        Resources resources = getResources();
        for (int resId : resIds) {
            String filename = resources.getResourceEntryName(resId);
            String fileExtension = resId == R.raw.params ? ".pv" : ".ppn";
            InputStream is = null;
            OutputStream os = null;
            try {
                is = new BufferedInputStream(resources.openRawResource(resId),
                        256);
                os = new BufferedOutputStream(openFileOutput(filename + fileExtension,
                        Context.MODE_PRIVATE), 256);
                int r;
                while ((r = is.read()) != -1) {
                    os.write(r);
                }
                os.flush();
            } catch (IOException e) {
                showErrorToast();
            } finally {
                try {
                    if (is != null) {
                        is.close();
                    }
                    if (os != null) {
                        os.close();
                    }
                } catch (IOException e) {
                    showErrorToast();
                }
            }
        }
    }

    private void showErrorToast() {
        Toast.makeText(this, "Something went wrong", Toast.LENGTH_SHORT).show();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        copyPorcupineConfigFiles();
        notificationPlayer = MediaPlayer.create(this, R.raw.notification);
        layout = findViewById(R.id.layout);
        recordButton = findViewById(R.id.record_button);

        // Make the footer linkable.
        TextView footer = findViewById(R.id.footer);
        footer.setMovementMethod(LinkMovementMethod.getInstance());
        // create the keyword spinner.
        configureKeywordSpinner();
    }

    /**
     * Handler for the record button. Processes the audio and uses Porcupine library to detect the
     * keyword. It increments a counter to indicate the occurrence of a keyword.
     *
     * @param view ToggleButton used for recording audio.
     */
    public void process(View view) {
        try {
            if (recordButton.isChecked()) {
                // check if record permission was given.
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
            showErrorToast();
        }
    }

    /**
     * Initialize the porcupineManager library.
     *
     * @return Porcupine instance.
     */
    private PorcupineManager initPorcupine() throws PorcupineManagerException {
        Spinner mySpinner = findViewById(R.id.keyword_spinner);
        String kwd = mySpinner.getSelectedItem().toString();
        // It is assumed that the file name is all lower-case and spaces are replaced with "_".
        String filename = kwd.toLowerCase().replaceAll("\\s+", "_");
        // get the keyword file and model parameter file from internal storage.
        String keywordFilePath = new File(this.getFilesDir(), filename + ".ppn")
                .getAbsolutePath();
        String modelFilePath = new File(this.getFilesDir(), "params.pv").getAbsolutePath();
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

    /**
     * Check the result of the record permission request.
     *
     * @param requestCode  request code of the permission request.
     * @param permissions  requested permissions.
     * @param grantResults results of the permission requests.
     */
    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        // We only ask for record permission.
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

    /**
     * Configure the style and behaviour of the keyword spinner.
     */
    private void configureKeywordSpinner() {
        Spinner spinner = findViewById(R.id.keyword_spinner);

        ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(
                this, R.array.keywords, R.layout.keyword_spinner_item);
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
