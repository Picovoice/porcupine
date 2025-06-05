/*
    Copyright 2025 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is
    located in the "LICENSE" file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the
    License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
    express or implied. See the License for the specific language governing permissions and
    limitations under the License.
*/

package ai.picovoice.porcupine.testapp;

import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;

import com.google.gson.JsonArray;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Objects;

import ai.picovoice.porcupine.Porcupine;


@RunWith(Parameterized.class)
public class LanguageTests extends BaseTest {

    @Parameterized.Parameter(value = 0)
    public String modelFile;

    @Parameterized.Parameter(value = 1)
    public String[] keywordFiles;

    @Parameterized.Parameter(value = 2)
    public String testAudioFile;

    @Parameterized.Parameter(value = 3)
    public int[] expectedResults;

    @Parameterized.Parameters(name = "{0}")
    public static Collection<Object[]> initParameters() throws IOException {
        String testDataJsonString = getTestDataString();

        JsonObject testDataJson = JsonParser.parseString(testDataJsonString).getAsJsonObject();
        JsonArray multipleKeywordDataJson = testDataJson.getAsJsonObject("tests").getAsJsonArray("multipleKeyword");

        List<Object[]> parameters = new ArrayList<>();
        for (int i = 0; i < multipleKeywordDataJson.size(); i++) {
            JsonObject testData = multipleKeywordDataJson.get(i).getAsJsonObject();
            String language = testData.get("language").getAsString();
            JsonArray keywords = testData.getAsJsonArray("wakewords");
            JsonArray groundTruthJson = testData.getAsJsonArray("groundTruth");

            String modelFile = String.format("porcupine_params_%s.pv", language);
            String[] keywordFiles = new String[keywords.size()];
            for (int j = 0; j < keywords.size(); j++) {
                String keyword = keywords.get(j).getAsString();
                keywordFiles[j] = String.format("%s/%s_android.ppn", language, keyword);
            }
            String audioFile = String.format("multiple_keywords_%s.wav", language);
            int[] groundTruth = new int[groundTruthJson.size()];
            for (int j = 0; j < groundTruthJson.size(); j++) {
                groundTruth[j] = groundTruthJson.get(j).getAsInt();
            }

            if (Objects.equals(language, "en")) {
                modelFile = "porcupine_params.pv";
                audioFile = "multiple_keywords.wav";
            }

            parameters.add(new Object[] {
                    modelFile,
                    keywordFiles,
                    audioFile,
                    groundTruth
            });
        }

        return parameters;
    }


    @Test
    public void testProcess() throws Exception {
        String modelPath = getModelFilepath(modelFile);
        String[] keywordPaths = new String[keywordFiles.length];
        for (int i = 0; i < keywordFiles.length; i++) {
            keywordPaths[i] = getKeywordFilepath(keywordFiles[i]);

        }
        Porcupine p = new Porcupine.Builder()
                .setAccessKey(accessKey)
                .setModelPath(modelPath)
                .setKeywordPaths(keywordPaths)
                .build(appContext);

        assertTrue(p.getVersion() != null && !p.getVersion().isEmpty());
        assertTrue(p.getFrameLength() > 0);
        assertTrue(p.getSampleRate() > 0);

        File testAudio = new File(getAudioFilepath(testAudioFile));
        ArrayList<Integer> detectionResults = processTestAudio(p, testAudio);
        p.delete();

        assertSame(expectedResults.length, detectionResults.size());
        for (int i = 0; i < expectedResults.length; i++) {
            assertSame(detectionResults.get(i), expectedResults[i]);
        }

        p.delete();
    }
}
