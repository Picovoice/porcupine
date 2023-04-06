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

using System.Collections.Generic;
using System;

using UnityEngine;


namespace Pv.Unity
{

    public class PorcupineManager
    {
        private VoiceProcessor _voiceProcessor;
        private Porcupine _porcupine;
        private Action<int> _wakeWordCallback;
        private Action<PorcupineException> _processErrorCallback;

        /// <summary>
        /// Creates an instance of the Porcupine wake word engine from built-in keywords
        /// </summary>
        /// <param name="accessKey">AccessKey obtained from Picovoice Console (https://picovoice.ai/console/)</param>
        /// <param name="keywords">
        /// List of built-in keywords (phrases) to detect. The list of available (default) keywords can be retrieved
        /// using `PorcupineManager.BUILT_IN_KEYWORDS`.
        /// </param>
        /// <param name="wakeWordCallback">A callback that is triggered when one of the given keywords has been detected by Porcupine.</param>
        /// <param name="modelPath">(Optional) Absolute path to the file containing model parameters. If not set it will be set to the default location.</param>
        /// <param name="sensitivities">
        /// (Optional) Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher sensitivity results in fewer
        /// misses at the cost of increasing the false alarm rate. If not set 0.5 will be used.
        /// </param>
        /// <param name="processErrorCallback">(Optional) Callback that triggers is the engine experiences a problem while processing audio.</param>
        /// <returns>An instance of PorcupineManager.</returns>
        public static PorcupineManager FromBuiltInKeywords(string accessKey, IEnumerable<Porcupine.BuiltInKeyword> keywords, Action<int> wakeWordCallback,
                                                    string modelPath = null, IEnumerable<float> sensitivities = null, Action<PorcupineException> processErrorCallback = null)
        {
            Porcupine porcupine = Porcupine.FromBuiltInKeywords(accessKey: accessKey, keywords: keywords, modelPath: modelPath, sensitivities: sensitivities);
            return new PorcupineManager(porcupine, wakeWordCallback, processErrorCallback);
        }

        /// <summary>
        /// Creates an instance of the Porcupine wake word engine from custom keyword files.
        /// </summary>
        /// <param name="accessKey">AccessKey obtained from Picovoice Console (https://picovoice.ai/console/)</param>
        /// <param name="keywordPaths">List of absolute paths to keyword model files (.ppn).</param>
        /// <param name="wakeWordCallback">A callback that is triggered when one of the given keywords has been detected by Porcupine.</param>
        /// <param name="modelPath">(Optional) Absolute path to the file containing model parameters. If not set it will be set to the default location.</param>
        /// <param name="sensitivities">
        /// (Optional) Sensitivities for detecting keywords. Each value should be a number within [0, 1]. A higher sensitivity results in fewer
        /// misses at the cost of increasing the false alarm rate. If not set 0.5 will be used.
        /// </param>
        /// <param name="processErrorCallback">(Optional) Callback that triggers is the engine experiences a problem while processing audio.</param>
        /// <returns>An instance of PorcupineManager.</returns>
        public static PorcupineManager FromKeywordPaths(string accessKey, IEnumerable<string> keywordPaths, Action<int> wakeWordCallback,
                                                    string modelPath = null, IEnumerable<float> sensitivities = null, Action<PorcupineException> processErrorCallback = null)
        {
            Porcupine porcupine = Porcupine.FromKeywordPaths(accessKey: accessKey, keywordPaths: keywordPaths, modelPath: modelPath, sensitivities: sensitivities);
            return new PorcupineManager(porcupine, wakeWordCallback, processErrorCallback);
        }

        // private constructor
        private PorcupineManager(Porcupine porcupine, Action<int> wakeWordCallback, Action<PorcupineException> processErrorCallback = null)
        {
            _porcupine = porcupine;
            _wakeWordCallback = wakeWordCallback;
            _processErrorCallback = processErrorCallback;

            _voiceProcessor = VoiceProcessor.Instance;
            _voiceProcessor.OnFrameCaptured += OnFrameCaptured;
        }

        /// <summary>
        /// Action to catch audio frames as voice processor produces them
        /// </summary>
        /// <param name="pcm">Frame of pcm audio</param>
        private void OnFrameCaptured(short[] pcm)
        {
            try
            {
                int keywordIndex = _porcupine.Process(pcm);
                if (keywordIndex >= 0)
                {
                    if (_wakeWordCallback != null)
                        _wakeWordCallback.Invoke(keywordIndex);
                }
            }
            catch (Exception ex)
            {
                if (_processErrorCallback != null)
                    _processErrorCallback(new PorcupineException(ex.Message));
                else
                    Debug.LogError(ex.ToString());
            }
        }

        /// <summary>
        /// Checks to see whether PorcupineManager is capturing audio or not
        /// </summary>
        /// <returns>whether PorcupineManager  is capturing audio or not</returns>
        public bool IsRecording => _voiceProcessor.IsRecording;

        /// <summary>
        /// Checks to see whether there are any audio capture devices available
        /// </summary>
        /// <returns>whether there are any audio capture devices available</returns>
        public bool IsAudioDeviceAvailable()
        {
            _voiceProcessor.UpdateDevices();
            return _voiceProcessor.CurrentDeviceIndex >= 0;
        }

        /// <summary>
        /// Starts audio capture and wake word detection
        /// </summary>
        public void Start()
        {
            if (_porcupine == null || _voiceProcessor == null)
            {
                throw new ObjectDisposedException("Porcupine", "Cannot start PorcupineManager - resources have already been released");
            }
            _voiceProcessor.StartRecording(_porcupine.SampleRate, _porcupine.FrameLength);
        }

        /// <summary>
        /// Stops audio capture and wake word detection
        /// </summary>
        public void Stop()
        {
            if (_porcupine == null || _voiceProcessor == null)
            {
                throw new ObjectDisposedException("Porcupine", "Stop called after PorcupineManager resources were released.");
            }
            _voiceProcessor.StopRecording();
        }

        /// <summary>
        /// Free resources that were allocated to Porcupine and the voice processor
        /// </summary>
        public void Delete()
        {
            if (_voiceProcessor != null)
            {
                if (_voiceProcessor.IsRecording)
                {
                    _voiceProcessor.StopRecording();
                }

                _voiceProcessor.OnFrameCaptured -= OnFrameCaptured;
                _voiceProcessor = null;
            }

            if (_porcupine != null)
            {
                _porcupine.Dispose();
                _porcupine = null;
            }
        }
    }
}