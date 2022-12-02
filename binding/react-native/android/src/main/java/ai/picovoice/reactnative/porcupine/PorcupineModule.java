/*
    Copyright 2020-2021 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is
    located in the "LICENSE" file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the
    License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
    express or implied. See the License for the specific language governing permissions and
    limitations under the License.
*/

package ai.picovoice.reactnative.porcupine;

import ai.picovoice.porcupine.*;

import com.facebook.react.bridge.Arguments;
import com.facebook.react.bridge.Promise;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.bridge.ReadableArray;
import com.facebook.react.bridge.WritableMap;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;


public class PorcupineModule extends ReactContextBaseJavaModule {

    private static final String LOG_TAG = "PvPorcupine";
    private final ReactApplicationContext reactContext;
    private final Map<String, Porcupine> porcupinePool = new HashMap<>();

    public PorcupineModule(ReactApplicationContext reactContext) {
        super(reactContext);
        this.reactContext = reactContext;
    }

    @Override
    public String getName() {
        return "PvPorcupine";
    }

    @ReactMethod
    public void fromBuiltInKeywords(
            String accessKey,
            String modelPath,
            ReadableArray keywords,
            ReadableArray sensitivities,
            Promise promise
    ) {
        // convert from ReadableArrays to Java types
        Porcupine.BuiltInKeyword[] keywordsJava = new Porcupine.BuiltInKeyword[keywords.size()];
        for (int i = 0; i < keywords.size(); i++) {
            try {
                String keyword = keywords.getString(i);
                if (keyword == null) {
                    promise.reject(
                            PorcupineInvalidArgumentException.class.getSimpleName(),
                            "keyword must be a valid string");
                    return;
                }

                keyword = keyword.replace(' ', '_');
                keywordsJava[i] = Porcupine.BuiltInKeyword.valueOf(keyword.toUpperCase());
            } catch (IllegalArgumentException e) {
                promise.reject(
                        PorcupineInvalidArgumentException.class.getSimpleName(),
                        String.format("'%s' is not a built-in keyword", keywords.getString(i)));
                return;
            }
        }

        float[] sensitivitiesJava = new float[sensitivities.size()];
        for (int i = 0; i < sensitivities.size(); i++) {
            sensitivitiesJava[i] = (float) sensitivities.getDouble(i);
        }

        try {
            Porcupine porcupine = new Porcupine.Builder()
                    .setAccessKey(accessKey)
                    .setModelPath(modelPath.isEmpty() ? null : modelPath)
                    .setKeywords(keywordsJava.length == 0 ? null : keywordsJava)
                    .setSensitivities(sensitivitiesJava.length == 0 ? null : sensitivitiesJava)
                    .build(reactContext);
            porcupinePool.put(String.valueOf(System.identityHashCode(porcupine)), porcupine);

            WritableMap paramMap = Arguments.createMap();
            paramMap.putString("handle", String.valueOf(System.identityHashCode(porcupine)));
            paramMap.putInt("frameLength", porcupine.getFrameLength());
            paramMap.putInt("sampleRate", porcupine.getSampleRate());
            paramMap.putString("version", porcupine.getVersion());
            promise.resolve(paramMap);
        } catch (PorcupineException e) {
            promise.reject(e.getClass().getSimpleName(), e.getMessage());
        }
    }

    @ReactMethod
    public void fromKeywordPaths(
            String accessKey,
            String modelPath,
            ReadableArray keywordPaths,
            ReadableArray sensitivities,
            Promise promise) {
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
            Porcupine porcupine = new Porcupine.Builder()
                    .setAccessKey(accessKey)
                    .setModelPath(modelPath.isEmpty() ? null : modelPath)
                    .setKeywordPaths(keywordPathsJava.length == 0 ? null : keywordPathsJava)
                    .setSensitivities(sensitivitiesJava.length == 0 ? null : sensitivitiesJava)
                    .build(reactContext);
            porcupinePool.put(String.valueOf(System.identityHashCode(porcupine)), porcupine);

            WritableMap paramMap = Arguments.createMap();
            paramMap.putString("handle", String.valueOf(System.identityHashCode(porcupine)));
            paramMap.putInt("frameLength", porcupine.getFrameLength());
            paramMap.putInt("sampleRate", porcupine.getSampleRate());
            paramMap.putString("version", porcupine.getVersion());
            promise.resolve(paramMap);
        } catch (PorcupineException e) {
            promise.reject(e.getClass().getSimpleName(), e.getMessage());
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
                promise.reject(PorcupineInvalidStateException.class.getSimpleName(),
                        "Invalid Porcupine handle provided to native module.");
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
            promise.reject(e.getClass().getSimpleName(), e.getMessage());
        }
    }
}
