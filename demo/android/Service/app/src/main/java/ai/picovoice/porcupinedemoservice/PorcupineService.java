package ai.picovoice.porcupinedemoservice;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.os.Build;
import android.os.IBinder;
import android.util.Log;

import androidx.annotation.Nullable;
import androidx.core.app.NotificationCompat;

import java.io.File;

import ai.picovoice.porcupinemanager.*;

public class PorcupineService extends Service {

    public static final String CHANNEL_ID = "ForegroundServiceChannel";

    private PorcupineManager porcupineManager;

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        String input = intent.getStringExtra("inputExtra");

        createNotificationChannel();

        Intent notificationIntent = new Intent(this, MainActivity.class);
        PendingIntent pendingIntent = PendingIntent.getActivity(
                this,
                0,
                notificationIntent,
                0);

        Notification notification = new NotificationCompat.Builder(this, CHANNEL_ID)
                .setContentTitle("Foreground Service")
                .setContentText(input)
                .setSmallIcon(R.drawable.ic_launcher_background)
                .setContentIntent(pendingIntent)
                .build();

        startForeground(1, notification);

        String keywordFilePath = new File(this.getFilesDir(), "porcupine_android.ppn")
                .getAbsolutePath();
        String modelFilePath = new File(this.getFilesDir(), "porcupine_params.pv").getAbsolutePath();

        try {
            porcupineManager = new PorcupineManager(
                    modelFilePath,
                    keywordFilePath,
                    0.5f,
                    (keywordIndex) -> {
                        Log.i("", "detected " + keywordIndex);
                    });
            porcupineManager.start();
        } catch (PorcupineManagerException e) {
            Log.e("", e.toString());
        }
//        audioRecorder = new AudioRecorder();
//        audioRecorder.start();

        return super.onStartCommand(intent, flags, startId);
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    private void createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel notificationChannel = new NotificationChannel(
                    CHANNEL_ID,
                    "Foreground Service Channel",
                    NotificationManager.IMPORTANCE_DEFAULT);

            NotificationManager manager = getSystemService(NotificationManager.class);
            manager.createNotificationChannel(notificationChannel);
        }
    }

    @Override
    public void onDestroy() {
        try {
            porcupineManager.stop();
//            audioRecorder.stop();
        } catch (PorcupineManagerException e) {
            Log.e("", e.toString());
        }
        super.onDestroy();
    }
}
