/*
    Copyright 2021-2023 Picovoice Inc.
    You may not use this file except in compliance with the license. A copy of the license is
    located in the "LICENSE" file accompanying this source.
    Unless required by applicable law or agreed to in writing, software distributed under the
    License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
    express or implied. See the License for the specific language governing permissions and
    limitations under the License.
*/

package ai.picovoice.porcupine;

import android.annotation.SuppressLint;

public class PorcupineException extends Exception {
    private final String message;
    private final String[] messageStack;

    public PorcupineException(Throwable cause) {
        super(cause);
        this.message = cause.getMessage();
        this.messageStack = null;
    }

    public PorcupineException(String message) {
        super(message);
        this.message = message;
        this.messageStack = null;
    }

    public PorcupineException(String message, String[] messageStack) {
        super(message);
        this.message = message;
        this.messageStack = messageStack;
    }

    public String[] getMessageStack() {
        return this.messageStack;
    }

    @SuppressLint("DefaultLocale")
    @Override
    public String getMessage() {
        StringBuilder sb = new StringBuilder(message);
        if (messageStack != null) {
            if (messageStack.length > 0) {
                sb.append(":");
                for (int i = 0; i < messageStack.length; i++) {
                    sb.append(String.format("\n  [%d] %s", i, messageStack[i]));
                }
            }
        }
        return sb.toString();
    }
}