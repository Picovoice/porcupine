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
    <h3>Loaded: {{ state.isLoaded }}</h3>
    <h3>Listening: {{ state.isListening }}</h3>
    <h3>Error: {{ state.error !== null }}</h3>
    <h3>
      <label>Keyword: </label>
      <select v-on:change="updateSelectedKeyword">
        <option
          v-for="(keyword, index) in keywords"
          :key="keyword"
          :value="index"
        >
          {{ keyword }}
        </option>
      </select>
    </h3>
    <p class="error-message" v-if="state.error !== null">
      {{ state.error }}
    </p>
    <button
      v-on:click="start"
      :disabled="!state.isLoaded || !!state.error || state.isListening"
    >
      Start
    </button>
    <button
      v-on:click="stop"
      :disabled="!state.isLoaded || !!state.error || !state.isListening"
    >
      Stop
    </button>
    <button v-on:click="release" :disabled="!state.isLoaded || !!state.error">
      Release
    </button>
    <h3>Keyword Detections:</h3>
    <ul v-if="detections.length > 0">
      <li v-for="(item, index) in detections" :key="index">
        {{ item }}
      </li>
    </ul>
  </div>
</template>

<script lang="ts">
import { defineComponent, onBeforeUnmount, ref, watch } from "vue";

import { BuiltInKeyword } from "@picovoice/porcupine-web";
import { usePorcupine } from "@picovoice/porcupine-vue";

// @ts-ignore
import porcupineModel from "../lib/porcupineModel";

// @ts-ignore
import porcupineKeywords from "../lib/porcupineKeywords";

if (
  porcupineKeywords.length === 0 &&
  porcupineModel.publicPath.endsWith("porcupine_params.pv")
) {
  for (const k in BuiltInKeyword) {
    // @ts-ignore
    porcupineKeywords.push({ builtin: BuiltInKeyword[k] });
  }
}

const VoiceWidget = defineComponent({
  name: "VoiceWidget",
  setup() {
    const porcupine = usePorcupine();
    const accessKey = ref("");
    const detections = ref<string[]>([]);
    const keywords = ref(
      porcupineKeywords.map((k: any) => k.label ?? k.builtin)
    );
    const selectedKeywordIndex = ref(0);
    watch(
      () => porcupine.state.keywordDetection,
      keyword => {
        if (keyword !== null) {
          detections.value.push(keyword.label);
        }
      }
    );

    async function initEngine() {
      if (porcupine.state.isLoaded) {
        await porcupine.release();
      }
      await porcupine.init(
        accessKey.value,
        porcupineKeywords[selectedKeywordIndex.value],
        porcupineModel
      );
    }

    function updateAccessKey(event: any) {
      accessKey.value = event.target.value;
    }

    function updateSelectedKeyword(event: any) {
      selectedKeywordIndex.value = parseInt(event.target.value);
      if (porcupine.state.isLoaded) {
        initEngine();
      }
    }

    onBeforeUnmount(() => {
      porcupine.release();
    });

    return {
      accessKey,
      detections,
      keywords,
      initEngine,
      updateAccessKey,
      updateSelectedKeyword,
      ...porcupine,
    };
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
