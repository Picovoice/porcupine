<template>
  <h1>Porcupine Web + Vue ("Porcupine" Renderless Component)</h1>
  <button v-on:click="toggle">
    Toggle VoiceWidget <span v-if="show">"OFF"</span><span v-else>"ON"</span>
  </button>
  <br />
  <br />
  <VoiceWidget v-if="show" />
</template>

<script lang="ts">
import { defineAsyncComponent, defineComponent } from "vue";
import VoiceWidget from "./components/VoiceWidget.vue";

export default defineComponent({
  name: "App",
  components: {
    VoiceWidget: defineAsyncComponent(
      () => import("./components/VoiceWidget.vue")
    ),
  },
  data: function () {
    return { show: true };
  },
  methods: {
    toggle: function () {
      this.show = !this.show;
    },
  },
});
</script>

<style>
body {
  padding: 2rem;
}

button {
  padding: 1rem;
  font-size: 1.5rem;
  margin-right: 1rem;
}
</style>
