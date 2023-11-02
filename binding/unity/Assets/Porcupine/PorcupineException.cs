//
// Copyright 2021-2023 Picovoice Inc.
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
        private string[] _messageStack;

        public PorcupineException() { }

        public PorcupineException(string message) : base(message) { }

        public PorcupineException(string message, string[] messageStack) : base(ModifyMessages(message, messageStack))
        {
            this._messageStack = messageStack;
        }

        public string[] MessageStack
        {
            get => _messageStack;
        }

        private static string ModifyMessages(string message, string[] messageStack)
        {
            string messageString = message;
            if (messageStack.Length > 0)
            {
                messageString += ":";
                for (int i = 0; i < messageStack.Length; i++)
                {
                    messageString += String.Format("\n  [{0}] {1}", i, messageStack[i]);
                }
            }
            return messageString;
        }
    }

    public class PorcupineMemoryException : PorcupineException
    {
        public PorcupineMemoryException() { }

        public PorcupineMemoryException(string message) : base(message) { }

        public PorcupineMemoryException(string message, string[] messageStack) : base(message, messageStack) { }
    }

    public class PorcupineIOException : PorcupineException
    {
        public PorcupineIOException() { }

        public PorcupineIOException(string message) : base(message) { }

        public PorcupineIOException(string message, string[] messageStack) : base(message, messageStack) { }
    }

    public class PorcupineInvalidArgumentException : PorcupineException
    {
        public PorcupineInvalidArgumentException() { }

        public PorcupineInvalidArgumentException(string message) : base(message) { }

        public PorcupineInvalidArgumentException(string message, string[] messageStack) : base(message, messageStack) { }
    }

    public class PorcupineStopIterationException : PorcupineException
    {
        public PorcupineStopIterationException() { }

        public PorcupineStopIterationException(string message) : base(message) { }

        public PorcupineStopIterationException(string message, string[] messageStack) : base(message, messageStack) { }
    }

    public class PorcupineKeyException : PorcupineException
    {
        public PorcupineKeyException() { }

        public PorcupineKeyException(string message) : base(message) { }

        public PorcupineKeyException(string message, string[] messageStack) : base(message, messageStack) { }
    }

    public class PorcupineInvalidStateException : PorcupineException
    {
        public PorcupineInvalidStateException() { }

        public PorcupineInvalidStateException(string message) : base(message) { }

        public PorcupineInvalidStateException(string message, string[] messageStack) : base(message, messageStack) { }
    }

    public class PorcupineRuntimeException : PorcupineException
    {
        public PorcupineRuntimeException() { }

        public PorcupineRuntimeException(string message) : base(message) { }

        public PorcupineRuntimeException(string message, string[] messageStack) : base(message, messageStack) { }
    }

    public class PorcupineActivationException : PorcupineException
    {
        public PorcupineActivationException() { }

        public PorcupineActivationException(string message) : base(message) { }

        public PorcupineActivationException(string message, string[] messageStack) : base(message, messageStack) { }
    }

    public class PorcupineActivationLimitException : PorcupineException
    {
        public PorcupineActivationLimitException() { }

        public PorcupineActivationLimitException(string message) : base(message) { }

        public PorcupineActivationLimitException(string message, string[] messageStack) : base(message, messageStack) { }
    }

    public class PorcupineActivationThrottledException : PorcupineException
    {
        public PorcupineActivationThrottledException() { }

        public PorcupineActivationThrottledException(string message) : base(message) { }

        public PorcupineActivationThrottledException(string message, string[] messageStack) : base(message, messageStack) { }
    }

    public class PorcupineActivationRefusedException : PorcupineException
    {
        public PorcupineActivationRefusedException() { }

        public PorcupineActivationRefusedException(string message) : base(message) { }

        public PorcupineActivationRefusedException(string message, string[] messageStack) : base(message, messageStack) { }
    }
}
