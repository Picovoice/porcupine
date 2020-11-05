

import React, { Component} from 'react';
import { Button } from 'react-native';
import { StyleSheet, Text, View } from 'react-native';
import Porcupine from 'react-native-porcupine';

type Props = {};
type State = {
  frameLength:number | undefined;
  sampleRate:number | undefined;
  version:string | undefined;

  error:string|null;
};

export default class App extends Component<Props, State>{  
  
  porcupine:Porcupine;    
  porcupine2:Porcupine;    
  porcupine3:Porcupine;    
  constructor(props:Props){
    super(props);
    
    this.state = {
      frameLength: undefined,
      sampleRate: undefined,
      version: undefined,

      error: null
    };

  }

  async componentDidMount() {        
    
  }

  _chunkArray(array:number[], size:number) {
    return Array.from({ length: Math.ceil(array.length / size) }, (_, index) =>
      array.slice(index * size, index * size + size)
    );
  }

  async _startProcessing(){        
  
    try{
      this.porcupine = await Porcupine.fromKeywords(["porcupine"]);    
      this.porcupine2 = await Porcupine.fromKeywords(["picovoice"]);    
      this.porcupine3 = await Porcupine.fromKeywords(["bumblebee"]);    
      this.setState({
        frameLength: this.porcupine.frameLength,
        sampleRate: this.porcupine.sampleRate,
        version: this.porcupine.version,
      });
    }catch(e){
      console.error(e);
    }

    let frames = this._chunkArray(audio, this.porcupine.frameLength);
    if (frames[frames.length - 1].length !== this.porcupine.frameLength) {
      frames.pop();
    }
    
    for (let i = 0; i < frames.length; i++) {
      const frame = frames[i];
      try{
        let result = await this.porcupine.process(frame);  
        let result2 = await this.porcupine2.process(frame);  
        let result3 = await this.porcupine3.process(frame);  
        if(result == 0){                    
          console.log("Porcupine Detected!");
        }
        if(result2 == 0){                    
          console.log("Picovoice Detected!");
        }
        if(result3 == 0){                    
          console.log("Bumblebee Detected!");
        }
      }
      catch(e){
        console.log(e);
      }
    }  
        
    this.porcupine.release();      
    this.porcupine2.release();  
    this.porcupine3.release();            
  }

  render() {
    return (
      <View style={styles.container}>
        <Text style={styles.welcome}>☆porc w props☆</Text>
        <Text style={styles.instructions}>FL: {this.state.frameLength}</Text>
        <Text style={styles.instructions}>SR: {this.state.sampleRate}</Text>
        <Text style={styles.instructions}>VER: {this.state.version}</Text>
        <View>
          <Button title="Start" onPress={() => this._startProcessing()} />
        </View>        
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#F5FCFF',
  },
  welcome: {
    fontSize: 20,
    textAlign: 'center',
    margin: 10,
  },
  instructions: {
    textAlign: 'center',
    color: '#333333',
    marginBottom: 5,
  },
});

