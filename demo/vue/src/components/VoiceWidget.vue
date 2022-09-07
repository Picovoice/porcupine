<template>
  <div class="voice-widget">
    <h2>VoiceWidget</h2>
    <h3>
      <label>
        AccessKey obtained from
        <a href="https://console.picovoice.ai/">Picovoice Console</a>:
        <input type="text" name="accessKey" v-on:change="updateAccessKey" />
      </label>
      <button class="start-button" v-on:click="initEngine">
        Start Porcupine
      </button>
    </h3>
    <h3>Loaded: {{ porcupine.isLoaded }}</h3>
<!--    <h3>Listening: {{ isListening }}</h3>-->
<!--    <h3>Error: {{ error !== null }}</h3>-->
<!--    <p class="error-message" v-if="error !== null">-->
<!--      {{ error }}-->
<!--    </p>-->
<!--    <button v-on:click="start" :disabled="!isLoaded || error || isListening">-->
<!--      Start-->
<!--    </button>-->
<!--    <button v-on:click="stop" :disabled="!isLoaded || error || !isListening">-->
<!--      Stop-->
<!--    </button>-->
<!--    <button v-on:click="release" :disabled="!isLoaded || error">Release</button>-->
    <h3>Keyword Detections (Listening for "Grasshopper" and "Grapefruit"):</h3>
    <ul v-if="detections.length > 0">
      <li v-for="(item, index) in detections" :key="index">
        {{ item }}
      </li>
    </ul>
  </div>
</template>

<script lang="ts">
import Vue from "vue";

import { BuiltInKeyword, PorcupineDetection } from "@picovoice/porcupine-web";
import { usePorcupine } from "@picovoice/porcupine-vue";

// @ts-ignore
import porcupineParams from "@/lib/porcupine_params";

const VoiceWidget = Vue.extend({
  name: "VoiceWidget",
  data() {
    return {
      accessKey: "",
      detections: [] as string[],
      porcupine: null
    };
  },
  beforeMount() {
    const porcupine = usePorcupine();
    // @ts-ignore
    Vue.set(this, "porcupine", porcupine)
    console.log(porcupine)
  },
  watch: {
    keywordDetection(porcupineDetection: PorcupineDetection | null) {
      if (porcupineDetection !== null) {
        this.detections.push(porcupineDetection.label);
      }
    },
  },
  methods: {
    initEngine: async function () {
      console.log(this);
      // @ts-ignore
      await this.porcupine.init(
        this.accessKey,
        [BuiltInKeyword.Grasshopper, BuiltInKeyword.Grapefruit],
        { base64: porcupineParams }
      );
      console.log("after init");
    },
    updateAccessKey: function (event: any) {
      this.accessKey = event.target.value;
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
