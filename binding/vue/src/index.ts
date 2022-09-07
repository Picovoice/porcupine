import Vue from 'vue';
import { usePorcupine } from './porcupine';

// if (Vue.version < "2.7") {
//   import("@vue/composition-api").then(compositionAPI => {
//     // Auto-install when vue is found (eg. in browser via <script> tag)
//     let GlobalVue = null;
//     if (typeof window !== 'undefined') {
//       // @ts-ignore
//       GlobalVue = window.Vue;
//       // @ts-ignore
//     } else if (typeof global !== 'undefined') {
//       // @ts-ignore
//       GlobalVue = global.Vue;
//     }
//     if (GlobalVue) {
//       // @ts-ignore
//       GlobalVue.use(compositionAPI);
//     } else {
//       // @ts-ignore
//       Vue.use(compositionAPI);
//     }
//   });
// }

export { usePorcupine };
