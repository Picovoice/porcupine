/*
    Copyright 2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is
    located in the "LICENSE" file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the
    License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
    express or implied. See the License for the specific language governing permissions and
    limitations under the License.
*/

package ai.picovoice.porcupinedemoservice;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.IBinder;
import android.util.Log;

import androidx.annotation.Nullable;
import androidx.core.app.NotificationCompat;

import ai.picovoice.porcupine.Porcupine;
import ai.picovoice.porcupine.PorcupineActivationException;
import ai.picovoice.porcupine.PorcupineActivationLimitException;
import ai.picovoice.porcupine.PorcupineActivationRefusedException;
import ai.picovoice.porcupine.PorcupineActivationThrottledException;
import ai.picovoice.porcupine.PorcupineException;
import ai.picovoice.porcupine.PorcupineInvalidArgumentException;
import ai.picovoice.porcupine.PorcupineManager;
import ai.picovoice.porcupine.PorcupineManagerCallback;

public class PorcupineService extends Service {
    private static final String CHANNEL_ID = "PorcupineServiceChannel";
    private static final String ACCESS_KEY = "${YOUR_ACCESS_KEY_HERE}";

    private PorcupineManager porcupineManager;
    private int numUtterances;
    private final PorcupineManagerCallback porcupineManagerCallback = (keywordIndex) -> {
        numUtterances++;

        final String contentText = numUtterances == 1 ? " time!" : " times!";
        Notification n = getNotification(
                "Wake word",
                "Detected " + numUtterances + contentText);

        NotificationManager notificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        assert notificationManager != null;
        notificationManager.notify(1234, n);
    };

    private void createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel notificationChannel = new NotificationChannel(
                    CHANNEL_ID,
                    "Porcupine",
                    NotificationManager.IMPORTANCE_HIGH);

            NotificationManager manager = getSystemService(NotificationManager.class);
            manager.createNotificationChannel(notificationChannel);
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {

        numUtterances = 0;
        createNotificationChannel();

        try {
            porcupineManager = new PorcupineManager.Builder()
                    .setAccessKey(ACCESS_KEY)
                    .setKeyword(Porcupine.BuiltInKeyword.COMPUTER)
                    .setSensitivity(0.7f).build(
                            getApplicationContext(),
                            porcupineManagerCallback);
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

        Notification notification = porcupineManager == null ?
                getNotification("Porcupine init failed", "Service will be shut down") :
                getNotification("Wake word", "Service running");
        startForeground(1234, notification);

        return super.onStartCommand(intent, flags, startId);
    }

    private void onPorcupineInitError(String message) {
        Intent i = new Intent("PorcupineInitError");
        i.putExtra("errorMessage", message);
        sendBroadcast(i);
    }

    private Notification getNotification(String title, String message) {
        PendingIntent pendingIntent = PendingIntent.getActivity(
                this,
                0,
                new Intent(this, MainActivity.class),
                PendingIntent.FLAG_MUTABLE);

        return new NotificationCompat.Builder(this, CHANNEL_ID)
                .setContentTitle(title)
                .setContentText(message)
                .setSmallIcon(R.drawable.ic_launcher_foreground)
                .setContentIntent(pendingIntent)
                .build();
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onDestroy() {
        if (porcupineManager != null) {
            try {
                porcupineManager.stop();
                porcupineManager.delete();
            } catch (PorcupineException e) {
                Log.e("PORCUPINE", e.toString());
            }
        }

        super.onDestroy();
    }
}
