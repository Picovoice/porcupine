//
// Copyright 2020-2023 Picovoice Inc.
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
import 'package:porcupine_flutter/porcupine.dart';
import 'package:porcupine_flutter/porcupine_error.dart';

typedef WakeWordCallback = Function(int keywordIndex);
typedef ErrorCallback = Function(PorcupineException error);

class PorcupineManager {
  VoiceProcessor? _voiceProcessor;
  Porcupine? _porcupine;

  late VoiceProcessorFrameListener _frameListener;
  late VoiceProcessorErrorListener _errorListener;

  bool _isListening = false;

  /// Static creator for initializing PorcupineManager from a selection of built-in keywords
  ///
  /// [accessKey] AccessKey obtained from Picovoice Console (https://console.picovoice.ai/).
  ///
  /// [keywords] is a List of (phrases) for detection. The list of available
  /// keywords can be retrieved using [BuiltInKeyword]
  ///
  /// [wakeWordCallback] A callback that is triggered when one of the given keywords
  /// has been detected by Porcupine
  ///
  /// [modelPath] is a path to the file containing model parameters. If not set
  /// it will be set to the default location.
  ///
  /// [sensitivities] sensitivities for each keywords model. A higher sensitivity
  /// reduces miss rate at the cost of potentially higher false alarm rate.
  /// Sensitivity should be a floating-point number within 0 and 1.
  ///
  /// [errorCallback] is an optional callback that triggers if Porcupine
  /// experiences a problem while processing audio
  ///
  /// Throws a `PorcupineException` if not successfully initialized.
  ///
  /// returns an instance of PorcupineManager
  static Future<PorcupineManager> fromBuiltInKeywords(String accessKey,
      List<BuiltInKeyword> keywords, WakeWordCallback wakeWordCallback,
      {String? modelPath,
      List<double>? sensitivities,
      ErrorCallback? errorCallback}) async {
    Porcupine porcupine = await Porcupine.fromBuiltInKeywords(
        accessKey, keywords,
        modelPath: modelPath, sensitivities: sensitivities);
    return PorcupineManager._(porcupine, wakeWordCallback, errorCallback);
  }

  /// Static creator for initializing PorcupineManager from a list of paths to custom keyword files
  ///
  /// [accessKey] AccessKey obtained from Picovoice Console (https://console.picovoice.ai/).
  ///
  /// [keywordPaths] A List of absolute paths to keyword model files.
  ///
  /// [wakeWordCallback] A callback that is triggered when one of the given keywords
  /// has been detected by Porcupine
  ///
  /// [modelPath] is a path to the file containing model parameters. If not set
  /// it will be set to the default location.
  ///
  /// [sensitivities] sensitivities for each keywords model. A higher sensitivity
  /// reduces miss rate at the cost of potentially higher false alarm rate.
  /// Sensitivity should be a floating-point number within 0 and 1.
  ///
  /// [errorCallback] is an optional callback that triggers if Porcupine
  /// experiences a problem while processing audio.
  ///
  /// Throws a `PorcupineException` if not successfully initialized.
  ///
  /// returns an instance of PorcupineManager
  static Future<PorcupineManager> fromKeywordPaths(String accessKey,
      List<String> keywordPaths, WakeWordCallback wakeWordCallback,
      {String? modelPath,
      List<double>? sensitivities,
      ErrorCallback? errorCallback}) async {
    Porcupine porcupine = await Porcupine.fromKeywordPaths(
        accessKey, keywordPaths,
        modelPath: modelPath, sensitivities: sensitivities);
    return PorcupineManager._(porcupine, wakeWordCallback, errorCallback);
  }

  // private constructor
  PorcupineManager._(this._porcupine, WakeWordCallback wakeWordCallback,
      ErrorCallback? errorCallback)
      : _voiceProcessor = VoiceProcessor.instance {
    _frameListener = (List<int> frame) async {
      if (!_isListening) {
        return;
      }

      try {
        int? keywordIndex = await _porcupine?.process(frame);
        if (keywordIndex != null && keywordIndex >= 0) {
          wakeWordCallback(keywordIndex);
        }
      } on PorcupineException catch (error) {
        errorCallback == null
            ? print("PorcupineException: ${error.message}")
            : errorCallback(error);
      }
    };

    _errorListener = (VoiceProcessorException error) {
      errorCallback == null
          ? print("PorcupineException: ${error.message}")
          : errorCallback(PorcupineException(error.message));
    };
  }

  /// Starts audio recording and processing with the Porcupine engine
  /// Throws a `PorcupineException` if there was a problem starting audio recording
  Future<void> start() async {
    if (_isListening) {
      return;
    }
    if (_porcupine == null || _voiceProcessor == null) {
      throw PorcupineInvalidStateException(
          "Cannot start Porcupine - resources have already been released");
    }

    if (await _voiceProcessor?.hasRecordAudioPermission() ?? false) {
      _voiceProcessor?.addFrameListener(_frameListener);
      _voiceProcessor?.addErrorListener(_errorListener);
      try {
        await _voiceProcessor?.start(
            _porcupine!.frameLength, _porcupine!.sampleRate);
      } on PlatformException catch (e) {
        throw PorcupineRuntimeException(
            "Failed to start audio recording: ${e.message}");
      }
    } else {
      throw PorcupineRuntimeException(
          "User did not give permission to record audio.");
    }

    _isListening = true;
  }

  /// Stops audio recording and processing
  /// Throws a `PorcupineException` if there was a problem stopping audio recording.
  Future<void> stop() async {
    if (!_isListening) {
      return;
    }

    _voiceProcessor?.removeErrorListener(_errorListener);
    _voiceProcessor?.removeFrameListener(_frameListener);

    if (_voiceProcessor?.numFrameListeners == 0) {
      try {
        await _voiceProcessor?.stop();
      } on PlatformException catch (e) {
        throw PorcupineRuntimeException(
            "Failed to stop audio recording: ${e.message}");
      }
    }

    _isListening = false;
  }

  /// Releases Porcupine and audio resources
  /// Throws a `PorcupineException` if there was a problem stopping audio recording.
  Future<void> delete() async {
    await stop();
    _voiceProcessor = null;

    _porcupine?.delete();
    _porcupine = null;
  }
}
