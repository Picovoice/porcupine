//
// Copyright 2021 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
// file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//

using System;

namespace Pv.Unity
{
    public class PorcupineException : Exception
    {
        public string[] messages;

        public PorcupineException() { }

        public PorcupineException(string message) : base(message) { }

        public PorcupineException(string[] messages) : base(ModifyMessages(messages)) {
            this.messages = messages;
        }

        private static string ModifyMessages(string[] messages)
        {
            string message_string = "";
            for(int i = 0; i < messages.Length; i++) {
                message_string += String.Format("[{0}] {1}\n" i, messages[i]);
            }
            return message_string;
        }
    }

    public class PorcupineMemoryException : PorcupineException
    {
        public PorcupineMemoryException() { }

        public PorcupineMemoryException(string message) : base(message) { }

        public PorcupineException(string[] messages) : base(messages) { }
    }

    public class PorcupineIOException : PorcupineException
    {
        public PorcupineIOException() { }

        public PorcupineIOException(string message) : base(message) { }

        public PorcupineException(string[] messages) : base(messages) { }
    }

    public class PorcupineInvalidArgumentException : PorcupineException
    {
        public PorcupineInvalidArgumentException() { }

        public PorcupineInvalidArgumentException(string message) : base(message) { }

        public PorcupineException(string[] messages) : base(messages) { }
    }

    public class PorcupineStopIterationException : PorcupineException
    {
        public PorcupineStopIterationException() { }

        public PorcupineStopIterationException(string message) : base(message) { }

        public PorcupineException(string[] messages) : base(messages) { }
    }

    public class PorcupineKeyException : PorcupineException
    {
        public PorcupineKeyException() { }

        public PorcupineKeyException(string message) : base(message) { }

        public PorcupineException(string[] messages) : base(messages) { }
    }

    public class PorcupineInvalidStateException : PorcupineException
    {
        public PorcupineInvalidStateException() { }

        public PorcupineInvalidStateException(string message) : base(message) { }

        public PorcupineException(string[] messages) : base(messages) { }
    }

    public class PorcupineRuntimeException : PorcupineException
    {
        public PorcupineRuntimeException() { }

        public PorcupineRuntimeException(string message) : base(message) { }

        public PorcupineException(string[] messages) : base(messages) { }
    }

    public class PorcupineActivationException : PorcupineException
    {
        public PorcupineActivationException() { }

        public PorcupineActivationException(string message) : base(message) { }

        public PorcupineException(string[] messages) : base(messages) { }
    }

    public class PorcupineActivationLimitException : PorcupineException
    {
        public PorcupineActivationLimitException() { }

        public PorcupineActivationLimitException(string message) : base(message) { }

        public PorcupineException(string[] messages) : base(messages) { }
    }

    public class PorcupineActivationThrottledException : PorcupineException
    {
        public PorcupineActivationThrottledException() { }

        public PorcupineActivationThrottledException(string message) : base(message) { }

        public PorcupineException(string[] messages) : base(messages) { }
    }

    public class PorcupineActivationRefusedException : PorcupineException
    {
        public PorcupineActivationRefusedException() { }

        public PorcupineActivationRefusedException(string message) : base(message) { }

        public PorcupineException(string[] messages) : base(messages) { }
    }
}
