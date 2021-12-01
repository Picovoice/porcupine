<template>
  <div>
    <slot />
  </div>
</template>

<script>
import { WebVoiceProcessor } from '@picovoice/web-voice-processor';

/**
 * Porcupine Vue Component.
 * 
 * Props
 * - porcupineFactoryArgs: Arguments for PorcupineWorkerFactory.
 * - PorcupineWorkerFactory: The language-specific worker factory.
 * 
 * Events
 * - ppn-ready: A method invoked after component has initialized.
 * - ppn-keyword: A method invoked upon detection of the keywords.
 * - ppn-error: A method invoked if an error occurs within `PorcupineWorkerFactory`.
 */
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
    /**
     * Method to start processing audio.
     */
    start() {
      if (this.webVp !== null) {
        this.webVp.start();
        return true;
      }
      return false;
    },
    /**
     * Method to stop processing audio.
     */
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
    /**
     * Method to initialize PorcupineWorker.
     */
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
