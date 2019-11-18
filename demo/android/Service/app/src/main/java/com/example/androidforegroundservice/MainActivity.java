package com.example.androidforegroundservice;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;

import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class MainActivity extends AppCompatActivity {

    Button buttonStartService;
    Button buttonStopService;

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

        buttonStartService = findViewById(R.id.buttonStartService);
        buttonStopService = findViewById(R.id.buttonStopService);

        buttonStartService.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startService();
            }
        });

        buttonStopService.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
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
