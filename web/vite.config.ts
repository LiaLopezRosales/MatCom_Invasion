import { defineConfig } from "vite";

// `base: './'` keeps built asset URLs relative so the site loads correctly
// when deployed to a GitHub Pages subpath (e.g. <user>.github.io/<repo>/).
export default defineConfig({
  base: "./",
  build: {
    outDir: "dist",
  },
});
