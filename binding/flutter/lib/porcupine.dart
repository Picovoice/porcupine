//
// Copyright 2020-2021 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
// file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//

import 'dart:ffi';
import 'dart:async';
import 'dart:io';
import 'dart:typed_data';

import 'package:flutter/services.dart';
import 'package:ffi/ffi.dart';
import 'package:flutter/services.dart' show rootBundle;
import 'package:path_provider/path_provider.dart';
import 'package:porcupine/porcupine_error.dart';

enum BuiltInKeyword {
  ALEXA,
  AMERICANO,
  BLUEBERRY,
  BUMBLEBEE,
  COMPUTER,
  GRAPEFRUIT,
  GRASSHOPPER,
  HEY_GOOGLE,
  HEY_SIRI,
  JARVIS,
  OK_GOOGLE,
  PICOVOICE,
  PORCUPINE,
  TERMINATOR
}

class Porcupine {
  static bool _resourcesExtracted = false;
  static String? _defaultModelPath;

  static Map<BuiltInKeyword, String>? _builtInKeywordPaths;

  int? _handle;
  final Pointer<Int16> _cFrame;

  /// Porcupine version string
  static String get version => _porcupineVersion().toDartString();

  /// The number of audio samples per frame required by Porcupine
  static int get frameLength => _porcupineFrameLength();

  /// The audio sample rate required by Porcupine
  static int get sampleRate => _porcupineSampleRate();

  /// Static creator for initializing Porcupine from a selection of built-in keywords
  ///
  /// [accessKey] AccessKey obtained from Picovoice Console (https://console.picovoice.ai/).
  ///
  /// [keywords] is a List of (phrases) for detection. The list of available
  /// keywords can be retrieved using [BuiltInKeyword] enum.
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
  /// returns an instance of the wake word engine
  static Future<Porcupine> fromBuiltInKeywords(
      String accessKey, List<BuiltInKeyword>? keywords,
      {String? modelPath, List<double>? sensitivities}) {
    if (keywords == null) {
      throw PorcupineInvalidArgumentException("'keywords' must be set.");
    }

    List<String> keywordPaths = List.empty(growable: true);
    for (var builtIn in keywords) {
      keywordPaths.add(_getResourcePath(builtIn));
    }

    return Porcupine._create(
        accessKey: accessKey,
        keywordPaths: keywordPaths,
        modelPath: modelPath,
        sensitivities: sensitivities);
  }

  /// Static creator for initializing Porcupine from a list of paths to custom keyword files
  ///
  /// [accessKey] AccessKey obtained from Picovoice Console (https://console.picovoice.ai/).
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
  /// returns an instance of the wake word engine
  static Future<Porcupine> fromKeywordPaths(
          String accessKey, List<String?>? keywordPaths,
          {String? modelPath, List<double>? sensitivities}) =>
      Porcupine._create(
          accessKey: accessKey,
          keywordPaths: keywordPaths,
          modelPath: modelPath,
          sensitivities: sensitivities);

