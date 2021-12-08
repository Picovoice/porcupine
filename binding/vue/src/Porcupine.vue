<template>
  <div>
    <slot />
  </div>
</template>

<script lang="ts">
import Vue, { PropType } from 'vue';

import { WebVoiceProcessor } from '@picovoice/web-voice-processor';

/**
 * Type alias for the Porcupine keywords.
 */
export type PorcupineKeyword = PorcupineKeywordCustom | PorcupineKeywordBuiltin;

/**
 * Type alias for builtin keywords.
 */
export type PorcupineKeywordBuiltin = {
  builtin: string;
  sensitivity?: number;
};

/**
 * Type alias for custom keywords.
 */
export type PorcupineKeywordCustom = {
  base64: string;
  custom: string;
  sensitivity?: number;
};

/**
 * Type alias for PorcupineWorkerFactory arguments.
 */
export type PorcupineWorkerFactoryArgs = {
  accessKey: string;
  keywords: Array<PorcupineKeyword | string> | PorcupineKeyword | string;
  start?: boolean;
};

/**
 * The language-specific worker factory, imported as { PorcupineWorkerFactory } from the 
 * @picovoice/porcupine-web-xx-worker series of packages, where xx is the two-letter language code.
 */
interface PorcupineWorkerFactory extends FunctionConstructor {
  create: (
    accessKey: String, 
    keywords: Array<PorcupineKeyword | string> | PorcupineKeyword | string,
    keywordDetectionCallback?: CallableFunction,
    processErrorCallback?: CallableFunction,
    start?: boolean) => Promise<Worker>,
};

/**
 * Function interface for Porcupine Vue Component.
 */
interface FunctionArgs extends Vue {
  webVp: WebVoiceProcessor | null,
  ppnWorker: Worker | null,
  porcupineFactoryArgs: PorcupineWorkerFactoryArgs,
  porcupineFactory: PorcupineWorkerFactory,
};

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
    start(this: FunctionArgs) {
      if (this.webVp !== null) {
        this.webVp.start();
        return true;
      }
      return false;
    },
    /**
     * Method to stop processing audio.
     */
    pause(this: FunctionArgs) {
      if (this.webVp !== null) {
        this.webVp.pause();
        return true;
      }
      return false;
    },
    /**
     * Method to initialize PorcupineWorker.
     */
    async initEngine(this: FunctionArgs) {
      try {
        const { accessKey, keywords } = this.porcupineFactoryArgs;
        this['ppnWorker'] = await this.porcupineFactory.create(
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
  beforeDestroy: function (this: FunctionArgs) {
    if (this.webVp !== null) {
      this.webVp.release();
    }
    if (this.ppnWorker !== null) {
      this.ppnWorker.postMessage({ command: 'release' });
    }
  },
  // Vue 3 release resources
  beforeUnmount: function(this: FunctionArgs) {
    if (this.webVp !== null) {
      this.webVp.release();
    }
    if (this.ppnWorker !== null) {
      this.ppnWorker.postMessage({ command: 'release' });
    }
  },
};
</script>
