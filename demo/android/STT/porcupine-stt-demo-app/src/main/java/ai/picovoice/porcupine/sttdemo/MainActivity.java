/*
    Copyright 2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is
    located in the "LICENSE" file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the
    License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
    express or implied. See the License for the specific language governing permissions and
    limitations under the License.
*/

package ai.picovoice.porcupine.sttdemo;


import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.speech.RecognitionListener;
import android.speech.RecognizerIntent;
import android.speech.SpeechRecognizer;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ScrollView;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import java.util.ArrayList;
import java.util.Locale;

import ai.picovoice.porcupine.Porcupine;
import ai.picovoice.porcupine.PorcupineException;
import ai.picovoice.porcupine.PorcupineManager;
import ai.picovoice.porcupine.PorcupineManagerCallback;


public class MainActivity extends AppCompatActivity {
    private PorcupineManager porcupineManager = null;

    private ScrollView intentWrapper;
    private TextView intentTextView;
    private TextView stateTextView;

    private SpeechRecognizer speechRecognizer;
    private Intent speechRecognizerIntent;

    private void displayError(String message) {
        Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
    }

    private PorcupineManager initPorcupine() throws PorcupineException {
        final Spinner mySpinner = findViewById(R.id.keyword_spinner);
        final String keywordName = mySpinner.getSelectedItem().toString();

        stateTextView.setText("Listening for Wake Word");

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
                                stateTextView.setText("Wake Word Detected");
                                try {
                                    // needs to stop porcupine manager before speechRecognizer can start listening.
                                    porcupineManager.stop();
                                } catch (PorcupineException e) {
                                    displayError("Failed to stop porcupine.");
                                    return;
                                }

                                new CountDownTimer(1000, 1000) {
                                    @Override
                                    public void onTick(long millisUntilFinished) {
                                        // do nothing
                                    }

                                    @Override
                                    public void onFinish() {
                                        stateTextView.setText("Running STT");
                                        intentWrapper.setBackgroundColor(getResources().getColor(R.color.colorPrimaryDark));
                                        speechRecognizer.startListening(speechRecognizerIntent);
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
                    stopService();
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

        intentWrapper = findViewById(R.id.intentWrapper);
        intentTextView = findViewById(R.id.intentView);
        stateTextView = findViewById(R.id.stateText);

        stateTextView.setText("Stopped");

        // on android 11, RecognitionService has to be specifically added to android manifest.
        if (!SpeechRecognizer.isRecognitionAvailable(this)) {
            displayError("Speech Recognition not available.");
        }

        speechRecognizerIntent = new Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH);
        speechRecognizerIntent.putExtra(RecognizerIntent.EXTRA_LANGUAGE_MODEL, RecognizerIntent.LANGUAGE_MODEL_FREE_FORM);
        speechRecognizerIntent.putExtra(RecognizerIntent.EXTRA_LANGUAGE, Locale.getDefault());

        speechRecognizer = SpeechRecognizer.createSpeechRecognizer(this);
        speechRecognizer.setRecognitionListener(new SpeechListener());

        configureKeywordSpinner();
    }

    @Override
    protected void onStop() {
        ToggleButton recordButton = findViewById(R.id.record_button);
        if (recordButton.isChecked()) {
            stopService();
            recordButton.toggle();
        }

        super.onStop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        speechRecognizer.destroy();
    }

    private boolean hasRecordPermission() {
        return ActivityCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) == PackageManager.PERMISSION_GRANTED;
    }

    private void requestRecordPermission() {
        ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.RECORD_AUDIO}, 0);
    }

    private void keepSttAlive() {
        speechRecognizer.stopListening();
        speechRecognizer.startListening(speechRecognizerIntent);
    }

    private void stopService() {
        if (porcupineManager != null) {
            try {
                porcupineManager.stop();
                porcupineManager.delete();
            } catch (PorcupineException e) {
                displayError("Failed to stop porcupine.");
            }
        }
        speechRecognizer.stopListening();

        stateTextView.setText("Stopped");
        intentWrapper.setBackgroundColor(getResources().getColor(android.R.color.black));
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (grantResults.length == 0 || grantResults[0] == PackageManager.PERMISSION_DENIED) {
            ToggleButton toggleButton = findViewById(R.id.record_button);
            toggleButton.toggle();
            displayError("Permission denied.");
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
                stopService();
            }
        } catch (PorcupineException e) {
            displayError("Something went wrong");
        }
    }

    public void clearText(View view) {
        intentTextView.setText("");
    }

    private class SpeechListener implements RecognitionListener {
        @Override
        public void onReadyForSpeech(Bundle params) {
        }

        @Override
        public void onBeginningOfSpeech() {
        }

        @Override
        public void onRmsChanged(float rmsdB) {
        }

        @Override
        public void onBufferReceived(byte[] buffer) {
        }

        @Override
        public void onEndOfSpeech() {
        }

        @Override
        public void onError(int error) {
            ToggleButton recordButton = findViewById(R.id.record_button);
            switch (error) {
                case SpeechRecognizer.ERROR_AUDIO:
                    displayError("Error recording audio.");
                    break;
                case SpeechRecognizer.ERROR_INSUFFICIENT_PERMISSIONS:
                    displayError("Not enough permissions.");
                    break;
                case SpeechRecognizer.ERROR_NETWORK_TIMEOUT:
                case SpeechRecognizer.ERROR_NETWORK:
                    displayError("Network Error.");
                    break;
                case SpeechRecognizer.ERROR_NO_MATCH:
                    if (recordButton.isChecked()) {
                        displayError("No recognition result matched.");
                        keepSttAlive();
                    }
                case SpeechRecognizer.ERROR_CLIENT:
                    return;
                case SpeechRecognizer.ERROR_RECOGNIZER_BUSY:
                    displayError("Recognition service is busy.");
                    break;
                case SpeechRecognizer.ERROR_SERVER:
                    displayError("Error with server");
                    break;
                case SpeechRecognizer.ERROR_SPEECH_TIMEOUT:
                    displayError("No speech input.");
                    break;
                default:
                    displayError("Something wrong occurred.");
            }

            stopService();
            recordButton.toggle();
        }

        @Override
        public void onResults(Bundle results) {
            Log.d("RecognitionListener", "onResults()");
            ArrayList<String> data = results.getStringArrayList(SpeechRecognizer.RESULTS_RECOGNITION);
            intentTextView.append(data.get(0) + " ");

            keepSttAlive();
        }

        @Override
        public void onPartialResults(Bundle partialResults) {
        }

        @Override
        public void onEvent(int eventType, Bundle params) {
        }
    }
}
