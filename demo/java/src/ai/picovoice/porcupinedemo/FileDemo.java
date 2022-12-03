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

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.UnsupportedAudioFileException;
import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;

public class FileDemo {

    public static void runDemo(String accessKey, File inputAudioFile, String libPath,
                               String modelPath, String[] keywordPaths, float[] sensitivities) {

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

        AudioInputStream audioInputStream;
        try {
            audioInputStream = AudioSystem.getAudioInputStream(inputAudioFile);
        } catch (UnsupportedAudioFileException e) {
            System.err.println("Audio format not supported. Please provide " +
                    "an input file of .au, .aiff or .wav format");
            return;
        } catch (IOException e) {
            System.err.println("Could not find input audio file at " + inputAudioFile);
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

            AudioFormat audioFormat = audioInputStream.getFormat();

            if (audioFormat.getSampleRate() != 16000.0f ||
                    audioFormat.getSampleSizeInBits() != 16) {
                throw new IllegalArgumentException(
                        String.format("Invalid input audio file format. " +
                                "Input file must be a %dkHz, 16-bit audio file.",
                                porcupine.getSampleRate()));
            }

            if (audioFormat.getChannels() > 1) {
                System.out.println("Picovoice processes single-channel audio, " +
                        "but a multi-channel file was provided. Processing leftmost channel only.");
            }

            int frameIndex = 0;
            long totalSamplesRead = 0;
            short[] porcupineFrame = new short[porcupine.getFrameLength()];

            ByteBuffer sampleBuffer = ByteBuffer.allocate(audioFormat.getFrameSize());
            sampleBuffer.order(ByteOrder.LITTLE_ENDIAN);
            while (audioInputStream.available() != 0) {
                totalSamplesRead++;

                int numBytesRead = audioInputStream.read(sampleBuffer.array());
                if (numBytesRead < 2) {
                    break;
                }

                porcupineFrame[frameIndex++] = sampleBuffer.getShort(0);

                if (frameIndex == porcupineFrame.length) {
                    int result = porcupine.process(porcupineFrame);
                    if (result >= 0) {
                        System.out.printf("Detected '%s' at %.02f sec\n", keywords[result],
                                totalSamplesRead / (float) porcupine.getSampleRate());
                    }

                    frameIndex = 0;
                }
            }
        } catch (Exception e) {
            System.out.println(e.toString());
        } finally {
            if (porcupine != null) {
                porcupine.delete();
            }
        }
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
            formatter.printHelp("porcupinefiledemo", options);
            System.exit(1);
            return;
        }

        if (cmd.hasOption("help")) {
            formatter.printHelp("porcupinefiledemo", options);
            return;
        }

        String accessKey = cmd.getOptionValue("access_key");
        String inputAudioPath = cmd.getOptionValue("input_audio_path");
        String libraryPath = cmd.getOptionValue("library_path");
        String modelPath = cmd.getOptionValue("model_path");
        String[] keywords = cmd.getOptionValues("keywords");
        String[] keywordPaths = cmd.getOptionValues("keyword_paths");
        String[] sensitivitiesStr = cmd.getOptionValues("sensitivities");

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
                            "Must be a floating-point number between [0,1].");
                }

                if (sensitivity < 0 || sensitivity > 1) {
                    throw new IllegalArgumentException(
                            String.format(
                                    "Failed to parse sensitivity value (%s). " +
                                    "Must be a floating-point number between [0,1].",
                                    sensitivitiesStr[i]));
                }
                sensitivities[i] = sensitivity;
            }
        }

        if (accessKey == null || accessKey.length() == 0) {
            throw new IllegalArgumentException("AccessKey is required for Porcupine.");
        }

        if (inputAudioPath == null) {
            throw new IllegalArgumentException(
                    "No input audio file provided. This is a required argument.");
        }
        File inputAudioFile = new File(inputAudioPath);
        if (!inputAudioFile.exists()) {
            throw new IllegalArgumentException(
                    String.format("Audio file at path %s does not exits.", inputAudioPath));
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

        runDemo(accessKey, inputAudioFile, libraryPath, modelPath, keywordPaths, sensitivities);
    }

    private static Options buildCommandLineOptions() {
        Options options = new Options();

        options.addOption(Option.builder("a")
                .longOpt("access_key")
                .hasArg(true)
                .desc("AccessKey obtained from Picovoice Console (https://console.picovoice.ai/).")
                .build());

        options.addOption(Option.builder("i")
                .longOpt("input_audio_path")
                .hasArg(true)
                .desc("Absolute path to input audio file.")
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
                        "List of built-in keywords for detection. Available keywords: %s",
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
        options.addOption(new Option("h", "help", false, ""));

        return options;
    }
}
