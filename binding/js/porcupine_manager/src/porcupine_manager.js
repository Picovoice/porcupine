PorcupineManager = (function (porcupineWorkerScript, downsamplingScript) {
    let porcupineWorker;

    let start = function (keywordIDs, sensitivities, detectionCallback, errorCallback) {
        porcupineWorker = new Worker(porcupineWorkerScript);
        porcupineWorker.postMessage({
            command: "init",
            keywordIDs: keywordIDs,
            sensitivities: sensitivities
        });

        porcupineWorker.onmessage = function (e) {
            detectionCallback(e.data.keyword);
        };

        WebVoiceProcessor.start([this], downsamplingScript, errorCallback);
    };

    let stop = function () {
        WebVoiceProcessor.stop();
        porcupineWorker.postMessage({command: "release"});
    };

    let processFrame = function (frame) {
        porcupineWorker.postMessage({command: "process", inputFrame: frame});
    };

    return {start: start, processFrame: processFrame, stop: stop}
});
