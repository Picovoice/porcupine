//
// Copyright 2020-2021 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
// file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//

package ai.picovoice.flutter.porcupine;

import android.content.Context;

import androidx.annotation.NonNull;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import io.flutter.embedding.engine.plugins.FlutterPlugin;
import io.flutter.plugin.common.MethodCall;
import io.flutter.plugin.common.MethodChannel;
import io.flutter.plugin.common.MethodChannel.MethodCallHandler;
import io.flutter.plugin.common.MethodChannel.Result;

import ai.picovoice.porcupine.*;

public class PorcupinePlugin implements FlutterPlugin, MethodCallHandler {

    private enum Method {
        FROM_BUILTIN_KEYWORDS,
        FROM_KEYWORD_PATHS,
        PROCESS,
        DELETE
    }

    private Context flutterContext;
    private MethodChannel channel;
    private final Map<String, Porcupine> porcupinePool = new HashMap<>();

    @Override
    public void onAttachedToEngine(@NonNull FlutterPluginBinding flutterPluginBinding) {
        flutterContext = flutterPluginBinding.getApplicationContext();
        channel = new MethodChannel(flutterPluginBinding.getBinaryMessenger(), "porcupine");
        channel.setMethodCallHandler(this);
    }

    @Override
    public void onMethodCall(@NonNull MethodCall call, @NonNull Result result) {
        Method method;
        try {
            method = Method.valueOf(call.method.toUpperCase());
        } catch (IllegalArgumentException e) {
            result.error(
                    PorcupineRuntimeException.class.getSimpleName(),
                    String.format("Porcupine method '%s' is not a valid function", call.method),
                    null);
            return;
        }

        switch (method) {
            case FROM_BUILTIN_KEYWORDS:
                try {
                    String accessKey = call.argument("accessKey");
                    String modelPath = call.argument("modelPath");
                    ArrayList<String> keywordsList = call.argument("keywords");
                    ArrayList<Double> sensitivitiesList = call.argument("sensitivities");

                    Porcupine.BuiltInKeyword[] keywords = null;
                    if (keywordsList != null) {
                        keywords = new Porcupine.BuiltInKeyword[keywordsList.size()];
                        for (int i = 0; i < keywordsList.size(); i++) {
                            try {
                                String keyword = keywordsList.get(i).replace(' ', '_').toUpperCase();
                                keywords[i] = Porcupine.BuiltInKeyword.valueOf(keyword);
                            } catch (IllegalArgumentException e) {
                                result.error(
                                        PorcupineInvalidArgumentException.class.getSimpleName(),
                                        String.format("'%s' is not a built-in keyword", keywordsList.get(i)),
                                        null);
                                return;
                            }
                        }
                    }

                    float [] sensitivities = null;
                    if (sensitivitiesList != null) {
                        sensitivities = new float[sensitivitiesList.size()];
                        for (int i = 0; i < sensitivitiesList.size(); i++) {
                            sensitivities[i] = sensitivitiesList.get(i).floatValue();
                        }
                    }

                    Porcupine porcupine = new Porcupine.Builder()
                            .setAccessKey(accessKey)
                            .setModelPath(modelPath)
                            .setKeywords(keywords)
                            .setSensitivities(sensitivities)
                            .build(flutterContext);

                    porcupinePool.put(String.valueOf(System.identityHashCode(porcupine)), porcupine);

                    Map<String, Object> param = new HashMap<>();
                    param.put("handle", String.valueOf(System.identityHashCode(porcupine)));
                    param.put("frameLength", porcupine.getFrameLength());
                    param.put("sampleRate", porcupine.getSampleRate());
                    param.put("version", porcupine.getVersion());

                    result.success(param);
                } catch (PorcupineException e) {
                    result.error(e.getClass().getSimpleName(), e.getMessage(), null);
                } catch (Exception e) {
                    result.error(PorcupineException.class.getSimpleName(), e.getMessage(), null);
                }
                break;
            case FROM_KEYWORD_PATHS:
                try {
                    String accessKey = call.argument("accessKey");
                    String modelPath = call.argument("modelPath");
                    ArrayList<String> keywordPathsList = call.argument("keywordPaths");
                    ArrayList<Double> sensitivitiesList = call.argument("sensitivities");

                    String[] keywordPaths = null;
                    if (keywordPathsList != null) {
                        keywordPaths = new String[keywordPathsList.size()];
                        for (int i = 0; i < keywordPathsList.size(); i++) {
                            keywordPaths[i] = keywordPathsList.get(i);
                        }
                    }

                    float [] sensitivities = null;
                    if (sensitivitiesList != null) {
                        sensitivities = new float[sensitivitiesList.size()];
                        for (int i = 0; i < sensitivitiesList.size(); i++) {
                            sensitivities[i] = sensitivitiesList.get(i).floatValue();
                        }
                    }

                    Porcupine porcupine = new Porcupine.Builder()
                            .setAccessKey(accessKey)
                            .setModelPath(modelPath)
                            .setKeywordPaths(keywordPaths)
                            .setSensitivities(sensitivities)
                            .build(flutterContext);

                    porcupinePool.put(String.valueOf(System.identityHashCode(porcupine)), porcupine);

                    Map<String, Object> param = new HashMap<>();
                    param.put("handle", String.valueOf(System.identityHashCode(porcupine)));
                    param.put("frameLength", porcupine.getFrameLength());
                    param.put("sampleRate", porcupine.getSampleRate());
                    param.put("version", porcupine.getVersion());

                    result.success(param);
                } catch (PorcupineException e) {
                    result.error(e.getClass().getSimpleName(), e.getMessage(), null);
                } catch (Exception e) {
                    result.error(PorcupineException.class.getSimpleName(), e.getMessage(), null);
                }
                break;
            case PROCESS:
                try {
                    String handle = call.argument("handle");
                    ArrayList<Integer> pcmList = call.argument("frame");

                    if (!porcupinePool.containsKey(handle)) {
                        result.error(
                                PorcupineInvalidArgumentException.class.getSimpleName(),
                                "Invalid Porcupine handle provided to native module",
                                null);
                        return;
                    }

                    short[] pcm = null;
                    if (pcmList != null) {
                        pcm = new short[pcmList.size()];
                        for (int i = 0; i < pcmList.size(); i++) {
                            pcm[i] = pcmList.get(i).shortValue();
                        }
                    }

                    Porcupine porcupine = porcupinePool.get(handle);
                    int keywordIndex = porcupine.process(pcm);
                    result.success(keywordIndex);
                } catch (PorcupineException e) {
                    result.error(
                            e.getClass().getSimpleName(),
                            e.getMessage(),
                            null);
                }
                break;
            case DELETE:
                String handle = call.argument("handle");

                if (!porcupinePool.containsKey(handle)) {
                    result.error(
                            PorcupineInvalidArgumentException.class.getSimpleName(),
                            "Invalid Porcupine handle provided to native module.",
                            null);
                    return;
                }

                Porcupine porcupine = porcupinePool.get(handle);
                porcupine.delete();
                porcupinePool.remove(handle);

                result.success(null);
                break;
            default:
        }
    }

    @Override
    public void onDetachedFromEngine(@NonNull FlutterPluginBinding binding) {
        channel.setMethodCallHandler(null);
    }
}
