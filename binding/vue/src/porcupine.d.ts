import Vue from 'vue';

import { WebVoiceProcessor } from "@picovoice/web-voice-processor";
import { PorcupineWorkerFactoryArgs, PorcupineWorkerFactory } from "./porcupine_types";

declare module 'vue/types/vue' {
  // types so 'this' contains necessary variables in component method
  interface Vue {
    webVp: WebVoiceProcessor | null;
    ppnWorker: Worker;
    porcupineFactoryArgs: PorcupineWorkerFactoryArgs;
    porcupineFactory: PorcupineWorkerFactory;
  }
}

declare module 'vue/types/options' {
  // add options so that either Vue 2 or Vue 3 can compile.
  interface ComponentOptions<V extends Vue> {
    beforeUnmount: (this: Vue, ...args: any[]) => any;
    beforeDestroy: (this: Vue, ...args: any[]) => any;
  }
}
