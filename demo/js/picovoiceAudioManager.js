navigator.getUserMedia = navigator.getUserMedia || navigator.webkitGetUserMedia || navigator.mozGetUserMedia;

let PicovoiceAudioManager = (function() {
    const inputBufferLength = 2048;

    let inputSampleRate;
    let engine;
    let processCallback;
    let isProcessing = false;

    let inputAudioBuffer = [];

    let process = function(inputAudioFrame) {
        if (!isProcessing) {
            return;
        }

        for (let i = 0 ; i < inputAudioFrame.length ; i++) {
            inputAudioBuffer.push((inputAudioFrame[i]) * 32767);
        }

        while(inputAudioBuffer.length * engine.sampleRate / inputSampleRate > engine.frameLength) {
            let result = new Int16Array(engine.frameLength);
            let bin = 0;
            let num = 0;
            let indexIn = 0;
            let indexOut = 0;

            while(indexIn < engine.frameLength) {
                bin = 0;
                num = 0;
                while(indexOut < Math.min(inputAudioBuffer.length, (indexIn + 1) * inputSampleRate / engine.sampleRate)) {
                    bin += inputAudioBuffer[indexOut];
                    num += 1;
                    indexOut++;
                }
                result[indexIn] = bin / num;
                indexIn++;
            }

            processCallback(engine.process(result));

            inputAudioBuffer = inputAudioBuffer.slice(indexOut);
        }
    };

    let getUserMediaSuccessCallback = function(stream) {
        let audioContext = new (window.AudioContext || window.webkitAudioContext)();

        let audioSource = audioContext.createMediaStreamSource(stream);

        inputSampleRate = audioSource.context.sampleRate;

        let engineNode = audioSource.context.createScriptProcessor(inputBufferLength, 1, 1);
        engineNode.onaudioprocess = function(ev) { process(ev.inputBuffer.getChannelData(0)); };
        audioSource.connect(engineNode);
        engineNode.connect(audioSource.context.destination);
    };

    this.start = function(picovoiceEngine, picovoiceProcessCallback, errorCallback) {
        if (!navigator.getUserMedia) {
            errorCallback("this browser does not support audio capture");
        }

        navigator.getUserMedia({audio: true}, getUserMediaSuccessCallback, errorCallback);

        engine = picovoiceEngine;
        processCallback = picovoiceProcessCallback;
        isProcessing = true;
    };

    this.stop = function() {
        isProcessing = false;
        inputAudioBuffer = [];
    };
});
