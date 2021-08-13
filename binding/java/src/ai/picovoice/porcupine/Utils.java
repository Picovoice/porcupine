/*
    Copyright 2018-2020 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is
    located in the "LICENSE" file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the
    License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
    express or implied. See the License for the specific language governing permissions and
    limitations under the License.
*/

package ai.picovoice.porcupine;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.net.URISyntaxException;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardCopyOption;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Locale;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;
import java.util.logging.Logger;

class Utils {

    private static final Path RESOURCE_DIRECTORY;
    private static final String ENVIRONMENT_NAME;
    private final static Logger logger = Logger.getLogger(Logger.GLOBAL_LOGGER_NAME);

    private static String linuxArch;

    static {
        RESOURCE_DIRECTORY = getResourceDirectory();
        ENVIRONMENT_NAME = getEnvironmentName();
    }

    public static boolean isResourcesAvailable() {
        return RESOURCE_DIRECTORY != null;
    }

    public static boolean isEnvironmentSupported() {
        return ENVIRONMENT_NAME != null;
    }

    private static Path getResourceDirectory() {
        // location of resources, either a JAR file or a directory
        final URL resourceURL = Porcupine.class.getProtectionDomain().getCodeSource().getLocation();
        Path resourcePath;

        try {
            resourcePath = Paths.get(resourceURL.toURI());
        } catch (URISyntaxException e) {
            resourcePath = Paths.get(resourceURL.getPath());
        }

        if (resourcePath.toString().endsWith(".jar")) {
            try {
                resourcePath = extractResources(resourcePath);
            } catch (IOException e) {
                logger.severe("Failed to extract resources from Porcupine jar.");
                e.printStackTrace();
                return null;
            }
        }

        return resourcePath.resolve("porcupine");
    }

    private static Path extractResources(Path jarPath) throws IOException {
        // use jar name to create versioned extraction directory
        String extractionDirName = jarPath.getFileName().toString().replace(".jar", "");

        // extract jar resources to temp directory
        String systemTempDir = System.getProperty("java.io.tmpdir");
        Path resourceDirectoryPath = new File(systemTempDir, extractionDirName).toPath();

        // try to create tmp dir for extraction
        if (!Files.exists(resourceDirectoryPath)) {
            try {
                Files.createDirectory(resourceDirectoryPath);
            } catch (IOException e) {
                logger.severe("Failed to create extraction directory at " + jarPath.toString());
                e.printStackTrace();

                // default extraction directly to tmp
                resourceDirectoryPath = new File(systemTempDir).toPath();
            }
        }

        // read jar file entries
        JarFile jf = new JarFile(jarPath.toFile());
        Enumeration<JarEntry> entries = jf.entries();

        while (entries.hasMoreElements()) {
            JarEntry jarEntry = entries.nextElement();
            String jarEntryName = jarEntry.getName();


            if (jarEntryName.startsWith("porcupine")) {
                // copy contents into resource directory
                if (jarEntry.isDirectory()) {
                    Path dstPath = resourceDirectoryPath.resolve(jarEntryName);
                    if (!dstPath.toFile().exists())
                        Files.createDirectory(dstPath);
                } else {
                    Path file = resourceDirectoryPath.resolve(jarEntryName);
                    try (InputStream is = jf.getInputStream(jarEntry)) {
                        Files.copy(is, file, StandardCopyOption.REPLACE_EXISTING);
                    }
                }
            }

        }

        return resourceDirectoryPath;
    }

    private static String getEnvironmentName() throws RuntimeException {
        String arch = System.getProperty("os.arch");
        String os = System.getProperty("os.name", "generic").toLowerCase(Locale.ENGLISH);
        if (arch.equals("amd64") || arch.equals("x86_64")) {
            if (os.contains("mac") || os.contains("darwin")) {
                return "mac";
            } else if (os.contains("win")) {
                return "windows";
            } else if (os.contains("linux")) {
                linuxArch = "x86_64";
                return "linux";
            } else {
                logger.severe("Execution environment not supported. " +
                        "Porcupine Java is supported on MacOS, Linux and Windows");
                return null;
            }
        } else if (os.contains("linux") && (arch.equals("arm") || arch.equals("arm64"))) {
            return getLinuxArch();
        } else {
            logger.severe(String.format("Platform architecture (%s) not supported. " +
                    "Porcupine Java is only supported on amd64 and x86_64 architectures.", arch));
            return null;
        }
    }

    private static String getLinuxArch() {
        String cpuPath;
        try {
            cpuPath = Files.lines(Paths.get("/proc/cpuinfo"))
                    .filter(line -> line.startsWith("CPU Part"))
                    .map(line -> line.substring(line.lastIndexOf(" ") + 1))
                    .findFirst()
                    .orElse("");
        } catch (IOException e) {
            logger.severe("Could not get CPU information.");
            return null;
        }

        String archInfo = (System.getProperty("os.arch").equals("arm64")) ? "-aarch64" : "";

        switch (cpuPath) {
            case "0xb76":
                linuxArch = "arm11" + archInfo;
                return "raspberry-pi";
            case "0xc07":
                linuxArch = "cortex-a7" + archInfo;
                return "raspberry-pi";
            case "0xd03":
                linuxArch = "cortex-a53" + archInfo;
                return "raspberry-pi";
            case "0xd07":
                linuxArch = "cortex-a57" + archInfo;
                return "jetson";
            case "0xd08":
                linuxArch = "cortex-a72" + archInfo;
                return "raspberry-pi";
            case "0xc08":
                linuxArch = "";
                return "beaglebone";
            default:
                logger.severe(String.format("CPU Part (%s) not supported.", cpuPath));
                return null;
        }
    }

    public static String getPackagedModelPath() {
        return RESOURCE_DIRECTORY.resolve("lib/common/porcupine_params.pv").toString();
    }

    public static HashMap<String, String> getPackagedKeywordPaths() {
        HashMap<String, String> keywordPaths = new HashMap<>();
        Path keywordFileDir = RESOURCE_DIRECTORY.resolve("resources/keyword_files").resolve(ENVIRONMENT_NAME);
        File[] keywordFiles = keywordFileDir.toFile().listFiles();

        if (keywordFiles == null || keywordFiles.length == 0) {
            logger.severe("Couldn't find any Porcupine keywords in jar.");
            return keywordPaths;
        }

        for (File keywordFile : keywordFiles) {
            keywordPaths.put(keywordFile.getName().split("_")[0], keywordFile.getAbsolutePath());
        }
        return keywordPaths;
    }

    public static String getPackagedLibraryPath() {
        if (ENVIRONMENT_NAME == null) {
            return null;
        }

        switch (ENVIRONMENT_NAME) {
            case "windows":
                return RESOURCE_DIRECTORY.resolve("lib/java/windows/amd64/pv_porcupine_jni.dll").toString();
            case "mac":
                return RESOURCE_DIRECTORY.resolve("lib/java/mac/x86_64/libpv_porcupine_jni.dylib").toString();
            case "linux":
                System.out.printf("os: %s, cpu: %s\n", ENVIRONMENT_NAME, linuxArch);
                return RESOURCE_DIRECTORY.resolve("lib/java")
                        .resolve(ENVIRONMENT_NAME)
                        .resolve(linuxArch)
                        .resolve("libpv_porcupine_jni.so").toString();
            default:
                return null;
        }
    }
}
