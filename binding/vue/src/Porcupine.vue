<template>
  <div>
    <slot />
  </div>
</template>

<script>
import { WebVoiceProcessor } from '@picovoice/web-voice-processor';

export default {
  name: 'Porcupine',
  props: {
    porcupineFactoryArgs: [Object],
    porcupineFactory: [Function],
  },
  data: function () {
    return { webVp: null, ppnWorker: null };
  },
  methods: {
    start() {
      if (this.webVp !== null) {
        this.webVp.start();
        return true;
      }
      return false;
    },
    pause() {
      if (this.webVp !== null) {
        this.webVp.pause();
        return true;
      }
      return false;
    },
    keywordCallback(label) {
      this.$emit('ppn-keyword', label);
    },
    errorCallback(error) {
      this.$emit('ppn-error', error);
    },
    async initEngine() {
      try {
        const { accessKey, keywords } = this.porcupineFactoryArgs;
        this.ppnWorker = await this.porcupineFactory.create(
          accessKey,
          JSON.parse(JSON.stringify(keywords)),
          this.keywordCallback,
          this.errorCallback
        );
        this.webVp = await WebVoiceProcessor.init({
          engines: [this.ppnWorker],
        });
        this.$emit('ppn-ready');
      } catch (error) {
        this.$emit('ppn-error', error);
      }
    },
  },
  beforeUnmount: function () {
    if (this.webVp !== null) {
      this.webVp.release();
    }
    if (this.ppnWorker !== null) {
      this.ppnWorker.postMessage({ command: 'release' });
    }
  },
};
</script>
