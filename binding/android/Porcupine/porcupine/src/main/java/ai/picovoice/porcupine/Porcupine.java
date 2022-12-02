/*
    Copyright 2021-2022 Picovoice Inc.

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

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Arrays;
import java.util.HashMap;

/**
 * Android binding for Porcupine wake word engine. It detects utterances of given keywords within an
 * incoming stream of audio in real-time. It processes incoming audio in consecutive frames and for
 * each frame emits the detection result. The number of samples per frame can be attained by calling
 * {@link #getFrameLength()}. The incoming audio needs to have a sample rate equal to
 * {@link #getSampleRate()} and be 16-bit linearly-encoded. Porcupine operates on single-channel
 * audio.
 */
public class Porcupine {

    private static final int[] KEYWORDS_RESOURCES = {
            R.raw.alexa, R.raw.americano, R.raw.blueberry, R.raw.bumblebee, R.raw.computer, R.raw.grapefruit,
            R.raw.grasshopper, R.raw.hey_google, R.raw.hey_siri, R.raw.jarvis, R.raw.ok_google, R.raw.picovoice,
            R.raw.porcupine, R.raw.terminator,
    };
    private static final HashMap<BuiltInKeyword, String> BUILT_IN_KEYWORD_PATHS = new HashMap<>();

    private static String DEFAULT_MODEL_PATH;
    private static boolean isExtracted;

    static {
        System.loadLibrary("pv_porcupine");
    }

    private long handle;

    /**
     * Constructor.
     *
     * @param accessKey     AccessKey obtained from Picovoice Console (https://console.picovoice.ai/).
     * @param modelPath     Absolute path to the file containing model parameters.
     * @param keywordPaths  Absolute paths to keyword model files.
     * @param sensitivities Sensitivities for detecting keywords. Each value should be a number
     *                      within [0, 1]. A higher sensitivity results in fewer misses at the cost
     *                      of increasing the false alarm rate.
     * @throws PorcupineException if there is an error while initializing Porcupine.
     */
    private Porcupine(
            String accessKey,
            String modelPath,
            String[] keywordPaths,
            float[] sensitivities) throws PorcupineException {
        handle = PorcupineNative.init(
                accessKey,
                modelPath,
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
                    BUILT_IN_KEYWORD_PATHS.put(BuiltInKeyword.valueOf(keywordName.toUpperCase()), keywordPath);
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

            if (this.accessKey == null || this.accessKey.equals("")) {
                throw new PorcupineInvalidArgumentException("No AccessKey provided to Porcupine.");
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

            return new Porcupine(accessKey, modelPath, keywordPaths, sensitivities);
        }
    }
}
