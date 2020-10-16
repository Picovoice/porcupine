package ai.picovoice.porcupinedemo;

import ai.picovoice.porcupine.Porcupine;
import org.apache.commons.cli.*;

import javax.sound.sampled.*;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.text.DateFormat;
import java.time.LocalTime;
import java.time.format.DateTimeFormatter;
import java.util.Arrays;

public class MicDemo {
    public static void RunDemo(String libPath, String modelPath,
                               String[] keywordPaths, String[] keywords, float[] sensitivities,
                               int audioDeviceIndex, String outputPath){

        Porcupine porcupine = null;
        try{
            porcupine = new Porcupine.Builder()
                    .libPath(libPath)
                    .modelPath(modelPath)
                    .keywordPaths(keywordPaths)
                    .sensitivities(sensitivities)
                    .build();

            AudioFormat format = new AudioFormat(16000f, 16, 1, true, false);
            TargetDataLine micDataLine;
            try {
                micDataLine = getAudioDevice(audioDeviceIndex, format);
                micDataLine.open(format);
            }
            catch (Exception ex){
                System.err.printf("Couldn't get capture device at the specified index (%s). " +
                        "Use --show_audio_devices to show available inputs and their indices\n", audioDeviceIndex);
                System.exit(1);
                return;
            }

//            ByteArrayOutputStream out  = new ByteArrayOutputStream();
            int numBytesRead;

            ByteBuffer captureBuffer = ByteBuffer.allocate(porcupine.getFrameLength() * 2);
            captureBuffer.order(ByteOrder.LITTLE_ENDIAN);
            short[] porcupineBuffer = new short[porcupine.getFrameLength()];
            micDataLine.start();

            DateTimeFormatter timeFormatter = DateTimeFormatter.ofPattern("HH:mm:ss");
            while (System.in.available() == 0) {
                // Read the next chunk of data from the TargetDataLine.
                numBytesRead =  micDataLine.read(captureBuffer.array(), 0, captureBuffer.capacity());
                if(numBytesRead != captureBuffer.capacity())
                    continue;

                captureBuffer.asShortBuffer().get(porcupineBuffer);
                int result = porcupine.process(porcupineBuffer);
                if(result >= 0){
                    System.out.printf("[%s] Detected '%s'\n", LocalTime.now().format(timeFormatter), keywords[result]);
                }

//                // Save this chunk of data.
//                out.write(data, 0, numBytesRead);
            }
        }
        catch (Exception ex)
        {
            System.out.println(ex.toString());
        }
        finally {
            if(porcupine != null)
                porcupine.delete();
        }
    }

    private static void showAudioDevices(){
        Mixer.Info[] allMixerInfo = AudioSystem.getMixerInfo();
        Line.Info captureLine = new Line.Info(TargetDataLine.class);

        for (int i = 0; i < allMixerInfo.length; i++) {
            Mixer mixer = AudioSystem.getMixer(allMixerInfo[i]);

            if(mixer.isLineSupported(captureLine)) {
                System.out.printf("Device %d: %s\n", i, allMixerInfo[i].getName());
            }
        }
    }

    private static TargetDataLine getAudioDevice(int deviceIndex, AudioFormat format) throws LineUnavailableException {
        DataLine.Info dataLineInfo = new DataLine.Info(TargetDataLine.class, format);

        if(deviceIndex == -1){
            if (!AudioSystem.isLineSupported(dataLineInfo)) {
                // Handle the error ...
            }

            return (TargetDataLine) AudioSystem.getLine(dataLineInfo);
        }
        else{
            Mixer.Info mixerInfo = AudioSystem.getMixerInfo()[deviceIndex];
            Mixer mixer = AudioSystem.getMixer(mixerInfo);
            if(!mixer.isLineSupported(dataLineInfo)) {

            }

            return (TargetDataLine)mixer.getLine(dataLineInfo);
        }
    }

