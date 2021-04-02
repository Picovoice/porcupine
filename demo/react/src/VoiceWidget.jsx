import { useEffect, useState } from "react";
import { usePorcupine } from "@picovoice/porcupine-web-react";

export default function VoiceWidget() {
  const [keywordDetections, setKeywordDetections] = useState([]);
  const [workerChunk, setWorkerChunk] = useState({ factory: null });
  const [keywords] = useState([
    { builtin: "Alexa", sensitivity: 0.7 },
    "Picovoice",
  ]);

  useEffect(() => {
    async function loadPorcupine() {
      // Dynamically import the worker
      const ppnEnFactory = (await import("@picovoice/porcupine-web-en-worker"))
        .PorcupineWorkerFactory;
      console.log("Porcupine worker (EN) chunk is loaded.");
      return ppnEnFactory;
    }

    if (workerChunk.factory === null) {
      loadPorcupine().then((workerFactory) => {
        setWorkerChunk({ factory: workerFactory });
      });
    }
  }, [workerChunk]);

  const keywordEventHandler = (porcupineKeywordLabel) => {
    setKeywordDetections((x) => [...x, porcupineKeywordLabel]);
  };

  const {
    isLoaded,
    isListening,
    isError,
    errorMessage,
    start,
    resume,
    pause,
  } = usePorcupine(
    workerChunk.factory,
    { keywords, start: true },
    keywordEventHandler
  );

  return (
    <div className="voice-widget">
      <h2>VoiceWidget</h2>
      <h3>Loaded: {JSON.stringify(isLoaded)}</h3>
      <h3>Listening: {JSON.stringify(isListening)}</h3>
      <h3>Error: {JSON.stringify(isError)}</h3>
      {isError && (
        <p className="error-message">{JSON.stringify(errorMessage)}</p>
      )}
      <h3>Keywords: {JSON.stringify(keywords)}</h3>
      <br />
      <button
        onClick={() => start()}
        disabled={isError || isListening || !isLoaded}
      >
        Start
      </button>
      <button
        onClick={() => pause()}
        disabled={isError || !isListening || !isLoaded}
      >
        Pause
      </button>
      <button
        onClick={() => resume()}
        disabled={isError || isListening || !isLoaded}
      >
        Resume
      </button>
      <h3>Keyword Detections:</h3>
      {keywordDetections.length > 0 && (
        <ul>
          {keywordDetections.map((label, index) => (
            <li key={index}>{label}</li>
          ))}
        </ul>
      )}
    </div>
  );
}
