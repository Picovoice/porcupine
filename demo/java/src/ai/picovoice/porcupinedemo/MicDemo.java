/*
    Copyright 2018-2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is
    located in the "LICENSE" file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the
    License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
    express or implied. See the License for the specific language governing permissions and
    limitations under the License.
*/

package ai.picovoice.porcupinedemo;

import ai.picovoice.porcupine.Porcupine;
import org.apache.commons.cli.*;

import javax.sound.sampled.*;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.time.LocalTime;
import java.time.format.DateTimeFormatter;
import java.util.*;

public class MicDemo {
    public static void runDemo(String accessKey, String libPath, String modelPath,
                               String[] keywordPaths, float[] sensitivities,
                               int audioDeviceIndex, String outputPath) {

        // create keywords from keyword_paths
        String[] keywords = new String[keywordPaths.length];
        for (int i = 0; i < keywordPaths.length; i++) {
            File keywordFile = new File(keywordPaths[i]);
            if (!keywordFile.exists()) {
                throw new IllegalArgumentException(String.format("Keyword file at '%s' " +
                        "does not exist", keywordPaths[i]));
            }
            keywords[i] = keywordFile.getName().split("_")[0];
        }

        // for file output
        File outputFile = null;
        ByteArrayOutputStream outputStream = null;
        long totalBytesCaptured = 0;
        AudioFormat format = new AudioFormat(16000f, 16, 1, true, false);

        // get audio capture device
        DataLine.Info dataLineInfo = new DataLine.Info(TargetDataLine.class, format);
        TargetDataLine micDataLine;
        try {
            micDataLine = getAudioDevice(audioDeviceIndex, dataLineInfo);
            micDataLine.open(format);
        } catch (LineUnavailableException e) {
            System.err.println(
                    "Failed to get a valid capture device. Use --show_audio_devices to " +
                    "show available capture devices and their indices");
            System.exit(1);
            return;
        }

        Porcupine porcupine = null;
        try {

            porcupine = new Porcupine.Builder()
                    .setAccessKey(accessKey)
                    .setLibraryPath(libPath)
                    .setModelPath(modelPath)
                    .setKeywordPaths(keywordPaths)
                    .setSensitivities(sensitivities)
                    .build();

            if (outputPath != null) {
                outputFile = new File(outputPath);
                outputStream = new ByteArrayOutputStream();
            }

            micDataLine.start();
            System.out.print("Listening for {");
            for (int i = 0; i < keywords.length; i++) {
                System.out.printf(" %s(%.02f)", keywords[i], sensitivities[i]);
            }
            System.out.print(" }\n");
            System.out.println("Press enter to stop recording...");

            // buffers for processing audio
            int frameLength = porcupine.getFrameLength();
            ByteBuffer captureBuffer = ByteBuffer.allocate(frameLength * 2);
            captureBuffer.order(ByteOrder.LITTLE_ENDIAN);
            short[] porcupineBuffer = new short[frameLength];

            int numBytesRead;
            while (System.in.available() == 0) {

                // read a buffer of audio
                numBytesRead = micDataLine.read(captureBuffer.array(), 0, captureBuffer.capacity());
                totalBytesCaptured += numBytesRead;

                // write to output if we're recording
                if (outputStream != null) {
                    outputStream.write(captureBuffer.array(), 0, numBytesRead);
                }

                // don't pass to porcupine if we don't have a full buffer
                if (numBytesRead != frameLength * 2) {
                    continue;
                }

                // copy into 16-bit buffer
                captureBuffer.asShortBuffer().get(porcupineBuffer);

                // process with porcupine
                int result = porcupine.process(porcupineBuffer);
                if (result >= 0) {
                    System.out.printf("[%s] Detected '%s'\n",
                            LocalTime.now().format(DateTimeFormatter.ofPattern("HH:mm:ss")),
                            keywords[result]);
                }
            }
        } catch (Exception e) {
            System.err.println(e.toString());
        } finally {
            if (outputStream != null && outputFile != null) {

                // need to transfer to input stream to write
                ByteArrayInputStream writeArray =
                        new ByteArrayInputStream(outputStream.toByteArray());
                AudioInputStream writeStream = new AudioInputStream(
                        writeArray,
                        format,
                        totalBytesCaptured / format.getFrameSize()
                );

                try {
                    AudioSystem.write(writeStream, AudioFileFormat.Type.WAVE, outputFile);
                } catch (IOException e) {
                    System.err.printf("Failed to write audio to '%s'.\n", outputFile.getPath());
                    e.printStackTrace();
                }
            }

            if (porcupine != null) {
                porcupine.delete();
            }
        }
    }

