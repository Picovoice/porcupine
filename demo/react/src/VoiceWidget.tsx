import {useEffect, useState} from "react";

import {BuiltInKeyword} from "@picovoice/porcupine-web";
import {usePorcupine} from "@picovoice/porcupine-react";

import porcupineParams from "./lib/porcupine_params";

export default function VoiceWidget() {
  const [keywordDetections, setKeywordDetections] = useState<string[]>([]);
  const [accessKey, setAccessKey] = useState("");
  const [keywords] = useState<Array<BuiltInKeyword>>([
    BuiltInKeyword.Alexa,
    BuiltInKeyword.Picovoice
  ]);

  const {
    keywordDetection,
    isLoaded,
    isListening,
    error,
    init,
    start,
    stop,
    release
  } = usePorcupine();

  const initEngine = async () => {
    await init(
      accessKey,
      keywords,
      {base64: porcupineParams}
    );
  }

  useEffect(() => {
    if (keywordDetection !== null) {
      setKeywordDetections((oldVal) => [...oldVal, keywordDetection.label])
    }
  }, [keywordDetection])

  return (
    <div className="voice-widget">
      <h2>VoiceWidget</h2>
      <h3>
        <label>
          AccessKey obtained from{" "}
          <a href="https://console.picovoice.ai/">Picovoice Console</a>:
          <input
            type="text"
            name="accessKey"
            onChange={(value) => setAccessKey(value.target.value)}
          />
          <button
            className="init-button"
            onClick={() => initEngine()}
          >
            Init Porcupine
          </button>
        </label>
      </h3>
      <h3>Loaded: {JSON.stringify(isLoaded)}</h3>
      <h3>Listening: {JSON.stringify(isListening)}</h3>
      <h3>Error: {JSON.stringify(error !== null)}</h3>
      {error && (
        <p className="error-message">{JSON.stringify(error)}</p>
      )}
      <h3>Keywords: {JSON.stringify(keywords)}</h3>
      <br/>
      <button
        onClick={() => start()}
        disabled={error !== null || !isLoaded || isListening}
      >
        Start
      </button>
      <button
        onClick={() => stop()}
        disabled={error !== null ||!isLoaded || !isListening}
      >
        Stop
      </button>
      <button
        onClick={() => release()}
        disabled={error !== null || !isLoaded}
      >
        Release
      </button>
      <h3>Keyword Detections (listening for "Picovoice" and "Alexa"):</h3>
      {keywordDetections.length > 0 && (
        <ul>
          {keywordDetections.map((label: string, index: number) => (
            <li key={index}>{label}</li>
          ))}
        </ul>
      )}
    </div>
  );
}
