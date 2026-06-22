/*
    Copyright 2021-2026 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is
    located in the "LICENSE" file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the
    License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
    express or implied. See the License for the specific language governing permissions and
    limitations under the License.
*/

package ai.picovoice.porcupine;

import android.content.Context;
import android.content.res.Resources;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Locale;
import java.util.Set;

import okhttp3.*;

/**
 * Android binding for Porcupine wake word engine. It detects utterances of given keywords within an
 * incoming stream of audio in real-time. It processes incoming audio in consecutive frames and for
 * each frame emits the detection result. The number of samples per frame can be attained by calling
 * {@link #getFrameLength()}. The incoming audio needs to have a sample rate equal to
 * {@link #getSampleRate()} and be 16-bit linearly-encoded. Porcupine operates on single-channel
 * audio.
 */
public class Porcupine {
    private static final Set<String> VALID_LANGUAGES =
            new HashSet<>(Arrays.asList("de", "en", "es", "fr", "it", "ja", "ko", "pt"));

    private static final int PORCUPINE_PHRASE_MAX_LENGTH = 64;

    private static final String PV_API_URL = "https://rest.picovoice.ai/";

    private static final OkHttpClient client = new OkHttpClient();


    private static final int[] KEYWORDS_RESOURCES = {
            R.raw.alexa, R.raw.americano, R.raw.blueberry, R.raw.bumblebee, R.raw.computer, R.raw.grapefruit,
            R.raw.grasshopper, R.raw.hey_google, R.raw.hey_siri, R.raw.jarvis, R.raw.ok_google, R.raw.picovoice,
            R.raw.porcupine, R.raw.terminator,
    };
    private static final HashMap<BuiltInKeyword, String> BUILT_IN_KEYWORD_PATHS = new HashMap<>();

    private static String DEFAULT_MODEL_PATH;
    private static boolean isExtracted;
    private static String _sdk = "android";

    static {
        System.loadLibrary("pv_porcupine");
    }

    private long handle;

    public static void setSdk(String sdk) {
        Porcupine._sdk = sdk;
    }

    /**
     * Trains a wake word model given a phrase.
     *
     * @param accessKey AccessKey obtained from Picovoice Console (https://console.picovoice.ai/).
     * @param outputPath Absolute path to file where the trained model will be saved.
     * @param language Two character language code for the model (e.g. "en", "fr").
     *                 See https://picovoice.ai/docs/model-api/porcupine/ for supported languages.
     * @param phrase Phrase to create a wake word from.
     * @throws PorcupineException if model training fails.
     */
    public static void trainWakeWordFromPhrase(
            String accessKey,
            String outputPath,
            String language,
            String phrase) throws PorcupineException {

        if (!VALID_LANGUAGES.contains(language)) {
            throw new PorcupineInvalidArgumentException(
                    "Invalid language ('" + language + "')"
            );
        }

        if (phrase == null || phrase.isEmpty()) {
            throw new PorcupineInvalidArgumentException("Phrase must not be empty");
        }

        if (phrase.length() > PORCUPINE_PHRASE_MAX_LENGTH) {
            throw new PorcupineInvalidArgumentException(
                    "Phrase must not exceed " + PORCUPINE_PHRASE_MAX_LENGTH + " characters"
            );
        }

        String payload;

        try {
            payload = new JSONObject()
                    .put("platform", "android")
                    .put("phrase", phrase)
                    .toString();
        } catch (JSONException e) {
            throw new PorcupineRuntimeException(
                    "Failed to create request payload " + e.getMessage()
            );
        }

        Request request = new Request.Builder()
                .url(PV_API_URL + language + "/api/rhn")
                .post(RequestBody.create(
                        payload,
                        MediaType.parse("application/json")
                ))
                .addHeader("x-api-key", accessKey)
                .build();

        try (Response res = client.newCall(request).execute()) {
            if (!res.isSuccessful()) {
                String errorBody = res.body() != null ? res.body().string() : "";
                throw new PorcupineRuntimeException("Failed to train model: " + errorBody);
            }

            if (res.body() == null) {
                throw new PorcupineRuntimeException("Empty response body");
            }

            byte[] data = res.body().bytes();

            if (data.length == 0) {
                throw new PorcupineRuntimeException("Empty response body");
            }

            File file = new File(outputPath);
            try (FileOutputStream fos = new FileOutputStream(file)) {
                fos.write(data);
            } catch (IOException e) {
                throw new PorcupineRuntimeException(
                        "Failed to save Porcupine model: " + e.getMessage()
                );
            }
        } catch (IOException e) {
            throw new PorcupineRuntimeException(
                    "Request failed: " + e.getMessage()
            );
        }
    }

    /**
     * Lists all available devices that Porcupine can use for inference.
     * Each entry in the list can be used as the `device` argument when initializing Porcupine.
     *
     * @return Array of all available devices that Porcupine can be used for inference.
     * @throws PorcupineException if getting available devices fails.
     */
    public static String[] getAvailableDevices() throws PorcupineException {
        return PorcupineNative.listHardwareDevices();
    }
    