    private static void showAudioDevices() {

        // get available audio devices
        Mixer.Info[] allMixerInfo = AudioSystem.getMixerInfo();
        Line.Info captureLine = new Line.Info(TargetDataLine.class);

        for (int i = 0; i < allMixerInfo.length; i++) {

            // check if supports capture in the format we need
            Mixer mixer = AudioSystem.getMixer(allMixerInfo[i]);
            if (mixer.isLineSupported(captureLine)) {
                System.out.printf("Device %d: %s\n", i, allMixerInfo[i].getName());
            }
        }
    }

    private static TargetDataLine getDefaultCaptureDevice(DataLine.Info dataLineInfo)
            throws LineUnavailableException {

        if (!AudioSystem.isLineSupported(dataLineInfo)) {
            throw new LineUnavailableException(
                            "Default capture device does not support the format required " +
                            "by Picovoice (16kHz, 16-bit, linearly-encoded, single-channel PCM).");
        }

        return (TargetDataLine) AudioSystem.getLine(dataLineInfo);
    }

    private static TargetDataLine getAudioDevice(int deviceIndex, DataLine.Info dataLineInfo)
            throws LineUnavailableException {

        if (deviceIndex >= 0) {
            try {
                Mixer.Info mixerInfo = AudioSystem.getMixerInfo()[deviceIndex];
                Mixer mixer = AudioSystem.getMixer(mixerInfo);

                if (mixer.isLineSupported(dataLineInfo)) {
                    return (TargetDataLine) mixer.getLine(dataLineInfo);
                } else {
                    System.err.printf("Audio capture device at index %s does not support the " +
                            "audio format required by Picovoice. Using default capture device.",
                            deviceIndex);
                }
            } catch (Exception e) {
                System.err.printf(
                        "No capture device found at index %s. Using default capture device.",
                        deviceIndex);
            }
        }

        // use default capture device if we couldn't get the one requested
        return getDefaultCaptureDevice(dataLineInfo);
    }

