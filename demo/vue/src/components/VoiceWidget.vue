<template>
  <div class="voice-widget">
    <h2>VoiceWidget</h2>
    <h3>
      <label>
        AccessKey obtained from
        <a href="https://console.picovoice.ai/">Picovoice Console</a>:
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

<script lang="ts">
import Vue, { VueConstructor } from 'vue';

import { PorcupineWorkerFactory as PorcupineWorkerFactoryEn } from "@picovoice/porcupine-web-en-worker";
import porcupineMixin, { PorcupineVue } from "@picovoice/porcupine-web-vue";

const VoiceWidget = (Vue as VueConstructor<Vue & {$porcupine: PorcupineVue}>).extend({
  name: "VoiceWidget",
  mixins: [porcupineMixin],
  data() {
    return {
      detections: [] as string[],
      isError: false,
      errorMessage: null as string | null,
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
      if (this.$porcupine.start()) {
        this.isListening = !this.isListening;
      }
    },
    pause: function () {
      if (this.$porcupine.pause()) {
        this.isListening = !this.isListening;
      }
    },
    initEngine: function (event: any) {
      this.factoryArgs.accessKey = event.target.value;
      this.isError = false;
      this.isLoaded = false;
      this.isListening = false;
      this.$porcupine.init(
        this.factoryArgs,
        this.factory,
        this.ppnKeywordFn,
        this.ppnReadyFn,
        this.ppnErrorFn
      );
    }, 
    ppnReadyFn: function () {
      this.isLoaded = true;
      this.isListening = true;
    },
    ppnKeywordFn: function (keywordLabel: string) {
      console.log(keywordLabel);
      this.detections = [...this.detections, keywordLabel];
    },
    ppnErrorFn: function (error: string | Error) {
      this.isError = true;
      this.errorMessage = error.toString();
    },
  },
});

export default VoiceWidget;
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
