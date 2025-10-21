import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";
import { fileURLToPath, URL } from "node:url";

/** @type {import('vite').UserConfig} */
export default defineConfig({
  plugins: [react()],
  resolve: {
    alias: {
      "@": fileURLToPath(new URL("./test", import.meta.url)),
    },
  },
  optimizeDeps: {
    include: ["@/porcupine_params.js"],
  },
});
