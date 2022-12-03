/*
    Copyright 2021-2022 Picovoice Inc.

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
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import ai.picovoice.porcupine.PorcupineActivationException;
import ai.picovoice.porcupine.PorcupineActivationLimitException;
import ai.picovoice.porcupine.PorcupineActivationRefusedException;
import ai.picovoice.porcupine.PorcupineActivationThrottledException;
import ai.picovoice.porcupine.PorcupineException;
import ai.picovoice.porcupine.PorcupineInvalidArgumentException;
import ai.picovoice.porcupine.PorcupineManager;
import ai.picovoice.porcupine.PorcupineManagerCallback;


public class MainActivity extends AppCompatActivity {
    private static final String ACCESS_KEY = "${YOUR_ACCESS_KEY_HERE}";
    private PorcupineManager porcupineManager = null;
    private MediaPlayer notificationPlayer;
    private final PorcupineManagerCallback porcupineManagerCallback = new PorcupineManagerCallback() {
        @Override
        public void invoke(int keywordIndex) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (!notificationPlayer.isPlaying()) {
                        notificationPlayer.start();
                    }

                    final int detectedBackgroundColor = ContextCompat.getColor(
                            getApplicationContext(),
                            R.color.colorAccent);
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
    };

    private void startPorcupine() {
        try {
            final Spinner mySpinner = findViewById(R.id.keyword_spinner);
            final String keywordName = mySpinner.getSelectedItem().toString();

            String keyword = keywordName.toLowerCase().replace(" ", "_") + ".ppn";
            porcupineManager = new PorcupineManager.Builder()
                    .setAccessKey(ACCESS_KEY)
                    .setKeywordPath(keyword)
                    .setSensitivity(0.7f)
                    .build(getApplicationContext(), porcupineManagerCallback);
            porcupineManager.start();
        } catch (PorcupineInvalidArgumentException e) {
            onPorcupineInitError(
                    String.format("%s\nEnsure your accessKey '%s' is a valid access key.", e.getMessage(), ACCESS_KEY)
            );
        } catch (PorcupineActivationException e) {
            onPorcupineInitError("AccessKey activation error");
        } catch (PorcupineActivationLimitException e) {
            onPorcupineInitError("AccessKey reached its device limit");
        } catch (PorcupineActivationRefusedException e) {
            onPorcupineInitError("AccessKey refused");
        } catch (PorcupineActivationThrottledException e) {
            onPorcupineInitError("AccessKey has been throttled");
        } catch (PorcupineException e) {
            onPorcupineInitError("Failed to initialize Porcupine " + e.getMessage());
        }
    }

    private void stopPorcupine() {
        if (porcupineManager != null) {
            try {
                porcupineManager.stop();
                porcupineManager.delete();
            } catch (PorcupineException e) {
                displayError("Failed to stop Porcupine.");
            }
        }
    }

    private void onPorcupineInitError(final String errorMessage) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                TextView errorText = findViewById(R.id.errorMessage);
                errorText.setText(errorMessage);
                errorText.setVisibility(View.VISIBLE);

                ToggleButton recordButton = findViewById(R.id.record_button);
                recordButton.setBackground(ContextCompat.getDrawable(
                        getApplicationContext(),
                        R.drawable.button_disabled));
                recordButton.setChecked(false);
                recordButton.setEnabled(false);
            }
        });
    }

    private void displayError(String message) {
        Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
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
                    stopPorcupine();
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
        recordButton.setChecked(false);

        super.onStop();
    }

    private boolean hasRecordPermission() {
        return ActivityCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) == 
            PackageManager.PERMISSION_GRANTED;
    }

    private void requestRecordPermission() {
        ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.RECORD_AUDIO}, 0);
    }

    @Override
    public void onRequestPermissionsResult(
            int requestCode, 
            @NonNull String[] permissions, 
            @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (grantResults.length == 0 || grantResults[0] == PackageManager.PERMISSION_DENIED) {
            onPorcupineInitError("Microphone permission is required for this demo");
        } else {
            startPorcupine();
        }
    }

    public void process(View view) {
        ToggleButton recordButton = findViewById(R.id.record_button);
        if (recordButton.isChecked()) {
            if (hasRecordPermission()) {
                startPorcupine();
            } else {
                requestRecordPermission();
            }
        } else {
            stopPorcupine();
        }
    }
}