    /**
     * Constructor.
     *
     * @param accessKey     AccessKey obtained from Picovoice Console (https://console.picovoice.ai/).
     * @param modelPath     Absolute path to the file containing model parameters.
     * @param device        String representation of the device (e.g., CPU or GPU) to use for inference.
     *                      If set to `best`, the most suitable device is selected automatically. If set to `gpu`,
     *                      the engine uses the first available GPU device. To select a specific GPU device, set this
     *                      argument to `gpu:${GPU_INDEX}`, where `${GPU_INDEX}` is the index of the target GPU. If
     *                      set to `cpu`, the engine will run on the CPU with the default number of threads. To specify
     *                      the number of threads, set this argument to `cpu:${NUM_THREADS}`, where `${NUM_THREADS}`
     *                      is the desired number of threads.
     * @param keywordPaths  Absolute paths to keyword model files.
     * @param sensitivities Sensitivities for detecting keywords. Each value should be a number
     *                      within [0, 1]. A higher sensitivity results in fewer misses at the cost
     *                      of increasing the false alarm rate.
     * @throws PorcupineException if there is an error while initializing Porcupine.
     */
    private Porcupine(
            String accessKey,
            String modelPath,
            String device,
            String[] keywordPaths,
            float[] sensitivities) throws PorcupineException {
        PorcupineNative.setSdk(Porcupine._sdk);

        handle = PorcupineNative.init(
                accessKey,
                modelPath,
                device,
                keywordPaths,
                sensitivities);
    }

    /**
     * Releases resources acquired by Porcupine.
     */
    public void delete() {
        if (handle != 0) {
            PorcupineNative.delete(handle);
            handle = 0;
        }
    }

    /**
     * Processes a frame of the incoming audio stream and emits the detection result.
     *
     * @param pcm A frame of audio samples. The number of samples per frame can be attained by
     *            calling {@link #getFrameLength()}. The incoming audio needs to have a sample rate
     *            equal to {@link #getSampleRate()} and be 16-bit linearly-encoded. Porcupine
     *            operates on single-channel audio.
     * @return Index of observed keyword at the end of the current frame. Indexing is 0-based and
     *         matches the ordering of keyword models provided to the constructor. If no keyword is detected
     *         then it returns -1.
     * @throws PorcupineException if there is an error while processing the audio frame.
     */
    public int process(short[] pcm) throws PorcupineException {
        if (handle == 0) {
            throw new PorcupineInvalidStateException("Attempted to call Porcupine process after delete.");
        }
        if (pcm == null) {
            throw new PorcupineInvalidArgumentException("Passed null frame to Porcupine process.");
        }

        if (pcm.length != getFrameLength()) {
            throw new PorcupineInvalidArgumentException(
                            String.format("Porcupine process requires frames of length %d. " +
                                    "Received frame of size %d.", getFrameLength(), pcm.length));
        }

        return PorcupineNative.process(handle, pcm);
    }

    /**
     * Getter for version.
     *
     * @return Version.
     */
    public String getVersion() {
        return PorcupineNative.getVersion();
    }

    /**
     * Getter for number of audio samples per frame..
     *
     * @return Number of audio samples per frame.
     */
    public int getFrameLength() {
        return PorcupineNative.getFrameLength();
    }

    /**
     * Getter for audio sample rate accepted by Picovoice.
     *
     * @return Audio sample rate accepted by Picovoice.
     */
    public int getSampleRate() {
        return PorcupineNative.getSampleRate();
    }

    /**
     * Porcupine BuiltInKeywords.
     */
    public enum BuiltInKeyword {
        ALEXA,
        AMERICANO,
        BLUEBERRY,
        BUMBLEBEE,
        COMPUTER,
        GRAPEFRUIT,
        GRASSHOPPER,
        HEY_GOOGLE,
        HEY_SIRI,
        JARVIS,
        OK_GOOGLE,
        PICOVOICE,
        PORCUPINE,
        TERMINATOR
    }

    /**
     * Builder for creating an instance of Porcupine with a mixture of default arguments.
     */
    public static class Builder {

        private String accessKey = null;
        private String modelPath = null;
        private String device = null;
        private String[] keywordPaths = null;
        private BuiltInKeyword[] keywords = null;
        private float[] sensitivities = null;

        public Builder setAccessKey(String accessKey) {
            this.accessKey = accessKey;
            return this;
        }

        public Builder setModelPath(String modelPath) {
            this.modelPath = modelPath;
            return this;
        }

        public Builder setDevice(String device) {
            this.device = device;
            return this;
        }

        public Builder setKeywordPaths(String[] keywordPaths) {
            this.keywordPaths = keywordPaths;
            return this;
        }

        public Builder setKeywordPath(String keywordPaths) {
            this.keywordPaths = new String[]{keywordPaths};
            return this;
        }

