/*
    Copyright 2022-2023 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is
    located in the "LICENSE" file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the
    License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
    express or implied. See the License for the specific language governing permissions and
    limitations under the License.
*/

package ai.picovoice.porcupine;

import com.google.gson.JsonObject;
import com.google.gson.JsonParser;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;


public class PorcupineTestUtils {
    private static final String ENVIRONMENT_NAME;

    static {
        ENVIRONMENT_NAME = Utils.getEnvironmentName();
    }

    public static String appendLanguage(String s, String language) {
        if (language.equals("en")) {
            return s;
        }
        return s + "_" + language;
    }

    public static String getTestKeywordPath(String language, String keyword) {
        return Paths.get(System.getProperty("user.dir"))
                .resolve("../../resources")
                .resolve(appendLanguage("keyword_files", language))
                .resolve(ENVIRONMENT_NAME)
                .resolve(keyword + "_" + ENVIRONMENT_NAME + ".ppn")
                .toString();
    }

    public static String getTestModelPath(String language) {
        return Paths.get(System.getProperty("user.dir"))
                .resolve("../../lib/common")
                .resolve(appendLanguage("porcupine_params", language) + ".pv")
                .toString();
    }

    public static String getTestAudioFilePath(String audioFileName) {
        return Paths.get(System.getProperty("user.dir"))
                .resolve("../../resources/audio_samples")
                .resolve(audioFileName)
                .toString();
    }

    public static String[] getTestKeywordPaths(String language, String[] keywords) {
        String[] keywordPaths = new String[keywords.length];
        for (int i = 0; i < keywords.length; i++) {
            keywordPaths[i] = getTestKeywordPath(language, keywords[i]);
        }
        return keywordPaths;
    }

    public static JsonObject loadTestData() throws IOException {
        final Path testDataPath = Paths.get(System.getProperty("user.dir"))
                .resolve("../../resources/.test")
                .resolve("test_data.json");
        final String testDataContent = new String(Files.readAllBytes(testDataPath), StandardCharsets.UTF_8);
        return JsonParser.parseString(testDataContent).getAsJsonObject();
    }
}
