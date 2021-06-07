/*
    Copyright 2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is
    located in the "LICENSE" file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the
    License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
    express or implied. See the License for the specific language governing permissions and
    limitations under the License.
*/

package ai.picovoice.porcupine.demo;


import android.Manifest;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.RelativeLayout;
import android.widget.Spinner;
import android.widget.Toast;
import android.widget.ToggleButton;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import ai.picovoice.porcupine.Porcupine;
import ai.picovoice.porcupine.PorcupineException;
import ai.picovoice.porcupine.PorcupineManager;
import ai.picovoice.porcupine.PorcupineManagerCallback;


public class MainActivity extends AppCompatActivity {
    private PorcupineManager porcupineManager = null;

    private MediaPlayer notificationPlayer;

    private void displayError(String message) {
        Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
    }

    private PorcupineManager initPorcupine() throws PorcupineException {
        final Spinner mySpinner = findViewById(R.id.keyword_spinner);
        final String keywordName = mySpinner.getSelectedItem().toString();

        final int detectedBackgroundColor = getResources().getColor(R.color.colorAccent);

        Porcupine.BuiltInKeyword keyword = Porcupine.BuiltInKeyword.valueOf(keywordName.toUpperCase().replace(" ", "_"));
        return new PorcupineManager.Builder()
                .setKeyword(keyword)
                .setSensitivity(0.7f)
                .build(getApplicationContext(), new PorcupineManagerCallback() {
                    @Override
                    public void invoke(int keywordIndex) {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                if (!notificationPlayer.isPlaying()) {
                                    notificationPlayer.start();
                                }

                                final RelativeLayout layout = findViewById(R.id.layout);
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

    private void configureKeywordSpinner() {
        Spinner spinner = findViewById(R.id.keyword_spinner);

        ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(
                this,
                R.array.keywords,
                R.layout.keyword_spinner_item);
        adapter.setDropDownViewResource(R.layout.keyword_spinner_item);
        spinner.setAdapter(adapter);

        final ToggleButton recordButton = findViewById(R.id.record_button);

        spinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parentView, View selectedItemView, int position, long id) {
                if (recordButton.isChecked()) {
                    if (porcupineManager != null) {
                        try {
                            porcupineManager.stop();
                            porcupineManager.delete();
                        } catch (PorcupineException e) {
                            displayError("Failed to stop Porcupine.");
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

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        notificationPlayer = MediaPlayer.create(this, R.raw.notification);

        configureKeywordSpinner();
    }

    @Override
    protected void onStop() {
        ToggleButton recordButton = findViewById(R.id.record_button);
        if (recordButton.isChecked()) {
            if (porcupineManager != null) {
                try {
                    porcupineManager.stop();
                    porcupineManager.delete();
                } catch (PorcupineException e) {
                    displayError("Failed to stop Porcupine.");
                }
            }
            recordButton.toggle();
        }

        super.onStop();
    }

    private boolean hasRecordPermission() {
        return ActivityCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) == PackageManager.PERMISSION_GRANTED;
    }

    private void requestRecordPermission() {
        ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.RECORD_AUDIO}, 0);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (grantResults.length == 0 || grantResults[0] == PackageManager.PERMISSION_DENIED) {
            ToggleButton toggleButton = findViewById(R.id.record_button);
            toggleButton.toggle();
        } else {
            try {
                porcupineManager = initPorcupine();
                porcupineManager.start();
            } catch (PorcupineException e) {
                displayError("Failed to initialize Porcupine.");
            }
        }
    }

    public void process(View view) {
        ToggleButton recordButton = findViewById(R.id.record_button);
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
                porcupineManager.delete();
            }
        } catch (PorcupineException e) {
            displayError("Something went wrong");
        }
    }
}