    public static void main(String[] args) {

        Options options = buildCommandLineOptions();
        CommandLineParser parser = new DefaultParser();
        HelpFormatter formatter = new HelpFormatter();

        CommandLine cmd;
        try {
            cmd = parser.parse(options, args);
        } catch (ParseException e) {
            System.out.println(e.getMessage());
            formatter.printHelp("porcupinemicdemo", options);
            System.exit(1);
            return;
        }

        if (cmd.hasOption("help")) {
            formatter.printHelp("porcupinemicdemo", options);
            return;
        }

        if (cmd.hasOption("show_audio_devices")) {
            showAudioDevices();
            return;
        }

        String accessKey = cmd.getOptionValue("access_key");
        String libraryPath = cmd.getOptionValue("library_path");
        String modelPath = cmd.getOptionValue("model_path");
        String[] keywords = cmd.getOptionValues("keywords");
        String[] keywordPaths = cmd.getOptionValues("keyword_paths");
        String[] sensitivitiesStr = cmd.getOptionValues("sensitivities");
        String audioDeviceIndexStr = cmd.getOptionValue("audio_device_index");
        String outputPath = cmd.getOptionValue("output_path");

        if (accessKey == null || accessKey.length() == 0) {
            throw new IllegalArgumentException("AccessKey is required for Porcupine.");
        }

        // parse sensitivity array
        float[] sensitivities = null;
        if (sensitivitiesStr != null) {
            sensitivities = new float[sensitivitiesStr.length];

            for (int i = 0; i < sensitivitiesStr.length; i++) {
                float sensitivity;
                try {
                    sensitivity = Float.parseFloat(sensitivitiesStr[i]);
                } catch (Exception e) {
                    throw new IllegalArgumentException("Failed to parse sensitivity value. " +
                            "Must be a decimal value between [0,1].");
                }

                if (sensitivity < 0 || sensitivity > 1) {
                    throw new IllegalArgumentException(
                            String.format("Failed to parse sensitivity value (%s). " +
                            "Must be a decimal value between [0,1].", sensitivitiesStr[i]));
                }
                sensitivities[i] = sensitivity;
            }
        }

        if (libraryPath == null) {
            libraryPath = Porcupine.LIBRARY_PATH;
        }

        if (modelPath == null) {
            modelPath = Porcupine.MODEL_PATH;
        }

        if (keywordPaths == null || keywordPaths.length == 0) {
            if (keywords == null || keywords.length == 0) {
                throw new IllegalArgumentException(
                        "Either '--keywords' or '--keyword_paths' must be set.");
            }

            keywordPaths = new String[keywords.length];
            for (int i = 0; i < keywords.length; i++) {
                final String keyword = keywords[i].toUpperCase().replace(" ", "_");
                try {
                    final Porcupine.BuiltInKeyword builtInKeyword =
                            Porcupine.BuiltInKeyword.valueOf(keyword);
                    keywordPaths[i] = Porcupine.BUILT_IN_KEYWORD_PATHS.get(builtInKeyword);
                } catch (Exception e) {
                    throw new IllegalArgumentException(
                            String.format("'%s' not a built-in keyword", keyword));
                }
            }
        }

        if (sensitivities == null) {
            sensitivities = new float[keywordPaths.length];
            Arrays.fill(sensitivities, 0.5f);
        }

        if (sensitivities.length != keywordPaths.length) {
            throw new IllegalArgumentException(
                    String.format("Number of keywords (%d) does " +
                            "not match number of sensitivities (%d)",
                            keywordPaths.length,
                            sensitivities.length));
        }

        int audioDeviceIndex = -1;
        if (audioDeviceIndexStr != null) {
            try {
                audioDeviceIndex = Integer.parseInt(audioDeviceIndexStr);
                if (audioDeviceIndex < 0) {
                    throw new IllegalArgumentException(
                            String.format("Audio device index %s is not a " +
                                    "valid positive integer.",
                                    audioDeviceIndexStr));
                }
            } catch (Exception e) {
                throw new IllegalArgumentException(
                        String.format("Audio device index '%s' is not a " +
                                "valid positive integer.",
                                audioDeviceIndexStr));
            }
        }

        runDemo(accessKey,
                libraryPath,
                modelPath,
                keywordPaths,
                sensitivities,
                audioDeviceIndex,
                outputPath);
    }

    private static Options buildCommandLineOptions() {
        Options options = new Options();

        options.addOption(Option.builder("a")
                .longOpt("access_key")
                .hasArg(true)
                .desc("AccessKey obtained from Picovoice Console (https://console.picovoice.ai/).")
                .build());

        options.addOption(Option.builder("l")
                .longOpt("library_path")
                .hasArg(true)
                .desc("Absolute path to the Porcupine native runtime library.")
                .build());

        options.addOption(Option.builder("m")
                .longOpt("model_path")
                .hasArg(true)
                .desc("Absolute path to the file containing model parameters.")
                .build());

        options.addOption(Option.builder("k")
                .longOpt("keywords")
                .hasArgs()
                .desc(String.format(
                        "List of default keywords for detection. Available keywords: %s",
                        Porcupine.BuiltInKeyword.options()))
                .build());

        options.addOption(Option.builder("kp")
                .longOpt("keyword_paths")
                .hasArgs()
                .desc("Absolute paths to keyword model files.")
                .build());

        options.addOption(Option.builder("s")
                .longOpt("sensitivities")
                .hasArgs()
                .desc("Sensitivities for detecting keywords. Each value should" +
                        "be a number within [0, 1]. A higher sensitivity results in fewer " +
                        "misses at the cost of increasing the false alarm rate. If not " +
                        "set 0.5 will be used.")
                .build());

        options.addOption(Option.builder("o")
                .longOpt("output_path")
                .hasArg(true)
                .desc("Absolute path to recorded audio for debugging.")
                .build());

        options.addOption(Option.builder("di")
                .longOpt("audio_device_index")
                .hasArg(true)
                .desc("Index of input audio device.")
                .build());

        options.addOption(new Option("sd",
                "show_audio_devices",
                false,
                "Print available recording devices."));
        options.addOption(new Option("h", "help", false, ""));

        return options;
    }
}
