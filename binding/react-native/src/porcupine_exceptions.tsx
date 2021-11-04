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

class PorcupineException extends Error {
    constructor(message: string) {
        super(message);
        this.name = "PorcupineException";
    }
}

class PorcupineMemoryException extends PorcupineException {
    constructor(message: string) {
        super(message);
        this.name = "PorcupineMemoryException";
    }
}

class PorcupineIOException extends PorcupineException {
    constructor(message: string) {
        super(message);
        this.name = "PorcupineIOException";
    }
}

class PorcupineInvalidArgumentException extends PorcupineException {
    constructor(message: string) {
        super(message);
        this.name = "PorcupineInvalidArgumentException";
    }
}

class PorcupineStopIterationException extends PorcupineException {
    constructor(message: string) {
        super(message);
        this.name = "PorcupineStopIterationException";
    }
}

class PorcupineKeyException extends PorcupineException {
    constructor(message: string) {
        super(message);
        this.name = "PorcupineKeyException";
    }
}

class PorcupineInvalidStateException extends PorcupineException {
    constructor(message: string) {
        super(message);
        this.name = "PorcupineInvalidStateException";
    }
}

class PorcupineRuntimeException extends PorcupineException {
    constructor(message: string) {
        super(message);
        this.name = "PorcupineRuntimeException";
    }
}

class PorcupineActivationException extends PorcupineException {
    constructor(message: string) {
        super(message);
        this.name = "PorcupineActivationException";
    }
}

class PorcupineActivationLimitException extends PorcupineException {
    constructor(message: string) {
        super(message);
        this.name = "PorcupineActivationLimitException";
    }
}

class PorcupineActivationThrottledException extends PorcupineException {
    constructor(message: string) {
        super(message);
        this.name = "PorcupineActivationThrottledException";
    }
}

class PorcupineActivationRefusedException extends PorcupineException {
    constructor(message: string) {
        super(message);
        this.name = "PorcupineActivationRefusedException";
    }
}

export {
    PorcupineException,
    PorcupineMemoryException,
    PorcupineIOException,
    PorcupineInvalidArgumentException,
    PorcupineStopIterationException,
    PorcupineKeyException,
    PorcupineInvalidStateException,
    PorcupineRuntimeException,
    PorcupineActivationException,
    PorcupineActivationLimitException,
    PorcupineActivationThrottledException,
    PorcupineActivationRefusedException
};
