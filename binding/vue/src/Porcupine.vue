<template>
  <div>
    <slot />
  </div>
</template>

<script lang="ts">
import { PropType } from 'vue';

import { WebVoiceProcessor } from '@picovoice/web-voice-processor';

import {
  PorcupineWorkerFactoryArgs, 
  PorcupineWorkerFactory
} from './porcupine_types';

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
    porcupineFactoryArgs: {
      type: Object as PropType<PorcupineWorkerFactoryArgs>,
      required: true,
    },
    porcupineFactory: {
      type: Function as PropType<PorcupineWorkerFactory>,
      required: true,
    },
  },
  data() {
    return {
      webVp: null as WebVoiceProcessor | null,
      ppnWorker: null as Worker | null,
    };
  },
  methods: {
    /**
     * Method to start processing audio.
     */
    start(this: Vue) {
      if (this.webVp !== null) {
        this.webVp.start();
        return true;
      }
      return false;
    },
    /**
     * Method to stop processing audio.
     */
    pause(this: Vue) {
      if (this.webVp !== null) {
        this.webVp.pause();
        return true;
      }
      return false;
    },
    /**
     * Method to initialize PorcupineWorker.
     */
    async initEngine(this: Vue) {
      try {
        const { accessKey, keywords } = this.porcupineFactoryArgs;
        this.ppnWorker = await this.porcupineFactory.create(
          accessKey,
          JSON.parse(JSON.stringify(keywords)),
          (label: string) => {
            this.$emit('ppn-keyword', label);
          },
          (error: Error) => {
            this.$emit('ppn-error', error);
          },
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
  // Vue 2 release resources
  beforeDestroy: function (this: Vue) {
    if (this.webVp !== null) {
      this.webVp.release();
    }
    if (this.ppnWorker !== null) {
      this.ppnWorker.postMessage({ command: 'release' });
    }
  },
  // Vue 3 release resources
  beforeUnmount: function(this: Vue) {
    if (this.webVp !== null) {
      this.webVp.release();
    }
    if (this.ppnWorker !== null) {
      this.ppnWorker.postMessage({ command: 'release' });
    }
  },
};
</script>