        public Builder setKeywords(BuiltInKeyword[] keywords) {
            this.keywords = keywords;
            return this;
        }

        public Builder setKeyword(BuiltInKeyword keyword) {
            this.keywords = new BuiltInKeyword[]{keyword};
            return this;
        }

        public Builder setSensitivities(float[] sensitivities) {
            this.sensitivities = sensitivities;
            return this;
        }

        public Builder setSensitivity(float sensitivity) {
            this.sensitivities = new float[]{sensitivity};
            return this;
        }

        private void extractPackageResources(Context context) throws PorcupineIOException {
            final Resources resources = context.getResources();

            try {
                for (final int resourceId : KEYWORDS_RESOURCES) {
                    final String keywordName = resources.getResourceEntryName(resourceId);
                    final String keywordPath = extractResource(context,
                            resources.openRawResource(resourceId),
                            keywordName + ".ppn");
                    BUILT_IN_KEYWORD_PATHS.put(
                            BuiltInKeyword.valueOf(keywordName.toUpperCase(Locale.ENGLISH)), keywordPath
                    );
                }

                DEFAULT_MODEL_PATH = extractResource(context,
                        resources.openRawResource(R.raw.porcupine_params),
                        resources.getResourceEntryName(R.raw.porcupine_params) + ".pv");

                isExtracted = true;
            } catch (IOException ex) {
                throw new PorcupineIOException(ex);
            }
        }

        private String extractResource(Context context, InputStream srcFileStream, String dstFilename)
                throws IOException {
            InputStream is = new BufferedInputStream(srcFileStream, 256);
            OutputStream os = new BufferedOutputStream(context.openFileOutput(dstFilename, Context.MODE_PRIVATE), 256);
            int r;
            while ((r = is.read()) != -1) {
                os.write(r);
            }
            os.flush();

            is.close();
            os.close();
            return new File(context.getFilesDir(), dstFilename).getAbsolutePath();
        }

        /**
         * Validates properties and creates an instance of the Porcupine wake word engine.
         *
         * @param context Android app context (for extracting Porcupine resources)
         * @return An instance of Porcupine wake word engine
         * @throws PorcupineException if there is an error while initializing Porcupine.
         */
        public Porcupine build(Context context) throws PorcupineException {

            if (!isExtracted) {
                extractPackageResources(context);
            }

            if (this.accessKey == null || this.accessKey.equals("")) {
                throw new PorcupineInvalidArgumentException("No AccessKey provided to Porcupine.");
            }

            if (modelPath == null) {
                modelPath = DEFAULT_MODEL_PATH;
            } else {
                File modelFile = new File(modelPath);
                String modelFilename = modelFile.getName();
                if (!modelFile.exists() && !modelFilename.equals("")) {
                    try {
                        modelPath = extractResource(context,
                                context.getAssets().open(modelPath),
                                modelFilename);
                    } catch (IOException ex) {
                        throw new PorcupineIOException(ex);
                    }
                }
            }

            if (device == null) {
                device = "best";
            }

            if (this.keywordPaths != null && this.keywords != null) {
                throw new PorcupineInvalidArgumentException("Both 'keywords' and 'keywordPaths' were set. " +
                        "Only one of the two arguments may be set for initialization.");
            }

            if (this.keywordPaths == null) {
                if (this.keywords == null) {
                    throw new PorcupineInvalidArgumentException("Either 'keywords' or 'keywordPaths' must be set.");
                }

                this.keywordPaths = new String[keywords.length];
                for (int i = 0; i < keywords.length; i++) {
                    this.keywordPaths[i] = BUILT_IN_KEYWORD_PATHS.get(keywords[i]);
                }
            } else {
                for (int i = 0; i < keywordPaths.length; i++) {
                    if (keywordPaths[i] == null || keywordPaths[i].equals("")) {
                        throw new PorcupineInvalidArgumentException("Empty keyword path passed to Porcupine.");
                    }

                    File keywordFile = new File(keywordPaths[i]);
                    String keywordFilename = keywordFile.getName();
                    if (!keywordFile.exists() && !keywordFilename.equals("")) {
                        try {
                            keywordPaths[i] = extractResource(context,
                                    context.getAssets().open(keywordPaths[i]),
                                    keywordFilename);
                        } catch (IOException ex) {
                            throw new PorcupineIOException(ex);
                        }
                    }
                }
            }

            if (sensitivities == null) {
                sensitivities = new float[keywordPaths.length];
                Arrays.fill(sensitivities, 0.5f);
            }

            if (sensitivities.length != keywordPaths.length) {
                throw new PorcupineInvalidArgumentException(
                        String.format("Number of keywords (%d) " +
                                "does not match number of sensitivities (%d)",
                                keywordPaths.length,
                                sensitivities.length));
            }

            return new Porcupine(
                    accessKey,
                    modelPath,
                    device,
                    keywordPaths,
                    sensitivities);
        }
    }
}
