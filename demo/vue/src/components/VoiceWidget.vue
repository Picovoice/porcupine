<template>
  <div class="voice-widget">
    <Porcupine
      ref="porcupine"
      v-bind:porcupineFactoryArgs="factoryArgs"
      v-bind:porcupineFactory="factory"
      v-on:ppn-ready="ppnReadyFn"
      v-on:ppn-keyword="ppnKeywordFn"
      v-on:ppn-error="ppnErrorFn"
    />
    <h2>VoiceWidget</h2>
    <h3>
      <label>
        AccessKey obtained from
        <a href="https://picovoice.ai/console/">Picovoice Console</a>:
        <input
          type="text"
          name="accessKey"
          v-on:change="initEngine"
          :disabled="isLoaded"
        />
      </label>
    </h3>
    <h3>Loaded: {{ isLoaded }}</h3>
    <h3>Listening: {{ isListening }}</h3>
    <h3>Error: {{ isError }}</h3>
    <p class="error-message" v-if="isError">
      {{ JSON.stringify(errorMessage) }}
    </p>
    <button v-on:click="start" :disabled="!isLoaded || isError || isListening">
      Start
    </button>
    <button v-on:click="pause" :disabled="!isLoaded || isError || !isListening">
      Pause
    </button>
    <h3>Keyword Detections (Listening for "Grasshopper" and "Grapefruit"):</h3>
    <ul v-if="detections.length > 0">
      <li v-for="(item, index) in detections" :key="index">
        {{ item }}
      </li>
    </ul>
  </div>
</template>

<script>
import Porcupine from "@picovoice/porcupine-web-vue";
import { PorcupineWorkerFactory as PorcupineWorkerFactoryEn } from "@picovoice/porcupine-web-en-worker";

export default {
  name: "VoiceWidget",
  components: {
    Porcupine,
  },
  data: function () {
    return {
      detections: [],
      isError: false,
      isLoaded: false,
      isListening: false,
      factory: PorcupineWorkerFactoryEn,
      factoryArgs: {
        accessKey: "",
        keywords: [
          { builtin: "Grasshopper", sensitivity: 0.5 },
          { builtin: "Grapefruit", sensitivity: 0.6 },
        ],
      },
    };
  },
  methods: {
    start: function () {
      if (this.$refs.porcupine.start()) {
        this.isListening = !this.isListening;
      }
    },
    pause: function () {
      if (this.$refs.porcupine.pause()) {
        this.isListening = !this.isListening;
      }
    },
    initEngine: function (event) {
      this.factoryArgs.accessKey = event.target.value;
      this.isError = false;
      this.isLoaded = false;
      this.isListening = false;
      this.$refs.porcupine.initEngine();
    },
    ppnReadyFn: function () {
      this.isLoaded = true;
      this.isListening = true;
    },
    ppnKeywordFn: function (keywordLabel) {
      console.log(keywordLabel);
      this.detections = [...this.detections, keywordLabel];
    },
    ppnErrorFn: function (error) {
      this.isError = true;
      this.errorMessage = error.toString();
    },
  },
};
</script>

<style scoped>
button {
  padding: 1rem;
  font-size: 1.5rem;
  margin-right: 1rem;
}

.voice-widget {
  border: 2px double #377dff;
  padding: 2rem;
}

.error-message {
  background-color: maroon;
  color: white;
  padding: 1rem;
  border-left: 5px solid red;
  font-family: monospace;
  font-weight: bold;
  font-size: 1.5rem;
}
</style>
