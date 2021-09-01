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
import 'package:porcupine/porcupine_manager.dart';
import 'package:porcupine/porcupine_error.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatefulWidget {
  @override
  _MyAppState createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> with WidgetsBindingObserver {
  final GlobalKey<ScaffoldState> _scaffoldKey = GlobalKey<ScaffoldState>();

  bool isButtonDisabled = false;
  bool isProcessing = false;
  Color detectionColour = new Color(0xff00e5c3);
  Color defaultColour = new Color(0xfff5fcff);
  Color? backgroundColour;
  String currentKeyword = "Click to choose a keyword";
  PorcupineManager? _porcupineManager;
  @override
  void initState() {
    super.initState();
    this.setState(() {
      isButtonDisabled = true;
      backgroundColour = defaultColour;
    });
    WidgetsBinding.instance?.addObserver(this);
  }

  @override
  void dispose() {
    WidgetsBinding.instance?.removeObserver(this);
    super.dispose();
  }

  @override
  void didChangeAppLifecycleState(AppLifecycleState state) async {
    if (state == AppLifecycleState.inactive) {
      await _stopProcessing();
      await _porcupineManager?.delete();
      _porcupineManager = null;
    }
  }

  Future<void> loadNewKeyword(String keyword) async {
    this.setState(() {
      isButtonDisabled = true;
    });

    if (isProcessing) {
      await _stopProcessing();
    }

    if (_porcupineManager != null) {
      await _porcupineManager?.delete();
      _porcupineManager = null;
    }
    try {
      _porcupineManager = await PorcupineManager.fromKeywords(
          [keyword], wakeWordCallback,
          errorCallback: errorCallback);
      this.setState(() {
        currentKeyword = keyword;
      });
    } on PvError catch (ex) {
      print("Failed to initialize Porcupine: ${ex.message}");
    } finally {
      this.setState(() {
        isButtonDisabled = false;
      });
    }
  }

  void wakeWordCallback(int keywordIndex) {
    if (keywordIndex >= 0) {
      this.setState(() {
        backgroundColour = detectionColour;
      });
      Future.delayed(const Duration(milliseconds: 1000), () {
        this.setState(() {
          backgroundColour = defaultColour;
        });
      });
    }
  }

  void errorCallback(PvError error) {
    print(error.message);
  }

  Future<void> _startProcessing() async {
    this.setState(() {
      isButtonDisabled = true;
    });

    if (_porcupineManager == null) {
      await loadNewKeyword(currentKeyword);
    }

    try {
      await _porcupineManager?.start();
      this.setState(() {
        isProcessing = true;
      });
    } on PvAudioException catch (ex) {
      print("Failed to start audio capture: ${ex.message}");
    } finally {
      this.setState(() {
        isButtonDisabled = false;
      });
    }
  }

  Future<void> _stopProcessing() async {
    this.setState(() {
      isButtonDisabled = true;
    });

    await _porcupineManager?.stop();

    this.setState(() {
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
          children: [buildPicker(context), buildStartButton(context), footer],
        ),
      ),
    );
  }

  buildPicker(BuildContext context) {
    return new Expanded(
      flex: 1,
      child: Container(
          alignment: Alignment.bottomCenter,
          // color: Colors.blue,
          child: FractionallySizedBox(
              widthFactor: 0.9,
              child: OutlinedButton(
                child: Text(currentKeyword,
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

    return new Expanded(
      flex: 2,
      child: Container(
          child: SizedBox(
              width: 150,
              height: 150,
              child: ElevatedButton(
                style: buttonStyle,
                onPressed: isButtonDisabled ? null : _toggleProcessing,
                child: Text(isProcessing ? "Stop" : "Start",
                    style: TextStyle(fontSize: 30)),
              ))),
    );
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
    Picker picker = new Picker(
        adapter: PickerDataAdapter<String>(
            pickerdata: PorcupineManager.BUILT_IN_KEYWORDS),
        changeToFirst: true,
        textAlign: TextAlign.left,
        columnPadding: const EdgeInsets.all(8.0),
        onConfirm: (Picker picker, List value) {
          loadNewKeyword(picker.getSelectedValues()[0]);
        });
    picker.show(_scaffoldKey.currentState!);
  }
}
