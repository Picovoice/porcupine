import 'dart:ffi';
import 'dart:async';
import 'dart:io';
import 'dart:typed_data';

import 'package:flutter/services.dart';
import 'package:ffi/ffi.dart';
import 'package:flutter/services.dart' show rootBundle;
import 'package:path_provider/path_provider.dart';

final _porcupineLib = _load();
DynamicLibrary _load() {
  if (Platform.isAndroid) {
    return DynamicLibrary.open("libpv_porcupine.so");
  } else {
    return DynamicLibrary.process();
  }
}

// pv_porcupine_init
typedef NativeInit = Int32 Function(Pointer<Utf8>, Int32,
    Pointer<Pointer<Utf8>>, Pointer<Float>, Pointer<IntPtr>);
typedef Init = int Function(Pointer<Utf8>, int, Pointer<Pointer<Utf8>>,
    Pointer<Float>, Pointer<IntPtr>);
final Init _porcupineInit =
    _porcupineLib.lookupFunction<NativeInit, Init>('pv_porcupine_init');

// pv_porcupine_process
typedef NativeProcess = Int32 Function(IntPtr, Pointer<Int16>, Pointer<Int32>);
typedef Process = int Function(int, Pointer<Int16>, Pointer<Int32>);
final Process _porcupineProcess = _porcupineLib
    .lookupFunction<NativeProcess, Process>('pv_porcupine_process');

// pv_porcupine_delete
typedef NativeDelete = Void Function(IntPtr);
typedef Delete = void Function(int);
final Delete _porcupineDelete =
    _porcupineLib.lookupFunction<NativeDelete, Delete>('pv_porcupine_delete');

// pv_porcupine_version
typedef NativeVersion = Pointer<Utf8> Function();
typedef Version = Pointer<Utf8> Function();
final Version _porcupineVersion = _porcupineLib
    .lookupFunction<NativeVersion, Version>('pv_porcupine_version');

// pv_porcupine_frame_length
typedef NativeFrameLength = Int32 Function();
typedef FrameLength = int Function();
final FrameLength _porcupineFrameLength =
    _porcupineLib.lookupFunction<NativeFrameLength, FrameLength>(
        'pv_porcupine_frame_length');

// pv_sample_rate
typedef NativeSampleRate = Int32 Function();
typedef SampleRate = int Function();
final SampleRate _porcupineSampleRate = _porcupineLib
    .lookupFunction<NativeSampleRate, SampleRate>('pv_sample_rate');

class Porcupine {
  int _handle;
  int _frameLength;
  int _sampleRate;
  String _version;
  Pointer<Int16> cFrame;

  static const List<String> BUILT_IN_KEYWORDS = ["porcupine"];

  static bool _resourcesExtracted = false;
  static String _defaultModelPath;

  // a list of built-in keywords
  static Map<String, String> _builtInKeywordPaths;

  static Future<Porcupine> fromKeywords(List<String> keywords,
          {String modelPath, List<double> sensitivities}) =>
      Porcupine._create(
          keywords: keywords,
          modelPath: modelPath,
          sensitivities: sensitivities);

  static Future<Porcupine> fromKeywordPaths(List<String> keywordPaths,
          {String modelPath, List<double> sensitivities}) =>
      Porcupine._create(
          keywordPaths: keywordPaths,
          modelPath: modelPath,
          sensitivities: sensitivities);

