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

        WebVoiceProcessor.start([porcupineWorker], downsamplingScript, errorCallback);
    };

    let stop = function () {
        WebVoiceProcessor.stop();
        porcupineWorker.postMessage({command: "release"});
    };

    return {start: start, stop: stop}
});