    public static void main(String[] args) {

        Options options = BuildCommandLineOptions();
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

        if(cmd.hasOption("help")){
            formatter.printHelp("porcupinefiledemo", options);
            return;
        }

        if(cmd.hasOption("show_audio_devices")){
            showAudioDevices();
            return;
        }

        String libPath = cmd.getOptionValue("lib_path");
        String modelPath = cmd.getOptionValue("model_path");
        String[] keywords = cmd.getOptionValues("keywords");
        String[] keywordPaths = cmd.getOptionValues("keyword_paths");
        String[] sensitivitiesStr = cmd.getOptionValues("sensitivities");
        String audioDeviceIndexStr = cmd.getOptionValue("audio_device_index");
        String outputPath = cmd.getOptionValue("output_path");

        // parse sensitivity array
        float[] sensitivities = null;
        if(sensitivitiesStr != null){
            sensitivities = new float[sensitivitiesStr.length];
            try{
                for(int i = 0; i < sensitivitiesStr.length; i++){
                    float sensitivity = Float.parseFloat(sensitivitiesStr[i]);
                    if(sensitivity < 0 || sensitivity > 1){
                        throw new IllegalArgumentException(String.format("Failed to parse sensitivity value (%s). " +
                                "Must be a decimal value between [0,1].", sensitivitiesStr[i]));
                    }
                    sensitivities[i] = sensitivity;
                }
            }
            catch (Exception ex){
                throw new IllegalArgumentException("Failed to parse sensitivity value. " +
                        "Must be a decimal value between [0,1].");
            }
        }

        if(libPath ==null)
            libPath = Porcupine.LIB_PATH;

        if(modelPath == null)
            modelPath = Porcupine.MODEL_PATH;

        if(keywordPaths == null || keywordPaths.length == 0){
            if(keywords == null || keywords.length == 0){
                throw new IllegalArgumentException("Either '--keywords' or '--keyword_paths' must be set.");
            }

            if(Porcupine.KEYWORDS.containsAll(Arrays.asList(keywords))){
                keywordPaths = new String[keywords.length];
                for(int i = 0; i < keywords.length; i++){
                    keywordPaths[i] = Porcupine.KEYWORD_PATHS.get(keywords[i]);
                }
            }
            else{
                throw new IllegalArgumentException("One or more keywords are not available by default. " +
                        "Available default keywords are:\n" + String.join(",", Porcupine.KEYWORDS));
            }
        }
        else {
            if(keywords == null) {
                // in the case that only keywords file paths were given, use the file names
                keywords = new String[keywordPaths.length];
                for(int i =0; i < keywordPaths.length; i++){
                    File keywordFile = new File(keywordPaths[i]);
                    if(!keywordFile.exists())
                        throw new IllegalArgumentException(String.format("Keyword file at '%s' " +
                                "does not exist", keywordPaths[i]));
                    keywords[i] = keywordFile.getName();
                }
            }
        }

        if(sensitivities == null){
            sensitivities = new float[keywordPaths.length];
            Arrays.fill(sensitivities, 0.5f);
        }

        if(sensitivities.length != keywordPaths.length){
            throw new IllegalArgumentException(String.format("Number of keywords (%d) does " +
                    "not match number of sensitivities (%d)", keywordPaths.length, sensitivities.length));
        }

        int audioDeviceIndex = -1;
        RunDemo(libPath, modelPath, keywordPaths, keywords, sensitivities, audioDeviceIndex, outputPath);
    }

    private static Options BuildCommandLineOptions(){
        Options options = new Options();

        options.addOption(Option.builder("l")
                .longOpt("lib_path")
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
                .desc(String.format("List of default keywords for detection. Available keywords: %s",
                        String.join(",", Porcupine.KEYWORDS)))
                .build());

        options.addOption(Option.builder("kp")
                .longOpt("keyword_paths")
                .hasArgs()
                .desc("Absolute paths to keyword model files. If not set it will be populated from " +
                        "`--keywords` argument")
                .build());

        options.addOption(Option.builder("s")
                .longOpt("sensitivities")
                .hasArgs()
                .desc("Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher " +
                        "sensitivity results in fewer misses at the cost of increasing the false alarm rate. " +
                        "If not set 0.5 will be used.")
                .build());

        options.addOption(Option.builder("o")
                .longOpt("output_path")
                .hasArg(true)
                .desc("Absolute path to recorded audio for debugging.")
                .build());

        options.addOption(Option.builder("di")
                .longOpt("audio_device_index")
                .hasArg(true)
                .type(int.class)
                .desc("Index of input audio device.")
                .build());

        options.addOption(new Option("sd","show_audio_devices", false, "Print available recording devices."));
        options.addOption(new Option("h","help", false, ""));

        return options;
    }
}
