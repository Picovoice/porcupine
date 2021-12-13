import { WebVoiceProcessor } from '@picovoice/web-voice-processor';
import { PorcupineVue } from './porcupine_types';

export default {
  computed: {
    /**
     * Porcupine Vue Mixin.
     */
    $porcupine(): PorcupineVue {
      return {
        $_ppnWorker_: null as Worker | null,
        $_webVp_: null as WebVoiceProcessor | null,
        /**
         * Init function for Porcupine.
         * 
         * @param porcupineFactoryArgs Arguments for PorcupineWorkerFactory.
         * @param porcupineFactory The language-specific worker factory
         * @param keywordCallback A method invoked upon detection of the keywords.
         * @param readyCallback A method invoked after component has initialized.
         * @param errorCallback A method invoked if an error occurs within `PorcupineWorkerFactory`.
         */
        async init(
          porcupineFactoryArgs,
          porcupineFactory,
          keywordCallback = (_: string) => {},
          readyCallback = () => {},
          errorCallback = (error: Error) => {console.error(error)}
        ) {
          try {
            const { accessKey, keywords } = porcupineFactoryArgs;
            this.$_ppnWorker_ = await porcupineFactory.create(
              accessKey,
              JSON.parse(JSON.stringify(keywords)),
              keywordCallback,
              errorCallback
            );
            this.$_webVp_ = await WebVoiceProcessor.init({
              engines: [this.$_ppnWorker_!],
            });
            readyCallback();
          } catch (error) {
            errorCallback(error as Error);
          }
        },
        /**
         * Start processing audio.
         */
        start() {
          if (this.$_webVp_ !== null) {
            this.$_webVp_.start();
            return true;
          }
          return false;
        },
        /**
         * Stop processing audio.
         */
        pause() {
          if (this.$_webVp_ !== null) {
            this.$_webVp_.pause();
            return true;
          }
          return false;
        },
        /**
         * Delete used resources.
         */
        delete() {
          this.$_webVp_?.release();
          this.$_ppnWorker_?.postMessage({ command: 'release' });
        }
      }
    }
  },
  // Vue 3 method to clean resources.
  beforeUnmount(this: any) {
    this.$porcupine.delete();
  },
  // Vue 2 method to clean resources.
  beforeDestory(this: any) {
    this.$porcupine.delete();
  }
};
