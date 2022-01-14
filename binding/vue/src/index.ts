import porcupineMixin, { PorcupineVue, PorcupineWorkerFactoryArgs } from './porcupine';

// Create module definition for Vue.use()
const plugin = {
  install: function(Vue: any) {
    Vue.mixin(porcupineMixin);
  }
};

// // Auto-install when vue is found (eg. in browser via <script> tag)
let GlobalVue = null;
if (typeof window !== 'undefined') {
  // @ts-ignore
  GlobalVue = window.Vue;
// @ts-ignore
} else if (typeof global !== 'undefined') {
  // @ts-ignore
  GlobalVue = global.Vue;
}
if (GlobalVue) {
  GlobalVue.use(plugin);
}

// To allow use as module (npm/webpack/etc.) export component
export default porcupineMixin;

// export types
export { PorcupineVue, PorcupineWorkerFactoryArgs };