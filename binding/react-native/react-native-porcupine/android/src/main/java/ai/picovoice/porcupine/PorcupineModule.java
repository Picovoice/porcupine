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

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.bridge.Callback;

public class PorcupineModule extends ReactContextBaseJavaModule {

    private final ReactApplicationContext reactContext;
    private Porcupine porcupine;

    public PorcupineModule(ReactApplicationContext reactContext) {
        super(reactContext);
        this.reactContext = reactContext;
    }

    @Override
    public String getName() {
        return "Porcupine";
    }

    @ReactMethod
    public void init(String modelPath, String keywordPath, float sensitivity) {
      try{
        porcupine = new Porcupine(modelPath, keywordPath, sensitivity);        
        System.out.println("S*******************************************************************************************D");        
      }
      catch(PorcupineException e){
        System.out.println("S*******************************************************************************************a");         
      }
    }
}
    

    // @ReactMethod
    // public void init(String modelPath, String[] keywordPaths, float[] sensitivities) {
    //   try{
    //     porcupine = new Porcupine(modelPath, keywordPaths, sensitivities);        
    //     System.out.println("S*******************************************************************************************D");      
    //   }
    //   catch(PorcupineException e){
    //     System.out.println("S*******************************************************************************************a");         
    //   }
    // }

    // @ReactMethod
    // public void delete() {
    //   porcupine.delete();
    // }

    // @ReactMethod
    // public void process(short[] pcm) {
    //   try{
    //     porcupine.process(pcm);
    //   }
    //   catch(PorcupineException e){
    //     System.err.println(e.toString());        
    //   }
    // }

    // @ReactMethod
    // public void getFrameLength() {
    //   porcupine.getFrameLength();
    // }

    // @ReactMethod
    // public void getSampleRate() {
    //    porcupine.getSampleRate();
    // }

    // @ReactMethod
    // public void getVersion() {
    //   porcupine.getVersion();
    // }
// }
