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
          v-on:change="updateInputValue"
          :disabled="!$porcupine.isLoaded"
        />
      </label>
      <button class="start-button" v-on:click="initEngine" :disabled="!$porcupine.isLoaded">
          Start Porcupine
      </button>
    </h3>
    <h3>Loaded: {{ $porcupine.isLoaded }}</h3>
    <h3>Listening: {{ $porcupine.isListening }}</h3>
    <h3>Error: {{ $porcupine.error !== null }}</h3>
    <p class="error-message" v-if="$porcupine.error !== null">
      {{ $porcupine.error.toString() }}
    </p>
    <button v-on:click="start" :disabled="!$porcupine.isLoaded || $porcupine.error || $porcupine.isListening">
      Start
    </button>
    <button v-on:click="pause" :disabled="!$porcupine.isLoaded || $porcupine.error || !$porcupine.isListening">
      Pause
    </button>
    <button v-on:click="stop" :disabled="!$porcupine.isLoaded || $porcupine.error || !$porcupine.isListening">
      Stop
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

import porcupineMixin, { PorcupineVue } from "@picovoice/porcupine-web-vue";

const VoiceWidget = (
  Vue as VueConstructor<Vue & { $porcupine: PorcupineVue }>
).extend({
  name: "VoiceWidget",
  mixins: [porcupineMixin],
  data() {
    return {
      inputValue: "",
      detections: [] as string[],
      $porcupine: this.$porcupine,
    };
  },
  methods: {
    start: function () {
      this.$porcupine.start();
    },
    stop: function () {
      this.$porcupine.stop();
    },
    pause: function () {
      this.$porcupine.pause();
    },
    initEngine: function (event: any) {
      this.$porcupine.init(
        this.inputValue,
        [BuiltInKeyword.Grasshopper, BuiltInKeyword.Grapefruit],
        this.keywordDetectionCallback,
        {base64: porcupineParams}
      );
    },
    updateInputValue: function (event: any) {
      this.inputValue = event.target.value;
    },
    keywordDetectionCallback: function (keywordLabel: string) {
      this.detections = [...this.detections, keywordLabel];
    }
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
