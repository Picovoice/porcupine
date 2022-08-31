<template>
  <div class="voice-widget">
    <h2>VoiceWidget</h2>
    <h3>
      <label>
        AccessKey obtained from
        <a href="https://console.picovoice.ai/">Picovoice Console</a>:
        <input type="text" name="accessKey" v-on:change="updateInputValue" />
      </label>
      <button class="start-button" v-on:click="initEngine">
        Start Porcupine
      </button>
    </h3>
    <h3>Loaded: {{ isLoaded }}</h3>
    <h3>Listening: {{ isListening }}</h3>
    <h3>Error: {{ error !== null }}</h3>
    <p class="error-message" v-if="error !== null">
      {{ error.toString() }}
    </p>
    <button v-on:click="start" :disabled="!isLoaded || error || isListening">
      Start
    </button>
    <button v-on:click="stop" :disabled="!isLoaded || error || !isListening">
      Stop
    </button>
    <button v-on:click="release" :disabled="!isLoaded || error">
      Release
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
import { defineComponent } from "vue";

import { BuiltInKeyword, PorcupineDetection } from "@picovoice/porcupine-web";
import porcupineMixin, { PorcupineVue } from "@picovoice/porcupine-vue";

// @ts-ignore
import porcupineParams from "@/lib/porcupine_params";

const VoiceWidget = defineComponent({
  name: "VoiceWidget",
  mixins: [porcupineMixin],
  data() {
    return {
      inputValue: "",
      detections: [] as string[],
      isLoaded: false,
      isListening: false,
      error: null as string | null,
      $porcupine: {} as PorcupineVue,
    };
  },
  methods: {
    start: function () {
      this.$porcupine.start();
    },
    stop: function () {
      this.$porcupine.stop();
    },
    release: function () {
      this.$porcupine.release();
    },
    initEngine: function () {
      this.$porcupine.init(
        this.inputValue,
        [BuiltInKeyword.Grasshopper, BuiltInKeyword.Grapefruit],
        this.keywordDetectionCallback,
        { base64: porcupineParams },
        this.isLoadedCallback,
        this.isListeningCallback,
        this.errorCallback
      );
    },
    updateInputValue: function (event: any) {
      this.inputValue = event.target.value;
    },
    keywordDetectionCallback: function (
      porcupineDetection: PorcupineDetection
    ) {
      this.detections = [...this.detections, porcupineDetection.label];
    },
    isLoadedCallback: function (isLoaded: boolean) {
      this.isLoaded = isLoaded;
    },
    isListeningCallback: function (isListening: boolean) {
      this.isListening = isListening;
    },
    errorCallback: function (error: string | null) {
      this.error = error;
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

.start-button {
  padding: 0.1rem;
  font-size: 1rem;
  margin-left: 0.5rem;
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
