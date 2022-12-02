/*
    Copyright 2018-2021 Picovoice Inc.

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
    private static final String ARCHITECTURE;
    private static final Logger logger = Logger.getLogger(Logger.GLOBAL_LOGGER_NAME);

    static {
        RESOURCE_DIRECTORY = getResourceDirectory();
        ENVIRONMENT_NAME = getEnvironmentName();
        ARCHITECTURE = getArchitecture();
    }

    public static boolean isResourcesAvailable() {
        return RESOURCE_DIRECTORY != null;
    }

    public static boolean isEnvironmentSupported() {
        return ENVIRONMENT_NAME != null;
    }

    private static Path getResourceDirectory() throws RuntimeException {
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
                throw new RuntimeException("Failed to extract resources from Porcupine jar.");
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
                    if (!dstPath.toFile().exists()) {
                        Files.createDirectory(dstPath);
                    }
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

    public static String getEnvironmentName() throws RuntimeException {
        String os = System.getProperty("os.name", "generic").toLowerCase(Locale.ENGLISH);
        if (os.contains("mac") || os.contains("darwin")) {
            return "mac";
        } else if (os.contains("win")) {
            return "windows";
        } else if (os.contains("linux")) {
            String arch = System.getProperty("os.arch");
            if (arch.equals("arm") || arch.equals("aarch64")) {
                String cpuPart = getCpuPart();
                switch (cpuPart) {
                    case "0xc07":
                    case "0xd03":
                    case "0xd08":
                        return "raspberry-pi";
                    case "0xd07":
                        return "jetson";
                    case "0xc08":
                        return "beaglebone";
                    default:
                        throw new RuntimeException(String.format("Execution environment " +
                                "not supported. Porcupine Java does not support CPU Part (%s).",
                                cpuPart));
                }
            }
            return "linux";
        } else {
            throw new RuntimeException("Execution environment not supported. " +
                    "Porcupine Java is supported on MacOS, Linux and Windows");
        }
    }

    private static String getArchitecture() throws RuntimeException {
        String arch = System.getProperty("os.arch");
        boolean isArm = arch.equals("arm") || arch.equals("aarch64");
        boolean isX86_64 = arch.equals("amd64") || arch.equals("x86_64");

        if (ENVIRONMENT_NAME.equals("mac")) {
            if (isArm) {
                return "arm64";
            } else if (isX86_64) {
                return "x86_64";
            }
        } else if (ENVIRONMENT_NAME.equals("windows")) {
            if (isX86_64) {
                return "amd64";
            }
        } else if (ENVIRONMENT_NAME.equals("linux")) {
            if (isX86_64) {
                return "x86_64";
            }
        } else if (isArm) {  // RPI, Beaglebone, etc..
            String cpuPart = getCpuPart();
            String archInfo = (arch.equals("aarch64")) ? "-aarch64" : "";

            switch (cpuPart) {
                case "0xc07":
                    return "cortex-a7" + archInfo;
                case "0xd03":
                    return "cortex-a53" + archInfo;
                case "0xd07":
                    return "cortex-a57" + archInfo;
                case "0xd08":
                    return "cortex-a72" + archInfo;
                case "0xc08":
                    return "";
                default:
                    throw new RuntimeException(
                            String.format("Environment (%s) with CPU Part (%s) is " +
                                    "not supported by Porcupine.",
                                    ENVIRONMENT_NAME,
                                    cpuPart)
                    );
            }
        }

        throw new RuntimeException(
                String.format("Environment (%s) with architecture (%s) " +
                        "is not supported by Porcupine.",
                        ENVIRONMENT_NAME,
                        arch)
        );
    }

    private static String getCpuPart() throws RuntimeException {
        try {
            return Files.lines(Paths.get("/proc/cpuinfo"))
                    .filter(line -> line.startsWith("CPU part"))
                    .map(line -> line.substring(line.lastIndexOf(" ") + 1))
                    .findFirst()
                    .orElse("");
        } catch (IOException e) {
            throw new RuntimeException("Porcupine failed to get get CPU information.");
        }
    }

    public static String getPackagedModelPath() {
        return RESOURCE_DIRECTORY.resolve("lib/common/porcupine_params.pv").toString();
    }

    public static HashMap<Porcupine.BuiltInKeyword, String> getPackagedKeywordPaths() {
        HashMap<Porcupine.BuiltInKeyword, String> keywordPaths = new HashMap<>();
        Path keywordFileDir = RESOURCE_DIRECTORY
                .resolve("resources/keyword_files")
                .resolve(ENVIRONMENT_NAME);
        File[] keywordFiles = keywordFileDir.toFile().listFiles();

        if (keywordFiles == null || keywordFiles.length == 0) {
            logger.severe("Couldn't find any Porcupine keywords in jar.");
            return keywordPaths;
        }

        for (File keywordFile : keywordFiles) {
            final String keywordName = keywordFile
                    .getName().split("_")[0].toUpperCase().replace(' ', '_');
            keywordPaths.put(
                    Porcupine.BuiltInKeyword.valueOf(keywordName),
                    keywordFile.getAbsolutePath());
        }

        return keywordPaths;
    }

    public static String getPackagedLibraryPath() {
        switch (ENVIRONMENT_NAME) {
            case "windows":
                return RESOURCE_DIRECTORY
                        .resolve("lib/java/windows/amd64/pv_porcupine_jni.dll")
                        .toString();
            case "mac":
                return RESOURCE_DIRECTORY.resolve("lib/java/mac")
                                         .resolve(ARCHITECTURE)
                                         .resolve("libpv_porcupine_jni.dylib").toString();
            case "jetson":
            case "beaglebone":
            case "raspberry-pi":
            case "linux":
                return RESOURCE_DIRECTORY.resolve("lib/java")
                        .resolve(ENVIRONMENT_NAME)
                        .resolve(ARCHITECTURE)
                        .resolve("libpv_porcupine_jni.so").toString();
            default:
                return null;
        }
    }
}