  /// private static creator
  static Future<Porcupine> _create(
      {String? accessKey,
      List<String?>? keywordPaths,
      String? modelPath,
      List<double>? sensitivities}) async {
    if (!_resourcesExtracted) {
      await _extractPorcupineResources();
      _resourcesExtracted = true;
    }

    if (accessKey == null || accessKey == "") {
      throw PorcupineInvalidArgumentException("'accessKey' must be set.");
    }

    if (modelPath == null || modelPath == "") {
      modelPath = _defaultModelPath;
    }

    if (!await File(modelPath!).exists()) {
      try {
        modelPath = await _extractResource(modelPath);
      } catch (_) {
        throw PorcupineInvalidArgumentException(
            "Could not find model file at path '$modelPath'. If this is a packaged asset, ensure you have added it to your pubspec.yaml file.");
      }
    }

    if (keywordPaths == null || keywordPaths.isEmpty) {
      throw PorcupineInvalidArgumentException("'keywordPaths' must be set.");
    }

    for (var i = 0; i < keywordPaths.length; i++) {
      if (keywordPaths[i] == null || keywordPaths[i] == "") {
        throw PorcupineInvalidArgumentException(
            "One of the provided keyword paths was empty.");
      }

      if (!await File(keywordPaths[i]!).exists()) {
        try {
          keywordPaths[i] = await _extractResource(keywordPaths[i]!);
        } catch (_) {
          throw PorcupineInvalidArgumentException(
              "Could not find keyword file at path '${keywordPaths[i]}'. If this is a packaged asset, ensure you have added it to your pubspec.yaml file.");
        }
      }
    }

    if (sensitivities == null || sensitivities.isEmpty) {
      sensitivities = List.filled(keywordPaths.length, 0.5);
    } else {
      for (double sensitivity in sensitivities) {
        if (sensitivity < 0 || sensitivity > 1 || sensitivity == double.nan) {
          throw PorcupineInvalidArgumentException(
              "Sensitivity value ($sensitivity) in given 'sensitivities' not in range [0,1]");
        }
      }

      if (keywordPaths.length != sensitivities.length) {
        throw PorcupineInvalidArgumentException(
            "Number of keywords (${keywordPaths.length}) does not match number of sensitivities (${sensitivities.length})");
      }
    }

    // generate arguments for ffi
    Pointer<Utf8> cAccessKey = accessKey.toNativeUtf8();
    Pointer<Utf8> cModelPath = modelPath.toNativeUtf8();
    Pointer<Pointer<Utf8>> cKeywordPaths = malloc(keywordPaths.length);
    for (var i = 0; i < keywordPaths.length; i++) {
      if (keywordPaths[i]?.toNativeUtf8() != null) {
        cKeywordPaths[i] = keywordPaths[i]!.toNativeUtf8();
      }
    }

    Pointer<Float> cSensitivities = malloc<Float>(sensitivities.length);
    cSensitivities
        .asTypedList(sensitivities.length)
        .setRange(0, sensitivities.length, sensitivities);

    Pointer<IntPtr> handlePtr = malloc<IntPtr>(1);

    String v = _porcupineVersion().toDartString();
    print(v);

    // init porcupine
    int status = _porcupineInit(cAccessKey, cModelPath, keywordPaths.length,
        cKeywordPaths, cSensitivities, handlePtr);
    PorcupineStatus pvStatus = PorcupineStatus.values[status];
    if (pvStatus != PorcupineStatus.SUCCESS) {
      porcupineStatusToException(pvStatus, "Failed to initialize Porcupine.");
    }

    int handle = handlePtr.value;
    return Porcupine._(handle);
  }

  // private constructor
  Porcupine._(this._handle) : _cFrame = malloc<Int16>(frameLength);

  /// Process a frame of audio with the wake word engine.
  ///
  /// [frame] is a frame of frame of audio samples to be assessed by Porcupine. The required audio format is found by
  /// calling `.sampleRate` to get the required sample rate and `.frameLength` to get the required frame size.
  /// Audio must be single-channel and 16-bit linearly-encoded.
  ///
  /// Index of the detected keyword, or -1 if no detection occurred
  int process(List<int>? frame) {
    if (_handle == null) {
      throw PorcupineInvalidStateException(
          "Attempted to process an audio frame after Porcupine was been deleted.");
    }

    if (frame == null) {
      throw PorcupineInvalidArgumentException(
          "Frame array provided to Porcupine process was null.");
    }

    if (frame.length != frameLength) {
      throw PorcupineInvalidArgumentException(
          "Size of frame array provided to 'process' (${frame.length}) does not match the engine 'frameLength' ($frameLength)");
    }

    // generate arguments for ffi
    _cFrame.asTypedList(frame.length).setAll(0, frame);
    Pointer<Int32> keywordIndexPtr = malloc(1);

    int status = _porcupineProcess(_handle!, _cFrame, keywordIndexPtr);
    PorcupineStatus pvStatus = PorcupineStatus.values[status];
    if (pvStatus != PorcupineStatus.SUCCESS) {
      porcupineStatusToException(
          pvStatus, "Porcupine failed to process an audio frame.");
    }

    return keywordIndexPtr.value;
  }

