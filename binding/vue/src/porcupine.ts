import { WebVoiceProcessor } from '@picovoice/web-voice-processor';
import { PorcupineVue } from './porcupine_types';

export default {
  computed: {
    $porcupine(): PorcupineVue {
      return {
        $_ppnWorker_: null as Worker | null,
        $_webVp_: null as WebVoiceProcessor | null,
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
        start() {
          if (this.$_webVp_ !== null) {
            this.$_webVp_.start();
            return true;
          }
          return false;
        },
        pause() {
          if (this.$_webVp_ !== null) {
            this.$_webVp_.pause();
            return true;
          }
          return false;
        },
        delete() {
          this.$_webVp_?.release();
          this.$_ppnWorker_?.postMessage({ command: 'release' });
        }
      }
    }
  },
  beforeUnmount(this: any) {
    this.$porcupine.delete();
  },
  beforeDestory(this: any) {
    this.$porcupine.delete();
  }
};
