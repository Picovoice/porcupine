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

import React, {Component} from 'react';
import {PermissionsAndroid, Platform, TouchableOpacity} from 'react-native';
import {StyleSheet, Text, View} from 'react-native';
import {
  PorcupineManager,
  BuiltInKeywords,
  PorcupineErrors,
} from '@picovoice/porcupine-react-native';
import {Picker} from '@react-native-picker/picker';

type Props = {};
type State = {
  currentKeyword: BuiltInKeywords;
  keywordOptions: (string | BuiltInKeywords)[];
  buttonText: string;
  buttonDisabled: boolean;
  isListening: boolean;
  backgroundColour: string;
  isError: boolean;
  errorMessage: string;
};

export default class App extends Component<Props, State> {
  _porcupineManager: PorcupineManager | undefined;
  _detectionColour: string = '#00E5C3';
  _defaultColour: string = '#F5FCFF';
  _accessKey: string = '${YOUR_ACCESS_KEY_HERE}'; // AccessKey obtained from Picovoice Console (https://console.picovoice.ai/)

  constructor(props: Props) {
    super(props);
    this.state = {
      keywordOptions: Object.values(BuiltInKeywords),
      currentKeyword: BuiltInKeywords.ALEXA,
      buttonText: 'Start',
      buttonDisabled: false,
      isListening: false,
      backgroundColour: this._defaultColour,
      isError: false,
      errorMessage: null,
    };
  }

  async componentDidMount() {
    this._loadNewKeyword(this.state.currentKeyword);
  }

  componentWillUnmount() {
    if (this.state.isListening) {
      this._stopProcessing();
    }
    this._porcupineManager?.delete();
  }

  async _startProcessing() {
    this.setState({
      buttonDisabled: true,
    });

    let recordAudioRequest;
    if (Platform.OS === 'android') {
      recordAudioRequest = this._requestRecordAudioPermission();
    } else {
      recordAudioRequest = new Promise(function (resolve, _) {
        resolve(true);
      });
    }

    recordAudioRequest.then((hasPermission) => {
      if (!hasPermission) {
        console.error(
          "Required permissions (Microphone) we're not found. Please add to platform code.",
        );
        this.setState({
          buttonDisabled: false,
        });
        return;
      }

      this._porcupineManager?.start().then((didStart) => {
        if (didStart) {
          this.setState({
            buttonText: 'Stop',
            buttonDisabled: false,
            isListening: true,
          });
        }
      });
    });
  }

  _stopProcessing() {
    this.setState({
      buttonDisabled: true,
    });

    this._porcupineManager?.stop().then((didStop) => {
      if (didStop) {
        this.setState({
          buttonText: 'Start',
          buttonDisabled: false,
          isListening: false,
        });
      }
    });
  }

  _toggleListening() {
    if (this.state.isListening) {
      this._stopProcessing();
    } else {
      this._startProcessing();
    }
  }

  async _loadNewKeyword(keyword: BuiltInKeywords) {
    if (this.state.isListening) {
      this._stopProcessing();
    }
    this._porcupineManager?.delete();

    this.setState({currentKeyword: keyword});
    try {
      this._porcupineManager = await PorcupineManager.fromBuiltInKeywords(
        this._accessKey,
        [keyword],
        (keywordIndex: number) => {
          if (keywordIndex >= 0) {
            this.setState({
              backgroundColour: this._detectionColour,
            });

            setTimeout(() => {
              this.setState({
                backgroundColour: this._defaultColour,
              });
            }, 1000);
          }
        },
        (error) => {
          this._stopProcessing();
          this.setState({
            isError: true,
            errorMessage: error.message,
          });
        },
        '',
        [0.5],
      );
    } catch (err) {
      let errorMessage = '';
      if (err instanceof PorcupineErrors.PorcupineInvalidArgumentError) {
        errorMessage = `${err.message}\nPlease make sure accessKey ${this._accessKey} is a valid access key.`;
      } else if (err instanceof PorcupineErrors.PorcupineActivationError) {
        errorMessage = 'AccessKey activation error';
      } else if (err instanceof PorcupineErrors.PorcupineActivationLimitError) {
        errorMessage = 'AccessKey reached its device limit';
      } else if (
        err instanceof PorcupineErrors.PorcupineActivationRefusedError
      ) {
        errorMessage = 'AccessKey refused';
      } else if (
        err instanceof PorcupineErrors.PorcupineActivationThrottledError
      ) {
        errorMessage = 'AccessKey has been throttled';
      } else {
        errorMessage = err.toString();
      }

      this.setState({
        isError: true,
        errorMessage: errorMessage,
      });
    }
  }

