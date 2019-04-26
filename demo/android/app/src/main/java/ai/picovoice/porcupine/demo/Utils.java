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
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.content.res.Resources;
import android.support.v4.app.ActivityCompat;
import android.widget.Toast;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;


/**
 * Utility class.
 */
final class Utils {
    /**
     * Copy the keyword config files to the internal storage if it is the first time user is running
     * the app.
     * @param context application context.
     */
    static void configurePorcupine(Context context) {
        // Copy the config files to the internal storage.
        copyPorcupineConfigFiles(context);
    }

    /**
     * Check whether user granted record permission.
     * @param context application context.
     */
    static boolean hasRecordPermission(Context context) {
        int permResult = ActivityCompat.checkSelfPermission(context,
                Manifest.permission.RECORD_AUDIO);
        return permResult == PackageManager.PERMISSION_GRANTED;
    }

    /**
     * Show a dialog to user and ask for record permission.
     * @param activity application activity.
     */
    static void showRecordPermission(Activity activity) {
        ActivityCompat.requestPermissions(activity,
                new String[]{Manifest.permission.RECORD_AUDIO}, 0);
    }

    /**
     * Copy the keyword config files used by Porcupine library to the internal storage of the app.
     */
    private static void copyPorcupineConfigFiles(Context context) {
        int[] resIds = {
                R.raw.americano, R.raw.blueberry, R.raw.bumblebee, R.raw.grapefruit,
                R.raw.grasshopper, R.raw.picovoice, R.raw.porcupine, R.raw.hey_pico,
                R.raw.terminator, R.raw.params
        };
        Resources resources = context.getResources();
        for (int resId : resIds) {
            String filename = resources.getResourceEntryName(resId);
            String fileExtension = resId == R.raw.params ? ".pv" : ".ppn";
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
                showErrorToast(context);
            } finally {
                try {
                    if (is != null) {
                        is.close();
                    }
                    if (os != null) {
                        os.close();
                    }
                } catch (IOException e) {
                    showErrorToast(context);
                }
            }
        }
    }

    static void showErrorToast(Context context) {
        Toast.makeText(context, "Something went wrong", Toast.LENGTH_SHORT).show();
    }
}
