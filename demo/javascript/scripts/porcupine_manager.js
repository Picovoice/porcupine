/*
    Copyright 2018-2020 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

PorcupineManager = function (
  initCallback,
  porcupineWorkerScript,
  downsamplingScript
) {
  let porcupineWorker;

  let start = function (
    keywordIDs,
    sensitivities,
    detectionCallback,
    errorCallback
  ) {
    porcupineWorker = new Worker(porcupineWorkerScript);

    // ppn-init message signals that ppn wasm has fully loaded and ready for processing
    porcupineWorker.onmessage = function (messageEvent) {
      if (messageEvent.data.type === "ppn-init") {
        porcupineWorker.postMessage({
          command: "init",
          keywordIDs: keywordIDs,
          sensitivities: sensitivities,
        });

        WebVoiceProcessor.start([this], downsamplingScript, errorCallback);
        initCallback();
      } else {
        detectionCallback(messageEvent.data.keyword);
      }
    }.bind(this);
  };

  let stop = function () {
    WebVoiceProcessor.stop();
    porcupineWorker.postMessage({ command: "release" });
    porcupineWorker.terminate();
  };

  let processFrame = function (frame) {
    porcupineWorker.postMessage({ command: "process", inputFrame: frame });
  };

  return { start: start, processFrame: processFrame, stop: stop };
};
