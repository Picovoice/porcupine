import {useEffect, useState} from "react";

import {BuiltInKeyword, PorcupineDetection} from "@picovoice/porcupine-web";
import {usePorcupine} from "@picovoice/porcupine-web-react";

import porcupineParams from "./lib/porcupine_params";

export default function VoiceWidget() {
  const [keywordDetections, setKeywordDetections] = useState<string[]>([]);
  const [accessKey, setAccessKey] = useState("");
  const [keywords] = useState<Array<BuiltInKeyword>>([
    BuiltInKeyword.Alexa,
    BuiltInKeyword.Picovoice
  ]);

  const keywordEventHandler = (porcupineDetection: PorcupineDetection) => {
    setKeywordDetections((x) => [...x, porcupineDetection.label]);
  };

  const { isLoaded, isListening, error, init, start, stop, release } = usePorcupine();

  useEffect(() => {
    if (accessKey.length > 0) {
      init(
        accessKey,
        keywords,
        keywordEventHandler,
        {base64: porcupineParams}
      );
    }
  }, [init, accessKey, keywords])

  useEffect(() => {
    return () => {
      stop();
    }
  }, [stop]);

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
        </label>
      </h3>
      <h3>Loaded: {JSON.stringify(isLoaded)}</h3>
      <h3>Listening: {JSON.stringify(isListening)}</h3>
      <h3>Error: {JSON.stringify(error !== null)}</h3>
      {error && (
        <p className="error-message">{JSON.stringify(error)}</p>
      )}
      <h3>Keywords: {JSON.stringify(keywords)}</h3>
      <br />
      <button
        onClick={() => start()}
        disabled={!isLoaded || isListening}
      >
        Start
      </button>
      <button
        onClick={() => stop()}
        disabled={!isLoaded || !isListening}
      >
        Stop
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
