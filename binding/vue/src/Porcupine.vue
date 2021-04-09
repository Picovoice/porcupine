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
    porcupineFactoryArgs: [Object, Array],
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
    resume() {
      if (this.webVp !== null) {
        this.webVp.resume();
        return true;
      }
      return false;
    },
  },
  async created() {
    this.$emit('ppn-loading');

    try {
      this.ppnWorker = await this.porcupineFactory.create(
        this.porcupineFactoryArgs
      );
      this.webVp = await WebVoiceProcessor.init({
        engines: [this.ppnWorker],
      });
      let _this = this;

      this.ppnWorker.onmessage = function (messageEvent) {
        switch (messageEvent.data.command) {
          case 'ppn-keyword':
            _this.$emit('ppn-keyword', messageEvent.data.keywordLabel);
            break;
        }
      };
    } catch (error) {
      this.$emit('ppn-error', error);
    }

    this.$emit('ppn-ready');
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
