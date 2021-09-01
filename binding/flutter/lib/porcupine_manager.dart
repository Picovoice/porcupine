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
typedef ErrorCallback(PvError error);

class PorcupineManager {
  /// a list of built-in keywords
  static const List<String> BUILT_IN_KEYWORDS = Porcupine.BUILT_IN_KEYWORDS;

  VoiceProcessor? _voiceProcessor;
  Porcupine? _porcupine;

  final WakeWordCallback _wakeWordCallback;
  RemoveListener? _removeVoiceProcessorListener;
  RemoveListener? _removeErrorListener;

  /// Static creator for initializing PorcupineManager from a selection of built-in keywords
  ///
  /// [keywords] is a List of (phrases) for detection. The list of available
  /// keywords can be retrieved using [Porcupine.BUILT_IN_KEYWORDS]
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
  /// returns an instance of PorcupineManager
  static Future<PorcupineManager> fromKeywords(
      List<String> keywords, WakeWordCallback wakeWordCallback,
      {String? modelPath,
      List<double>? sensitivities,
      ErrorCallback? errorCallback}) async {
    Porcupine porcupine = await Porcupine.fromKeywords(keywords,
        modelPath: modelPath, sensitivities: sensitivities);
    return new PorcupineManager._(porcupine, wakeWordCallback, errorCallback);
  }

  /// Static creator for initializing PorcupineManager from a list of paths to custom keyword files
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
  /// experiences a problem while processing audio
  ///
  /// Thows a `PvError` if not initialized correctly
  ///
  /// returns an instance of PorcupineManager
  static Future<PorcupineManager> fromKeywordPaths(
      List<String> keywordPaths, WakeWordCallback wakeWordCallback,
      {String? modelPath,
      List<double>? sensitivities,
      ErrorCallback? errorCallback}) async {
    Porcupine porcupine = await Porcupine.fromKeywordPaths(keywordPaths,
        modelPath: modelPath, sensitivities: sensitivities);
    return new PorcupineManager._(porcupine, wakeWordCallback, errorCallback);
  }

  // private constructor
  PorcupineManager._(
      this._porcupine, this._wakeWordCallback, ErrorCallback? errorCallback)
      : _voiceProcessor = VoiceProcessor.getVoiceProcessor(
            Porcupine.frameLength, Porcupine.sampleRate) {
    _removeVoiceProcessorListener = _voiceProcessor?.addListener((buffer) {
      // cast from dynamic to int array
      List<int> porcupineFrame;
      try {
        porcupineFrame = (buffer as List<dynamic>).cast<int>();
      } on Error {
        PvError castError = new PvError(
            "flutter_voice_processor sent an unexpected data type.");
        errorCallback == null
            ? print(castError.message)
            : errorCallback(castError);
        return;
      }

      // process frame with Porcupine
      try {
        int? keywordIndex = _porcupine?.process(porcupineFrame);
        if (keywordIndex != null && keywordIndex >= 0) {
          _wakeWordCallback(keywordIndex);
        }
      } on PvError catch (error) {
        errorCallback == null ? print(error.message) : errorCallback(error);
      }
    });

    _removeErrorListener = _voiceProcessor?.addErrorListener((errorMsg) {
      PvError nativeRecorderError = new PvError(errorMsg as String);
      errorCallback == null
          ? print(nativeRecorderError.message)
          : errorCallback(nativeRecorderError);
    });
  }

  /// Opens audio input stream and sends audio frames to Porcupine
  /// Throws a `PvAudioException` if there was a problem starting the audio engine
  Future<void> start() async {
    if (_porcupine == null || _voiceProcessor == null) {
      throw new PvStateError(
          "Cannot start Porcupine - resources have already been released");
    }

    if (await _voiceProcessor?.hasRecordAudioPermission() ?? false) {
      try {
        await _voiceProcessor?.start();
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
  Future<void> stop() async => await _voiceProcessor?.stop();

  /// Releases Porcupine and audio resources
  Future<void> delete() async {
    if (_voiceProcessor?.isRecording ?? false) {
      await _voiceProcessor?.stop();
    }
    _removeVoiceProcessorListener?.call();
    _removeErrorListener?.call();
    _voiceProcessor = null;

    _porcupine?.delete();
    _porcupine = null;
  }
}
