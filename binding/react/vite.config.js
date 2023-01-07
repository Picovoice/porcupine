const { fileURLToPath, URL } = require("node:url");

const { defineConfig } = require('vite')

/** @type {import('vite').UserConfig} */
module.exports = defineConfig({
  resolve: {
    alias: {
      "@": fileURLToPath(new URL("./test", import.meta.url)),
    },
  },
  optimizeDeps: {
    include: ["@/porcupine_params.js"],
  },
});