  static Future<Porcupine> _create(
      {List<String> keywords,
      List<String> keywordPaths,
      String modelPath,
      List<double> sensitivities}) async {
    if (!_resourcesExtracted) {
      await _extractPorcupineResources();
      _resourcesExtracted = true;
    }

    modelPath ??= _defaultModelPath;

    if (keywords != null && keywordPaths != null) {
      throw new ArgumentError(
          "Both 'keywords' and 'keywordPaths' were set. Only one of the two arguments may be set for intializtion.");
    }

    if (keywordPaths == null || keywordPaths.length == 0) {
      if (keywords == null || keywords.length == 0) {
        throw new ArgumentError(
            "No built-in or custom keywords provided. Either 'keywords' or 'keywordPaths' must be set.");
      }

      keywordPaths = new List(keywords.length);
      for (var i = 0; i < keywords.length; i++) {
        if (BUILT_IN_KEYWORDS.contains(keywords[i])) {
          keywordPaths[i] = _builtInKeywordPaths[keywords[i]];
        } else {
          throw new ArgumentError(
              "One or more keywords are not available by default. Available default keywords are: $BUILT_IN_KEYWORDS");
        }
      }
    }

    if (sensitivities == null || sensitivities.length == 0) {
      sensitivities = List.filled(keywordPaths.length, 0.5);
    } else {
      for (double sensitivity in sensitivities) {
        if (sensitivity < 0 || sensitivity > 1 || sensitivity == double.nan) {
          throw new ArgumentError(
              "Sensitivity value ($sensitivity) in given 'sensitivities' not in range [0,1]");
        }
      }

      if (keywordPaths.length != sensitivities.length) {
        throw new ArgumentError(
            "Number of keywords (${keywordPaths.length}) does not match number of sensitivities (${sensitivities.length})");
      }
    }

    // generate arguments for ffi
    Pointer<Utf8> cModelPath = Utf8.toUtf8(_defaultModelPath);
    Pointer<Pointer<Utf8>> cKeywordPaths = allocate(count: keywordPaths.length);
    for (var i = 0; i < keywordPaths.length; i++) {
      cKeywordPaths[i] = Utf8.toUtf8(keywordPaths[i]);
    }

    Pointer<Float> cSensitivities =
        allocate<Float>(count: sensitivities.length);
    cSensitivities
        .asTypedList(sensitivities.length)
        .setRange(0, sensitivities.length, sensitivities);

    Pointer<IntPtr> handlePtr = allocate<IntPtr>(count: 1);
    int status = _porcupineInit(cModelPath, keywordPaths.length, cKeywordPaths,
        cSensitivities, handlePtr);

    if (status != 0) {
      throw new StateError("Porcupine init returned an error");
    }

    String version = Utf8.fromUtf8(_porcupineVersion());
    int frameLength = _porcupineFrameLength();
    int sampleRate = _porcupineSampleRate();
    int handle = handlePtr.value;
    return new Porcupine._(handle, frameLength, sampleRate, version);
  }

  // private constructor
  Porcupine._(
      this._handle, this._frameLength, this._sampleRate, this._version) {
    cFrame = allocate<Int16>(count: _frameLength);
  }

  int process(List<int> frame) {
    if (frame == null) {
      throw new ArgumentError("Frame array provided to process() was null.");
    }

    if (frame.length != _frameLength) {
      throw new ArgumentError(
          "Size of frame array provided to 'process' (${frame.length}) does not match the engine 'frameLength' (${_frameLength})");
    }

    cFrame.asTypedList(frame.length).setAll(0, frame);
    Pointer<Int32> keywordIndexPtr = allocate(count: 1);
    int status = _porcupineProcess(_handle, cFrame, keywordIndexPtr);
    if (status != 0) {
      throw new StateError("Porcupine process returned an error");
    }

    return keywordIndexPtr.value;
  }

  void delete() {
    _porcupineDelete(_handle);
  }

  String get version => _version;
  int get frameLength => _frameLength;
  int get sampleRate => _sampleRate;

  static final _platform = Platform.isAndroid ? "android" : "ios";

  static Future<void> _extractPorcupineResources() async {
    _defaultModelPath =
        await _extractResource("lib/common/porcupine_params.pv");

    _builtInKeywordPaths = new Map();
    String keywordAssetDir = "resources/keyword_files/$_platform";
    for (var i = 0; i < BUILT_IN_KEYWORDS.length; i++) {
      String keyword = BUILT_IN_KEYWORDS[i];
      String keywordAssetPath = "$keywordAssetDir/${keyword}_$_platform.ppn";
      String extractedPath = await _extractResource(keywordAssetPath);

      if (extractedPath != null) {
        _builtInKeywordPaths[keyword] = extractedPath;
      }
    }
  }

  static Future<String> _extractResource(String filePath) async {
    String resourceDirectory = (await getApplicationDocumentsDirectory()).path;
    String outputPath = '$resourceDirectory/$filePath';
    File outputFile = new File(outputPath);

    ByteData data = await rootBundle.load(filePath);
    final buffer = data.buffer;

    await outputFile.create(recursive: true);
    await outputFile.writeAsBytes(
        buffer.asUint8List(data.offsetInBytes, data.lengthInBytes));
    return outputPath;
  }
}
