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

  const { isListening, error, start, stop } = usePorcupine(
    accessKey,
    keywords,
    keywordEventHandler,
    {base64: porcupineParams}
  );

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
      <h3>Listening: {JSON.stringify(isListening)}</h3>
      <h3>Error: {JSON.stringify(error)}</h3>
      {error && accessKey && (
        <p className="error-message">{JSON.stringify(error)}</p>
      )}
      <h3>Keywords: {JSON.stringify(keywords)}</h3>
      <br />
      <button
        onClick={() => start()}
        disabled={isListening}
      >
        Start
      </button>
      <button
        onClick={() => stop()}
        disabled={error !== null || !isListening}
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