  async _requestRecordAudioPermission() {
    try {
      const granted = await PermissionsAndroid.request(
        PermissionsAndroid.PERMISSIONS.RECORD_AUDIO,
        {
          title: 'Microphone Permission',
          message:
            'Porcupine needs access to your microphone to listen for wake words.',
          buttonNeutral: 'Ask Me Later',
          buttonNegative: 'Cancel',
          buttonPositive: 'OK',
        },
      );
      return granted === PermissionsAndroid.RESULTS.GRANTED;
    } catch (err) {
      this.setState({
        isError: true,
        errorMessage: err.toString(),
      });
      return false;
    }
  }

  render() {
    let keywordOptions = this.state.keywordOptions.map((s, i) => {
      return <Picker.Item key={i} value={s} label={s} />;
    });

    return (
      <View
        style={[
          styles.container,
          {backgroundColor: this.state.backgroundColour},
        ]}>
        <View style={styles.statusBar}>
          <Text style={styles.statusBarText}>Porcupine</Text>
        </View>
        <View style={styles.keyword}>
          <Text style={styles.keywordText}>Keyword</Text>
          <View style={styles.picker}>
            <Picker
              selectedValue={this.state.currentKeyword}
              mode="dropdown"
              style={{}}
              itemStyle={styles.itemStyle}
              onValueChange={(keyword) => this._loadNewKeyword(keyword)}>
              {keywordOptions}
            </Picker>
          </View>
        </View>

        <View style={styles.button}>
          <TouchableOpacity
            style={styles.buttonStyle}
            onPress={() => this._toggleListening()}
            disabled={this.state.buttonDisabled || this.state.isError}>
            <Text style={styles.buttonText}>{this.state.buttonText}</Text>
          </TouchableOpacity>
        </View>
        {this.state.isError && (
          <View style={styles.errorBox}>
            <Text style={styles.errorText}>{this.state.errorMessage}</Text>
          </View>
        )}
        <View style={styles.footerView}>
          <Text style={styles.instructions}>
            Made in Vancouver, Canada by Picovoice
          </Text>
        </View>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    flexDirection: 'column',
    justifyContent: 'center',
    backgroundColor: '#F5FCFF',
  },
  subContainer: {
    flex: 1,
    justifyContent: 'center',
  },
  statusBar: {
    flex: 0.4,
    backgroundColor: '#377DFF',
    justifyContent: 'flex-end',
  },
  statusBarText: {
    fontSize: 18,
    color: 'white',
    fontWeight: 'bold',
    marginLeft: 15,
    marginBottom: 15,
  },
  keyword: {
    flex: 1,
    paddingTop: '10%',
  },
  keywordText: {
    color: '#666666',
    marginLeft: 15,
    marginBottom: 5,
  },
  picker: {
    width: '90%',
    height: '40%',
    alignContent: 'center',
    justifyContent: 'center',
    alignSelf: 'center',
  },
  button: {
    flex: 1,
    justifyContent: 'center',
    alignContent: 'center',
  },
  buttonStyle: {
    width: '50%',
    height: '50%',
    alignSelf: 'center',
    justifyContent: 'center',
    backgroundColor: '#377DFF',
    borderRadius: 100,
  },
  buttonText: {
    fontSize: 30,
    fontWeight: 'bold',
    color: 'white',
    textAlign: 'center',
  },
  itemStyle: {
    fontWeight: 'bold',
    fontSize: 20,
    textAlign: 'center',
  },
  footerView: {
    flex: 1,
    justifyContent: 'flex-end',
    paddingBottom: 25,
  },
  instructions: {
    textAlign: 'center',
    color: '#666666',
  },
  errorBox: {
    backgroundColor: 'red',
    borderRadius: 5,
    margin: 20,
    padding: 20,
    textAlign: 'center',
  },
  errorText: {
    color: 'white',
    fontSize: 16,
  },
});
