package ai.picovoice.porcupinedemoservice;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.res.Resources;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;
import android.widget.ToggleButton;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class MainActivity extends AppCompatActivity {

    ToggleButton startButton;

    private boolean hasRecordPermission() {
        return ActivityCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) == PackageManager.PERMISSION_GRANTED;
    }

    private void requestRecordPermission() {
        ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.RECORD_AUDIO}, 0);
    }

    private void showErrorToast() {
        Toast.makeText(this, "Something went wrong", Toast.LENGTH_SHORT).show();
    }

    private static void copyPorcupineConfigFiles(Context context) {
        int[] resIds = {
                R.raw.porcupine_params, R.raw.porcupine_android
        };
        Resources resources = context.getResources();
        for (int resId : resIds) {
            String filename = resources.getResourceEntryName(resId);
            String fileExtension = resId == R.raw.porcupine_params ? ".pv" : ".ppn";
            InputStream is = null;
            OutputStream os = null;
            try {
                is = new BufferedInputStream(resources.openRawResource(resId),
                        256);
                os = new BufferedOutputStream(context.openFileOutput(filename + fileExtension,
                        Context.MODE_PRIVATE), 256);
                int r;
                while ((r = is.read()) != -1) {
                    os.write(r);
                }
                os.flush();
            } catch (IOException e) {
                //
            } finally {
                try {
                    if (is != null) {
                        is.close();
                    }
                    if (os != null) {
                        os.close();
                    }
                } catch (IOException e) {
                    //
                }
            }
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        copyPorcupineConfigFiles(this);

        startButton = findViewById(R.id.startButton);

        startButton.setOnClickListener(v -> {
            if (startButton.isChecked()) {
                if (hasRecordPermission()) {
                    startService();
                } else {
                    requestRecordPermission();
                }
            } else {
                stopService();
            }
        });
    }

    public void startService() {
        Intent serviceIntent = new Intent(this, PorcupineService.class);
        serviceIntent.putExtra("inputExtra", "blah blah blah");

        ContextCompat.startForegroundService(this, serviceIntent);
    }

    public void stopService() {
        Intent serviceIntent = new Intent(this, PorcupineService.class);
        stopService(serviceIntent);
    }
}
