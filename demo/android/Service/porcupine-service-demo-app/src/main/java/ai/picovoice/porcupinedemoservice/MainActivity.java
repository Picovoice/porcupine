/*
    Copyright 2021-2024 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is
    located in the "LICENSE" file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the
    License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
    express or implied. See the License for the specific language governing permissions and
    limitations under the License.
*/

package ai.picovoice.porcupinedemoservice;

import android.Manifest;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;
import android.widget.ToggleButton;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import java.util.ArrayList;

public class MainActivity extends AppCompatActivity {

    private ServiceBroadcastReceiver receiver;

    private boolean hasRecordPermission() {
        return ActivityCompat.checkSelfPermission(
                this,
                Manifest.permission.RECORD_AUDIO) == PackageManager.PERMISSION_GRANTED;
    }

    private boolean hasNotificationPermission() {
        return Build.VERSION.SDK_INT < Build.VERSION_CODES.TIRAMISU ||
                ActivityCompat.checkSelfPermission(
                        this,
                        Manifest.permission.POST_NOTIFICATIONS) == PackageManager.PERMISSION_GRANTED;
    }

    private void requestRecordPermissions(String[] permissions) {
        ActivityCompat.requestPermissions(
                this,
                permissions,
                0);
    }

    @Override
    public void onRequestPermissionsResult(
            int requestCode, 
            @NonNull String[] permissions, 
            @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (grantResults.length == 0 || grantResults[0] == PackageManager.PERMISSION_DENIED) {
            onPorcupineInitError("Microphone/notification permissions are required for this demo");
        } else {
            startService();
        }
    }

    private void startService() {
        Intent serviceIntent = new Intent(this, PorcupineService.class);
        ContextCompat.startForegroundService(this, serviceIntent);
    }

    private void stopService() {
        Intent serviceIntent = new Intent(this, PorcupineService.class);
        stopService(serviceIntent);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        receiver = new ServiceBroadcastReceiver();

        ToggleButton recordButton = findViewById(R.id.startButton);

        recordButton.setOnClickListener(v -> {
            if (recordButton.isChecked()) {
                ArrayList<String> permissionsToRequest  = new ArrayList<>();
                if (!hasNotificationPermission()) {
                    permissionsToRequest.add(Manifest.permission.POST_NOTIFICATIONS);
                }
                if (!hasRecordPermission()) {
                    permissionsToRequest.add(Manifest.permission.RECORD_AUDIO);
                }

                if (permissionsToRequest.size() == 0) {
                    startService();
                } else {
                    requestRecordPermissions(permissionsToRequest.toArray(new String[0]));
                }
            } else {
                stopService();
            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
        registerReceiver(receiver, new IntentFilter("PorcupineInitError"));
    }

    @Override
    protected void onDestroy() {
        unregisterReceiver(receiver);
        super.onDestroy();
    }

    private void onPorcupineInitError(final String errorMessage) {
        runOnUiThread(() -> {
            TextView errorText = findViewById(R.id.errorMessage);
            errorText.setText(errorMessage);
            errorText.setVisibility(View.VISIBLE);

            ToggleButton recordButton = findViewById(R.id.startButton);
            recordButton.setBackground(ContextCompat.getDrawable(
                    getApplicationContext(),
                    R.drawable.button_disabled));
            recordButton.setChecked(false);
            recordButton.setEnabled(false);
            stopService();
        });
    }

    public class ServiceBroadcastReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            onPorcupineInitError(intent.getStringExtra("errorMessage"));
        }
    }
}
