//
// Copyright 2020 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
// file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//

import 'dart:async';

import 'package:flutter/services.dart';
import 'package:flutter_voice_processor/flutter_voice_processor.dart';
import 'package:porcupine/porcupine.dart';
import 'package:porcupine/porcupine_error.dart';

typedef WakeWordCallback(int keywordIndex);

class PorcupineManager {
  static const List<String> BUILT_IN_KEYWORDS = Porcupine.BUILT_IN_KEYWORDS;

  VoiceProcessor _voiceProcessor;
  final WakeWordCallback _wakeWordCallback;
  Porcupine _porcupine;
  RemoveListener _removeVoiceProcessorListener;

  /// Static creator for initializing PorcupineManager a selection of built-in keywords
  ///
  /// [keywords] is a List of (phrases) for detection. The list of available
  /// keywords can be retrieved using [Porcupine.BUILT_IN_KEYWORDS]
  ///
  /// [modelPath] is a path to the file containing model parameters. If not set
  /// it will be set to the default location.
  ///
  /// [sensitivities] sensitivities for each keywords model. A higher sensitivity
  /// reduces miss rate at the cost of potentially higher false alarm rate.
  /// Sensitivity should be a floating-point number within 0 and 1.
  ///
  /// returns an instance of PorcupineManager
  static Future<PorcupineManager> fromKeywords(
      List<String> keywords, WakeWordCallback wakeWordCallback,
      {String modelPath, List<double> sensitivities}) async {
    Porcupine porcupine = await Porcupine.fromKeywords(keywords,
        modelPath: modelPath, sensitivities: sensitivities);
    return new PorcupineManager._(porcupine, wakeWordCallback);
  }

  /// Static creator for initializing PorcupineManager from a list of paths to custom keyword files
  ///
  /// [keywordPaths] A List of absolute paths to keyword model files.
  ///
  /// [modelPath] is a path to the file containing model parameters. If not set
  /// it will be set to the default location.
  ///
  /// [sensitivities] sensitivities for each keywords model. A higher sensitivity
  /// reduces miss rate at the cost of potentially higher false alarm rate.
  /// Sensitivity should be a floating-point number within 0 and 1.
  ///
  /// Thows a `PvError` if not initialized correctly
  ///
  /// returns an instance of PorcupineManager
  static Future<PorcupineManager> fromKeywordPaths(
      List<String> keywordPaths, WakeWordCallback wakeWordCallback,
      {String modelPath, List<double> sensitivities}) async {
    Porcupine porcupine = await Porcupine.fromKeywordPaths(keywordPaths,
        modelPath: modelPath, sensitivities: sensitivities);
    return new PorcupineManager._(porcupine, wakeWordCallback);
  }

  PorcupineManager._(this._porcupine, this._wakeWordCallback)
      : _voiceProcessor = VoiceProcessor.getVoiceProcessor(
            _porcupine.frameLength, _porcupine.sampleRate) {
    _removeVoiceProcessorListener = _voiceProcessor.addListener((buffer) {
      if (buffer == null) return;
      List<int> bufferData = (buffer as List<dynamic>).cast<int>();
      try {
        int keywordIndex = _porcupine.process(bufferData);
        if (keywordIndex >= 0) {
          _wakeWordCallback(keywordIndex);
        }
      } on PvError catch (e) {
        print(e.toString());
      }
    });
  }

  /// Opens audio input stream and sends audio frames to Porcupine
  /// Throws a `PvAudioException` if there was a problem starting the audio engine
  Future<void> start() async {
    if (await _voiceProcessor.hasRecordAudioPermission()) {
      try {
        await _voiceProcessor.start();
      } on PlatformException {
        throw new PvAudioException(
            "Audio engine failed to start. Hardware may not be supported.");
      }
    } else {
      throw new PvAudioException(
          "User did not give permission to record audio.");
    }
  }

  /// Closes audio stream
  Future<void> stop() async => await _voiceProcessor.stop();

  /// Releases Porcupine and audio resouces
  void delete() {
    if (_voiceProcessor != null) {
      _voiceProcessor.stop();
      _removeVoiceProcessorListener();
      _voiceProcessor = null;
    }

    if (_porcupine != null) {
      _porcupine.delete();
      _porcupine = null;
    }
  }
}
