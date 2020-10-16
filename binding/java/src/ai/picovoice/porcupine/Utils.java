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

public class Utils {

    private static final Path resourceDirectory;

    static {

        // location of resources, either a JAR file or a directory
        URL resourceURL = Porcupine.class.getProtectionDomain().getCodeSource().getLocation();
        Path resourcePath;

        try {
            resourcePath = Paths.get(resourceURL.toURI());
        }
        catch (URISyntaxException e) {
            System.err.println("Couldn't convert resource path to URI, using path instead.");
            e.printStackTrace();
            resourcePath = Paths.get(resourceURL.getPath());
        }

        if(resourcePath.toString().endsWith(".jar"))
        {
            // use jar name to create versioned extraction directory
            String extractionDirName = resourcePath.getFileName().toString().replace(".jar", "");

            // extract jar resources to temp directory
            String systemTempDir = System.getProperty("java.io.tmpdir");
            Path resourceDirectoryPath = new File(systemTempDir, extractionDirName).toPath();

            // try to create tmp dir for extraction
            if (!Files.exists(resourceDirectoryPath)) {
                try {
                    Files.createDirectory(resourceDirectoryPath);
                } catch (IOException ioException) {
                    System.err.printf("Failed to create extraction directory at %s\n", resourcePath.toString());
                    ioException.printStackTrace();

                    // default extraction directly to tmp
                    resourceDirectoryPath = new File(systemTempDir).toPath();
                }
            }
            resourceDirectory = resourceDirectoryPath;

            try{
                // read jar file entries
                JarFile jf = new JarFile(resourcePath.toFile());
                Enumeration<JarEntry> entries = jf.entries();

                while (entries.hasMoreElements()) {
                    JarEntry jarEntry = entries.nextElement();
                    String jarEntryName = jarEntry.getName();

                    try {
                        if(jarEntryName.startsWith("jniLibs") || jarEntryName.startsWith("resources") || jarEntryName.startsWith("lib")){
                            // copy contents into resource directory
                            if (jarEntry.isDirectory()) {
                                Path dstPath = resourceDirectory.resolve(jarEntryName);
                                if(!dstPath.toFile().exists())
                                    Files.createDirectory(dstPath);
                            } else {
                                Path file = resourceDirectory.resolve(jarEntryName);
                                if(!Files.exists(file)) {
                                    try (InputStream is = jf.getInputStream(jarEntry)) {
                                        Files.copy(is, file, StandardCopyOption.REPLACE_EXISTING);
                                    }
                                }
                            }
                        }
                    }
                    catch (IOException ex)
                    {
                        System.err.printf("Failed to copy resource %s from Porcupine jar.\n", jarEntryName);
                        ex.printStackTrace();
                    }
                }
            }
            catch (IOException ex){
                System.err.println("Failed to access resources from Porcupine jar.\n");
                ex.printStackTrace();
            }
        }
        else{
            resourceDirectory = resourcePath;
        }
    }

    private static String getEnvironmentName()
    {
        String os = System.getProperty("os.name", "generic").toLowerCase(Locale.ENGLISH);
        if (os.contains("mac") || os.contains("darwin")) {
            return "mac";
        } else if (os.contains("win")) {
            return "windows";
        } else if (os.contains("linux")) {
            return "linux";
        }
        else{
            return "unknown";
        }
    }

    public static String pvModelPath(){
        return resourceDirectory.resolve("lib/common/porcupine_params.pv").toString();
    }

    public static HashMap<String, String> pvKeywordPaths() throws IOException{
        HashMap<String, String> keywordPaths = new HashMap<>();
        Path keywordFileDir = resourceDirectory.resolve("resources/keyword_files").resolve(getEnvironmentName());
        File[] keywordFiles = keywordFileDir.toFile().listFiles();
        if (keywordFiles == null || keywordFiles.length == 0)
            throw new IOException("Couldn't find any Porcupine keywords in jar.");

        for (File keywordFile : keywordFiles) {
            keywordPaths.put(keywordFile.getName().split("_")[0], keywordFile.getAbsolutePath());
        }
        return keywordPaths;
    }

    public static String pvLibraryPath(){
        String os = System.getProperty("os.name", "generic").toLowerCase(Locale.ENGLISH);
        String arch = System.getProperty("os.arch");
        if(arch.equals("amd64") || arch.equals("x86_64"))
        {
            if(os.contains("win")){
                return resourceDirectory.resolve("jniLibs/windows/amd64/pv_porcupine_jni.dll").toString();
            }
            else if(os.contains("mac") || os.contains("darwin")){
                return resourceDirectory.resolve("jniLibs/mac/x86_64/libpv_porcupine_jni.dylib").toString();
            }
            else if(os.contains("linux")){
                return resourceDirectory.resolve("jniLibs/linux/x86_64/libpv_porcupine_jni.so").toString();
            }
            else
                throw new RuntimeException(String.format("Platform OS (%s) not supported", os));
        }
        else{
            throw new RuntimeException(String.format("Platform architecture (%s) not supported. " +
                    "Porcupine is only supported on 64-bit architectures.", arch));
        }
    }
}
