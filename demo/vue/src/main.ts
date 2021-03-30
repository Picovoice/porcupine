import { createApp } from "vue";
import App from "./App.vue";

const porcupineDemoApp = createApp(App)
porcupineDemoApp.component('VoiceWidget',
  () => import('./components/VoiceWidget.vue')
)
porcupineDemoApp.mount("#app");
