package ai.picovoice.porcupine;

import java.io.File;
import java.util.HashMap;
import java.util.Locale;
import java.util.Objects;

public class Utils {

    private static String getEnvironment()
    {
        String os = System.getProperty("os.name", "generic").toLowerCase(Locale.ENGLISH);
        if (os.contains("mac") || os.contains("darwin")) {
            return "mac";
        } else if (os.contains("win")) {
            return "windows";
        } else if (os.contains("nux")) {
            return "linux";
        }
        else{
            return "unknown";
        }
    }

    public static String pvModelPath(){
        return "../../lib/common/porcupine_params.pv";
    }

    public static HashMap<String, String> pvKeywordPaths()
    {
        File keywordFilesDir = new File("../../resources/keyword_files", getEnvironment());

        HashMap<String, String> keywordPaths = new HashMap<>();
        for (File keywordFile: Objects.requireNonNull(keywordFilesDir.listFiles())) {
            keywordPaths.put(keywordFile.getName().split("_")[0], keywordFile.getAbsolutePath());
        }

        return keywordPaths;
    }
}
