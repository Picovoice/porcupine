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

import 'package:flutter/material.dart';
import 'dart:async';

import 'package:flutter_picker/flutter_picker.dart';
import 'package:porcupine_flutter/porcupine.dart';
import 'package:porcupine_flutter/porcupine_manager.dart';
import 'package:porcupine_flutter/porcupine_error.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatefulWidget {
  @override
  _MyAppState createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> with WidgetsBindingObserver {
  final String accessKey =
      "{YOUR_ACCESS_KEY_HERE}"; // AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)

  final GlobalKey<ScaffoldState> _scaffoldKey = GlobalKey<ScaffoldState>();
  final Map<String, BuiltInKeyword> _keywordMap = {};

  bool isError = false;
  String errorMessage = "";

  bool isButtonDisabled = false;
  bool isProcessing = false;
  Color detectionColour = Color(0xff00e5c3);
  Color defaultColour = Color(0xfff5fcff);
  Color? backgroundColour;
  String currentKeyword = "Click to choose a keyword";
  PorcupineManager? _porcupineManager;
  @override
  void initState() {
    super.initState();
    setState(() {
      isButtonDisabled = true;
      backgroundColour = defaultColour;
    });
    WidgetsBinding.instance?.addObserver(this);
    _initializeKeywordMap();
  }

  @override
  void dispose() {
    WidgetsBinding.instance?.removeObserver(this);
    super.dispose();
  }

  @override
  void didChangeAppLifecycleState(AppLifecycleState state) async {
    if (state == AppLifecycleState.paused) {
      await _stopProcessing();
      await _porcupineManager?.delete();
      _porcupineManager = null;
    }
  }

  Future<void> loadNewKeyword(String keyword) async {
    setState(() {
      isButtonDisabled = true;
    });

    if (!_keywordMap.containsKey(keyword)) {
      return;
    }

    BuiltInKeyword builtIn = _keywordMap[keyword]!;

    if (isProcessing) {
      await _stopProcessing();
    }

    if (_porcupineManager != null) {
      await _porcupineManager?.delete();
      _porcupineManager = null;
    }
    try {
      _porcupineManager = await PorcupineManager.fromBuiltInKeywords(
          accessKey, [builtIn], wakeWordCallback,
          errorCallback: errorCallback);
      setState(() {
        currentKeyword = keyword;
        isError = false;
      });
    } on PorcupineInvalidArgumentException catch (ex) {
      errorCallback(PorcupineInvalidArgumentException(
          "${ex.message}\nEnsure your accessKey '$accessKey' is a valid access key."));
    } on PorcupineActivationException {
      errorCallback(
          PorcupineActivationException("AccessKey activation error."));
    } on PorcupineActivationLimitException {
      errorCallback(PorcupineActivationLimitException(
          "AccessKey reached its device limit."));
    } on PorcupineActivationRefusedException {
      errorCallback(PorcupineActivationRefusedException("AccessKey refused."));
    } on PorcupineActivationThrottledException {
      errorCallback(PorcupineActivationThrottledException(
          "AccessKey has been throttled."));
    } on PorcupineException catch (ex) {
      errorCallback(ex);
    } finally {
      setState(() {
        isButtonDisabled = false;
      });
    }
  }

  void wakeWordCallback(int keywordIndex) {
    if (keywordIndex >= 0) {
      setState(() {
        backgroundColour = detectionColour;
      });
      Future.delayed(const Duration(milliseconds: 1000), () {
        setState(() {
          backgroundColour = defaultColour;
        });
      });
    }
  }

  void errorCallback(PorcupineException error) {
    setState(() {
      isError = true;
      errorMessage = error.message!;
    });
  }

  Future<void> _startProcessing() async {
    setState(() {
      isButtonDisabled = true;
    });

    if (_porcupineManager == null) {
      await loadNewKeyword(currentKeyword);
    }

    try {
      await _porcupineManager?.start();
      setState(() {
        isProcessing = true;
      });
    } on PorcupineException catch (ex) {
      errorCallback(ex);
    } finally {
      setState(() {
        isButtonDisabled = false;
      });
    }
  }

  Future<void> _stopProcessing() async {
    setState(() {
      isButtonDisabled = true;
    });

    await _porcupineManager?.stop();

    setState(() {
      isButtonDisabled = false;
      isProcessing = false;
    });
  }

  void _toggleProcessing() async {
    if (isProcessing) {
      await _stopProcessing();
    } else {
      await _startProcessing();
    }
  }

  void _initializeKeywordMap() {
    for (var builtIn in BuiltInKeyword.values) {
      String keyword =
          builtIn.toString().split(".").last.replaceAll("_", " ").toLowerCase();
      _keywordMap[keyword] = builtIn;
    }
  }

  Color picoBlue = Color.fromRGBO(55, 125, 255, 1);
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        key: _scaffoldKey,
        backgroundColor: backgroundColour,
        appBar: AppBar(
          title: const Text('Porcupine Demo'),
          backgroundColor: picoBlue,
        ),
        body: Column(
          children: [
            buildPicker(context),
            buildStartButton(context),
            buildErrorMessage(context),
            footer
          ],
        ),
      ),
    );
  }

  buildPicker(BuildContext context) {
    return Expanded(
      flex: 1,
      child: Container(
          alignment: Alignment.bottomCenter,
          // color: Colors.blue,
          child: FractionallySizedBox(
              widthFactor: 0.9,
              child: OutlinedButton(
                child: Text(currentKeyword.toString(),
                    style: TextStyle(fontSize: 20, color: picoBlue)),
                onPressed: () {
                  showPicker(context);
                },
              ))),
    );
  }

  buildStartButton(BuildContext context) {
    final ButtonStyle buttonStyle = ElevatedButton.styleFrom(
        primary: picoBlue,
        shape: CircleBorder(),
        textStyle: TextStyle(color: Colors.white));

    return Expanded(
      flex: 2,
      child: Container(
          child: SizedBox(
              width: 150,
              height: 150,
              child: ElevatedButton(
                style: buttonStyle,
                onPressed:
                    (isButtonDisabled || isError) ? null : _toggleProcessing,
                child: Text(isProcessing ? "Stop" : "Start",
                    style: TextStyle(fontSize: 30)),
              ))),
    );
  }

  buildErrorMessage(BuildContext context) {
    return Expanded(
        flex: 1,
        child: Container(
            alignment: Alignment.center,
            margin: EdgeInsets.only(left: 20, right: 20),
            decoration: !isError
                ? null
                : BoxDecoration(
                    color: Colors.red, borderRadius: BorderRadius.circular(5)),
            child: !isError
                ? null
                : Text(
                    errorMessage,
                    style: TextStyle(color: Colors.white, fontSize: 20),
                  )));
  }

  Widget footer = Expanded(
      flex: 1,
      child: Container(
          alignment: Alignment.bottomCenter,
          padding: EdgeInsets.only(bottom: 20),
          child: const Text(
            "Made in Vancouver, Canada by Picovoice",
            style: TextStyle(color: Color(0xff666666)),
          )));

  showPicker(BuildContext context) {
    Picker picker = Picker(
        adapter:
            PickerDataAdapter<String>(pickerData: _keywordMap.keys.toList()),
        changeToFirst: true,
        textAlign: TextAlign.left,
        columnPadding: const EdgeInsets.all(8.0),
        onConfirm: (Picker picker, List value) {
          loadNewKeyword(picker.getSelectedValues()[0]);
        });
    picker.show(_scaffoldKey.currentState!);
  }
}
