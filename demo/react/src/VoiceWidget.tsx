import { useEffect, useState } from "react";
import { usePorcupine } from "@picovoice/porcupine-web-react";
import { PorcupineWorkerFactory } from "@picovoice/porcupine-web-core";

export default function VoiceWidget() {
  const [keywordDetections, setKeywordDetections] = useState<string[]>([]);
  const [workerChunk, setWorkerChunk] = useState<Record<string, PorcupineWorkerFactory | null>>({ factory: null });
  const [isChunkLoaded, setIsChunkLoaded] = useState(false);
  const [accessKey, setAccessKey] = useState("");
  const [keywords] = useState([
    { builtin: "Alexa", sensitivity: 0.7 },
    { builtin: "Picovoice", sensitivity: 0.8 },
  ]);

  useEffect(() => {
    if (workerChunk.factory === null) {
      let isCanceled = false;
      const loadPorcupine = async () => {
        // Dynamically import the worker
        const ppnEnFactory = (
          await import("@picovoice/porcupine-web-en-worker")
        ).PorcupineWorkerFactory;
        console.log("Porcupine worker (EN) chunk is loaded.");

        if (!isCanceled) {
          setWorkerChunk({ factory: ppnEnFactory });
          setIsChunkLoaded(true);
        }
      };

      loadPorcupine();

      return () => {
        isCanceled = true;
      };
    }
  }, [workerChunk]);

  const keywordEventHandler = (porcupineKeywordLabel: string) => {
    setKeywordDetections((x) => [...x, porcupineKeywordLabel]);
  };

  const { isLoaded, isListening, isError, errorMessage, start, pause } =
    usePorcupine(
      workerChunk.factory,
      { accessKey, keywords, start: true },
      keywordEventHandler
    );

  return (
    <div className="voice-widget">
      <h2>VoiceWidget</h2>
      <h3>
        <label>
          AccessKey obtained from{" "}
          <a href="https://picovoice.ai/console/">Picovoice Console</a>:
          <input
            type="text"
            name="accessKey"
            onChange={(value) => setAccessKey(value.target.value)}
            disabled={isLoaded}
          />
        </label>
      </h3>
      <h3>Dynamic Import Loaded: {JSON.stringify(isChunkLoaded)}</h3>
      <h3>Porcupine Loaded: {JSON.stringify(isLoaded)}</h3>
      <h3>Listening: {JSON.stringify(isListening)}</h3>
      <h3>Error: {JSON.stringify(isError)}</h3>
      {isError && accessKey && (
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