  /// Frees memory that was allocated for Porcupine
  void delete() {
    if (_handle != null) {
      _porcupineDelete(_handle!);
      _handle = null;
    }
  }

  static final String _platform = Platform.isAndroid ? "android" : "ios";
  static const String _assetDir = "packages/porcupine/assets";
  static final String _keywordAssetDir =
      "$_assetDir/resources/keyword_files/$_platform";

  static String _getResourcePath(BuiltInKeyword builtIn) {
    String keyword =
        builtIn.toString().split(".").last.replaceAll("_", " ").toLowerCase();
    return "$_keywordAssetDir/${keyword}_$_platform.ppn";
  }

  static Future<void> _extractPorcupineResources() async {
    _defaultModelPath =
        await _extractResource("$_assetDir/lib/common/porcupine_params.pv");

    _builtInKeywordPaths = {};
    for (var builtIn in BuiltInKeyword.values) {
      String keywordAssetPath = _getResourcePath(builtIn);
      String extractedPath = await _extractResource(keywordAssetPath);

      _builtInKeywordPaths?[builtIn] = extractedPath;
    }
  }

  static Future<String> _extractResource(String filePath) async {
    String resourceDirectory = (await getApplicationDocumentsDirectory()).path;
    String outputPath = '$resourceDirectory/$filePath';
    File outputFile = File(outputPath);

    ByteData data = await rootBundle.load(filePath);
    final buffer = data.buffer;

    await outputFile.create(recursive: true);
    await outputFile.writeAsBytes(
        buffer.asUint8List(data.offsetInBytes, data.lengthInBytes));
    return outputPath;
  }
}

// loads lib
final _porcupineLib = _load();

DynamicLibrary _load() {
  if (Platform.isAndroid) {
    return DynamicLibrary.open("libpv_porcupine.so");
  } else {
    return DynamicLibrary.process();
  }
}

// pv_porcupine_init
typedef NativeInit = Int32 Function(Pointer<Utf8>, Pointer<Utf8>, Int32,
    Pointer<Pointer<Utf8>>, Pointer<Float>, Pointer<IntPtr>);
typedef Init = int Function(Pointer<Utf8>, Pointer<Utf8>, int,
    Pointer<Pointer<Utf8>>, Pointer<Float>, Pointer<IntPtr>);

final Init _porcupineInit = _porcupineLib
    .lookup<NativeFunction<NativeInit>>('pv_porcupine_init')
    .asFunction();

// pv_porcupine_process
typedef NativeProcess = Int32 Function(IntPtr, Pointer<Int16>, Pointer<Int32>);
typedef Process = int Function(int, Pointer<Int16>, Pointer<Int32>);

final Process _porcupineProcess = _porcupineLib
    .lookup<NativeFunction<NativeProcess>>('pv_porcupine_process')
    .asFunction();

// pv_porcupine_delete
typedef NativeDelete = Void Function(IntPtr);
typedef Delete = void Function(int);

final Delete _porcupineDelete = _porcupineLib
    .lookup<NativeFunction<NativeDelete>>('pv_porcupine_delete')
    .asFunction();

// pv_porcupine_version
typedef NativeVersion = Pointer<Utf8> Function();
typedef Version = Pointer<Utf8> Function();

final Version _porcupineVersion = _porcupineLib
    .lookup<NativeFunction<NativeVersion>>('pv_porcupine_version')
    .asFunction();

// pv_porcupine_frame_length
typedef NativeFrameLength = Int32 Function();
typedef FrameLength = int Function();

final FrameLength _porcupineFrameLength = _porcupineLib
    .lookup<NativeFunction<NativeFrameLength>>('pv_porcupine_frame_length')
    .asFunction();

// pv_sample_rate
typedef NativeSampleRate = Int32 Function();
typedef SampleRate = int Function();

final SampleRate _porcupineSampleRate = _porcupineLib
    .lookup<NativeFunction<NativeSampleRate>>('pv_sample_rate')
    .asFunction();
