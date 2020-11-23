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

import React, {Component} from 'react';
import {PermissionsAndroid, Platform, TouchableOpacity} from 'react-native';
import {StyleSheet, Text, View} from 'react-native';
import {PorcupineManager} from '@picovoice/porcupine-react-native';
import {Picker} from '@react-native-picker/picker';

type Props = {};
type State = {
  currentKeyword: string;
  keywordOptions: string[];
  buttonText: string;
  buttonDisabled: boolean;
  isListening: boolean;
  backgroundColour: string;
};

export default class App extends Component<Props, State> {
  _porcupineManager: PorcupineManager | undefined;
  _detectionColour: string = '#00E5C3';
  _defaultColour: string = '#F5FCFF';

  constructor(props: Props) {
    super(props);
    let keywordOptions = PorcupineManager.KEYWORDS;
    this.state = {
      keywordOptions: keywordOptions,
      currentKeyword: keywordOptions[0],
      buttonText: 'Start',
      buttonDisabled: false,
      isListening: false,
      backgroundColour: this._defaultColour,
    };
  }

  async componentDidMount() {
    this._loadNewKeyword(this.state.currentKeyword as string);
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
    if (Platform.OS == 'android') {
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
    if (this.state.isListening) this._stopProcessing();
    else this._startProcessing();
  }

  async _loadNewKeyword(keyword: string) {
    if (this.state.isListening) {
      this._stopProcessing();
    }
    this._porcupineManager?.delete();

    this.setState({currentKeyword: keyword});
    try {
      this._porcupineManager = await PorcupineManager.fromKeywords(
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
      );
    } catch (err) {
      console.error(err);
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
      console.error(err);
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
        <View style={{flex: 1, paddingTop: '10%'}}>
          <Text style={{color: '#666666', marginLeft: 15, marginBottom: 5}}>
            Keyword
          </Text>
          <View
            style={{
              width: '90%',
              height: '40%',
              alignContent: 'center',
              justifyContent: 'center',
              alignSelf: 'center',
            }}>
            <Picker
              selectedValue={this.state.currentKeyword}
              mode="dropdown"
              style={{}}
              itemStyle={styles.itemStyle}
              onValueChange={(keyword) =>
                this._loadNewKeyword(keyword as string)
              }>
              {keywordOptions}
            </Picker>
          </View>
        </View>

        <View
          style={{flex: 1, justifyContent: 'center', alignContent: 'center'}}>
          <TouchableOpacity
            style={{
              width: '50%',
              height: '50%',
              alignSelf: 'center',
              justifyContent: 'center',
              backgroundColor: '#377DFF',
              borderRadius: 100,
            }}
            onPress={() => this._toggleListening()}
            disabled={this.state.buttonDisabled}>
            <Text style={styles.buttonText}>{this.state.buttonText}</Text>
          </TouchableOpacity>
        </View>
        <View style={{flex: 1, justifyContent: 'flex-end', paddingBottom: 25}}>
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
  buttonStyle: {
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
  instructions: {
    textAlign: 'center',
    color: '#666666',
  },
});
