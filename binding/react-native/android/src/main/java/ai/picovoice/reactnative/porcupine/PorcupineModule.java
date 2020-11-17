/*
    Copyright 2020 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is
    located in the "LICENSE" file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the
    License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
    express or implied. See the License for the specific language governing permissions and
    limitations under the License.
*/

package ai.picovoice.reactnative.porcupine;

import ai.picovoice.porcupine.Porcupine;
import ai.picovoice.porcupine.PorcupineException;

import android.content.res.Resources;
import android.util.Log;

import com.facebook.react.bridge.Arguments;
import com.facebook.react.bridge.Promise;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.bridge.ReadableArray;
import com.facebook.react.bridge.WritableMap;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;


public class PorcupineModule extends ReactContextBaseJavaModule {

  private static final String LOG_TAG = "PvPorcupine";
  private static final int[] KEYWORDS = {
    R.raw.americano_android, R.raw.blueberry_android, R.raw.bumblebee_android, R.raw.grapefruit_android, 
    R.raw.grasshopper_android, R.raw.picovoice_android, R.raw.porcupine_android, R.raw.terminator_android
  };
  private final ReactApplicationContext reactContext;
  private final Map<String, Porcupine> porcupinePool = new HashMap<String, Porcupine>();

  public PorcupineModule(ReactApplicationContext reactContext) {
    super(reactContext);
    this.reactContext = reactContext;

    try {
      copyResourceFiles();
    } catch (IOException e) {
      Log.e(LOG_TAG, e.toString());
    }
  }

  @Override
  public String getName() {
    return "PvPorcupine";
  }

  @Override
  public Map<String, Object> getConstants() {

    // default model file
    final File resourceDirectory = reactContext.getFilesDir();
    final Map<String, Object> constants = new HashMap<>();
    constants.put("DEFAULT_MODEL_PATH", new File(resourceDirectory, "porcupine_params.pv").getAbsolutePath());

    // default keyword files
    final Map<String, String> keywordPaths = new HashMap<>();
    final Resources resources = reactContext.getResources();
    for (final int x : KEYWORDS) {
      final String fileName = resources.getResourceEntryName(x);
      final String keyword = fileName.split("_")[0];
      keywordPaths.put(keyword, new File(resourceDirectory, fileName + ".ppn").getAbsolutePath());
    }
    constants.put("KEYWORD_PATHS", keywordPaths);

    return constants;
  }

  @ReactMethod
  public void create(String modelPath, ReadableArray keywordPaths, ReadableArray sensitivities, Promise promise) {

    // convert from ReadableArrays to Java types
    String[] keywordPathsJava = new String[keywordPaths.size()];
    for (int i = 0; i < keywordPaths.size(); i++) {
      keywordPathsJava[i] = keywordPaths.getString(i);
    }

    float[] sensitivitiesJava = new float[sensitivities.size()];
    for (int i = 0; i < sensitivities.size(); i++) {
      sensitivitiesJava[i] = (float) sensitivities.getDouble(i);
    }

    try {
      Porcupine porcupine = new Porcupine(modelPath, keywordPathsJava, sensitivitiesJava);
      porcupinePool.put(String.valueOf(System.identityHashCode(porcupine)), porcupine);
     
      WritableMap paramMap = Arguments.createMap();
      paramMap.putString("handle", String.valueOf(System.identityHashCode(porcupine)));
      paramMap.putInt("frameLength", porcupine.getFrameLength());
      paramMap.putInt("sampleRate", porcupine.getSampleRate());
      paramMap.putString("version", porcupine.getVersion());
      promise.resolve(paramMap);
    } catch (PorcupineException e) {
      promise.reject(e.toString());
    }
  }

  @ReactMethod
  public void delete(String handle) {
    if (porcupinePool.containsKey(handle)) {
      porcupinePool.get(handle).delete();
      porcupinePool.remove(handle);
    }
  }

  @ReactMethod
  public void process(String handle, ReadableArray pcmArray, Promise promise) {
    try {

      if (!porcupinePool.containsKey(handle)) {
        promise.reject("Invalid Porcupine handle provided to native module.");
        return;
      }

      Porcupine porcupine = porcupinePool.get(handle);
      ArrayList<Object> pcmArrayList = pcmArray.toArrayList();
      short[] buffer = new short[pcmArray.size()];
      for (int i = 0; i < pcmArray.size(); i++) {
        buffer[i] = ((Number) pcmArrayList.get(i)).shortValue();
      }
      int result = porcupine.process(buffer);
      promise.resolve(result);
    } catch (PorcupineException e) {
      promise.reject(e.toString());
    }
  }

  private void copyResourceFiles() throws IOException {
    final Resources resources = reactContext.getResources();

    for (final int x : KEYWORDS) {
      copyResourceFile(x, resources.getResourceEntryName(x) + ".ppn");
    }

    copyResourceFile(
      R.raw.porcupine_params,
      resources.getResourceEntryName(R.raw.porcupine_params) + ".pv");
  }

  private void copyResourceFile(int resourceId, String filename) throws IOException {
    final Resources resources = reactContext.getResources();
    try (
      InputStream is = new BufferedInputStream(resources.openRawResource(resourceId), 256);
      OutputStream os = new BufferedOutputStream(reactContext.openFileOutput(filename, ReactApplicationContext.MODE_PRIVATE), 256)
    ) {
      int r;
      while ((r = is.read()) != -1) {
        os.write(r);
      }
      os.flush();
    }
  }
}
