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
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.speech.RecognitionListener;
import android.speech.RecognizerIntent;
import android.speech.SpeechRecognizer;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import java.util.ArrayList;
import java.util.Locale;

import ai.picovoice.porcupine.Porcupine;
import ai.picovoice.porcupine.PorcupineActivationException;
import ai.picovoice.porcupine.PorcupineActivationLimitException;
import ai.picovoice.porcupine.PorcupineActivationRefusedException;
import ai.picovoice.porcupine.PorcupineActivationThrottledException;
import ai.picovoice.porcupine.PorcupineException;
import ai.picovoice.porcupine.PorcupineInvalidArgumentException;
import ai.picovoice.porcupine.PorcupineManager;
import ai.picovoice.porcupine.PorcupineManagerCallback;

enum AppState {
    STOPPED,
    WAKEWORD,
    STT
}

public class MainActivity extends AppCompatActivity {
    private PorcupineManager porcupineManager = null;

    private static final String ACCESS_KEY = "${YOUR_ACCESS_KEY_HERE}";

    private final Porcupine.BuiltInKeyword defaultKeyword = Porcupine.BuiltInKeyword.PORCUPINE;

    private TextView intentTextView;
    private ToggleButton recordButton;

    private SpeechRecognizer speechRecognizer;
    private Intent speechRecognizerIntent;

    private AppState currentState;

    private void displayError(String message) {
        Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
    }

    private final PorcupineManagerCallback porcupineManagerCallback = new PorcupineManagerCallback() {
        @Override
        public void invoke(int keywordIndex) {
            runOnUiThread(() -> {
                intentTextView.setText("");
                try {
                    // need to stop porcupine manager before speechRecognizer can start listening.
                    porcupineManager.stop();
                } catch (PorcupineException e) {
                    displayError("Failed to stop Porcupine.");
                    return;
                }

                speechRecognizer.startListening(speechRecognizerIntent);
                currentState = AppState.STT;
            });
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        intentTextView = findViewById(R.id.intentView);
        recordButton = findViewById(R.id.record_button);

        // on android 11, RecognitionService has to be specifically added to android manifest.
        if (!SpeechRecognizer.isRecognitionAvailable(this)) {
            displayError("Speech Recognition not available.");
        }

        speechRecognizerIntent = new Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH);
        speechRecognizerIntent.putExtra(RecognizerIntent.EXTRA_PARTIAL_RESULTS, true);
        speechRecognizerIntent.putExtra(
                RecognizerIntent.EXTRA_LANGUAGE_MODEL, 
                RecognizerIntent.LANGUAGE_MODEL_FREE_FORM);
        speechRecognizerIntent.putExtra(RecognizerIntent.EXTRA_LANGUAGE, Locale.getDefault());

        try {
            porcupineManager = new PorcupineManager.Builder()
                    .setAccessKey(ACCESS_KEY)
                    .setKeyword(defaultKeyword)
                    .setSensitivity(0.7f)
                    .build(getApplicationContext(), porcupineManagerCallback);

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

        currentState = AppState.STOPPED;
    }

    private void onPorcupineInitError(final String errorMessage) {
        runOnUiThread(() -> {
            TextView errorText = findViewById(R.id.errorMessage);
            errorText.setText(errorMessage);
            errorText.setVisibility(View.VISIBLE);

            ToggleButton recordButton = findViewById(R.id.record_button);
            recordButton.setBackground(ContextCompat.getDrawable(
                    getApplicationContext(),
                    R.drawable.disabled_button_background));
            recordButton.setChecked(false);
            recordButton.setEnabled(false);
        });
    }

    @Override
    protected void onStop() {
        if (recordButton.isChecked()) {
            stopService();
            recordButton.toggle();
            speechRecognizer.destroy();
        }

        super.onStop();
    }

    private boolean hasRecordPermission() {
        return ActivityCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) == 
            PackageManager.PERMISSION_GRANTED;
    }

    private void requestRecordPermission() {
        ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.RECORD_AUDIO}, 0);
    }

    private void playback(int milliSeconds) {
        speechRecognizer.stopListening();
        currentState = AppState.WAKEWORD;

        new Handler(Looper.getMainLooper()).postDelayed(new Runnable() {
            @Override
            public void run() {
                if (currentState == AppState.WAKEWORD) {
                    porcupineManager.start();
                    intentTextView.setTextColor(Color.WHITE);
                    intentTextView.setText("Listening for " + defaultKeyword + " ...");
                }
            }
        }, milliSeconds);
    }

    private void stopService() {
        if (porcupineManager != null) {
            try {
                porcupineManager.stop();
            } catch (PorcupineException e) {
                displayError("Failed to stop porcupine.");
            }
        }

        intentTextView.setText("");
        speechRecognizer.stopListening();
        speechRecognizer.destroy();
        currentState = AppState.STOPPED;
    }

    @Override
    public void onRequestPermissionsResult(
            int requestCode,
            @NonNull String[]
            permissions,
            @NonNull int[] grantResults
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (grantResults.length == 0 || grantResults[0] == PackageManager.PERMISSION_DENIED) {
            onPorcupineInitError("Microphone permission is required for this demo");
        } else {
            speechRecognizer = SpeechRecognizer.createSpeechRecognizer(this);
            speechRecognizer.setRecognitionListener(new SpeechListener());
            playback(0);
        }
    }

    public void process(View view) {
        if (recordButton.isChecked()) {
            if (hasRecordPermission()) {
                speechRecognizer = SpeechRecognizer.createSpeechRecognizer(this);
                speechRecognizer.setRecognitionListener(new SpeechListener());
                playback(0);
            } else {
                requestRecordPermission();
            }
        } else {
            stopService();
        }
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
            switch (error) {
                case SpeechRecognizer.ERROR_AUDIO:
                    displayError("Error recording audio.");
                    break;
                case SpeechRecognizer.ERROR_INSUFFICIENT_PERMISSIONS:
                    displayError("Insufficient permissions.");
                    break;
                case SpeechRecognizer.ERROR_NETWORK_TIMEOUT:
                case SpeechRecognizer.ERROR_NETWORK:
                    displayError("Network Error.");
                    break;
                case SpeechRecognizer.ERROR_NO_MATCH:
                    if (recordButton.isChecked()) {
                        displayError("No recognition result matched.");
                        playback(1000);
                    }
                    return;
                case SpeechRecognizer.ERROR_CLIENT:
                    return;
                case SpeechRecognizer.ERROR_RECOGNIZER_BUSY:
                    displayError("Recognition service is busy.");
                    break;
                case SpeechRecognizer.ERROR_SERVER:
                    displayError("Server Error.");
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
            ArrayList<String> data = results.getStringArrayList(SpeechRecognizer.RESULTS_RECOGNITION);
            intentTextView.setTextColor(Color.WHITE);
            intentTextView.setText(data.get(0));

            playback(3000);
        }

        @Override
        public void onPartialResults(Bundle partialResults) {
            ArrayList<String> data = partialResults.getStringArrayList(SpeechRecognizer.RESULTS_RECOGNITION);
            intentTextView.setTextColor(Color.DKGRAY);
            intentTextView.setText(data.get(0));
        }

        @Override
        public void onEvent(int eventType, Bundle params) {
        }
    }
}
