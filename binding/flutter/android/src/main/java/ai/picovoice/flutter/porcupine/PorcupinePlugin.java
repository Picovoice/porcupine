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

import java.io.File;
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
    INIT,
    CREATE,
    PROCESS,
    DELETE,
    DEFAULT
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
      method = Method.DEFAULT;
    }

    switch (method) {
      case INIT:
        try {
          // default model file
          final File resourceDirectory = flutterContext.getFilesDir();
          final Map<String, Object> constants = new HashMap<>();
          constants.put("DEFAULT_MODEL_PATH", new File(resourceDirectory, "porcupine_params.pv").getAbsolutePath());

          // default keyword files
          final Map<String, String> keywordPaths = new HashMap<>();
          for (Porcupine.BuiltInKeyword x : Porcupine.BuiltInKeyword.values()) {
            String fileName = x.name().toLowerCase();
            String keyword = fileName.replace('_', ' ');
            keywordPaths.put(keyword, new File(resourceDirectory, fileName + ".ppn").getAbsolutePath());
          }
          constants.put("KEYWORD_PATHS", keywordPaths);

          result.success(constants);
        } catch (Exception e) {
          result.error(
                  PorcupineException.class.getSimpleName(),
                  String.format("Failed to get constants: %s", e.getMessage()),
                  null);
        }
        break;
      case CREATE:
        try {
          String accessKey = call.argument("accessKey");
          String modelPath = call.argument("modelPath");
          ArrayList<String> keywordPathsList = call.argument("keywordPaths");
          ArrayList<Double> sensitivitiesList = call.argument("sensitivities");

          if (keywordPathsList == null || sensitivitiesList == null) {
            result.error(
                    PorcupineInvalidArgumentException.class.getSimpleName(),
                    "Arguments 'keywordPaths' and 'sensitivities' must be set",
                    null);
            return;
          }

          String[] keywordPaths = new String[keywordPathsList.size()];
          float [] sensitivities = new float[sensitivitiesList.size()];

          for (int i = 0; i < keywordPaths.length; i++) {
            keywordPaths[i] = keywordPathsList.get(i);
            sensitivities[i] = sensitivitiesList.get(i).floatValue();
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
                    "Invalid Porcupine handle provided to native module.",
                    null);
            return;
          }

          if (pcmList == null) {
            result.error(
                    PorcupineInvalidArgumentException.class.getSimpleName(),
                    "Argument 'frame' must be provided.",
                    null);
            return;
          }

          Porcupine porcupine = porcupinePool.get(handle);
          short[] pcm = new short[pcmList.size()];

          for (int i = 0; i < pcmList.size(); i++) {
            pcm[i] = pcmList.get(i).shortValue();
          }

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
        result.success(null);
        break;
      default:
        result.error(
                PorcupineRuntimeException.class.getSimpleName(),
                String.format("Porcupine method '%s' is not a valid function.", call.method),
                null);
    }
  }

  @Override
  public void onDetachedFromEngine(@NonNull FlutterPluginBinding binding) {
    channel.setMethodCallHandler(null);
  }
}
