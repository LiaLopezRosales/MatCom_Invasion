# AGENTS.md — MatCom Invasion

Conventions and environment notes for working on this repository. Written for
both human developers and coding agents.

## 1. Project overview

**MatCom Invasion** is a modern, neon-styled re-imagining of a retro arcade
space shooter, rebuilt as a hybrid **C → WebAssembly + TypeScript/Vite** app.
The entire game engine runs in compiled C (ported to WASM via Emscripten), while
the UI is a TypeScript SPA rendered on a canvas with Web Audio for music/SFX.

It is a **portfolio project** showcasing a clean migration from a monolithic C
codebase into a maintainable, tested, multi-platform game, delivered as a
[GitHub Pages demo](https://lia.lopezrosales.github.io/MatCom_Invasion/).

## 2. Architecture

- **C engine (pure logic):** deterministic, single-threaded `game_t` in `src/`.
  Compiled natively with `gcc` for tests/CLI and to WASM with `emcc` for the web.
- **Web frontend:** TypeScript + Vite in `web/`, canvas rendering with neon
  tinting, HTML overlay HUD, Web Audio. WASM module bound via a small snapshot
  reader (`web/src/game.ts`).
- **Tests:** Criterion (C) unit + headless simulation runner in `tests/`,
  built with AddressSanitizer + UndefinedBehaviorSanitizer.

## 3. Repo structure

```
.
├── src/               # C game engine (game, scheduler, enemy, projectile, balance…)
├── tests/             # Criterion C tests + headless simulation runner
├── web/
│   ├── public/
│   │   ├── assets/    # CC0 Kenney sprites (see LICENSE.txt)
│   │   └── matcom_logic.{js,wasm}   # Emscripten build output (gitignored)
│   └── src/
│       ├── main.ts    # app entry, game loop, menus
│       ├── game.ts    # WASM binding + snapshot reader
│       ├── render.ts  # canvas rendering + neon tinting
│       ├── audio.ts   # Web Audio synthesized music + SFX
│       └── styles.css # neon theme
├── docs/planning/     # per-phase specs and ADRs
├── docs/portfolio/    # portfolio quality/agents/roadmap guides
├── .github/workflows/ # CI/CD pipeline
├── Makefile           # native / wasm / test / coverage targets
└── README.md
```

## 4. Setup

Prerequisites (installed in the repo's home, no sudo):

- **C toolchain** — `gcc`, `make`, POSIX.
- **Criterion** — dev-time test framework (`libcriterion-dev`).
- **Emscripten (`emsdk`)** — activated in `$HOME/emsdk`; `emcc` on `PATH`
  (`export PATH="$HOME/emsdk/upstream/emscripten:$PATH"`). Version **6.0.9**.
- **Node.js / npm** — for the web frontend (Node ≥ 20).

## 5. Build commands

Run from the repo root (or `web/` where noted):

```sh
make native         # build the native CLI binary (invasion_cli) — gcc semantics check
make wasm           # build WASM module -> web/public/matlogic_logic.{js,wasm} (emcc)
make test           # run Criterion tests (ASan/UBSan) — must stay green
make coverage       # gcov coverage report (optional)

cd web && npm install   # install frontend deps
cd web && npm run dev   # local dev server
cd web && npm run build # production build -> web/dist (used by CI/Pages)
```

Verification (`web/smoke.test.mjs`, Playwright) runs against a served build;
point it at any base URL with `BASE_URL`.

## 6. Coding conventions

- **Language:** English for code, docs and game UI.
- **Naming:** `snake_case` (C files/functions), `PascalCase`/`camelCase`
  (TS), `UPPER_SNAKE_CASE` (constants/macros).
- **C:** strict warnings as errors (`-Wall -Wextra -Werror`), C11, single-thread,
  deterministic (seeded) update loop, data-driven entities.
- **Commits:** [Conventional Commits](https://www.conventionalcommits.org/)
  (`feat:`, `fix:`, `docs:`, `chore:`, `test:`, `refactor:`), optionally scoped
  (`fix(parser):`). One concern per commit.
- **Do not** commit build artifacts (`*.o`, `*.wasm`, `dist/`, `node_modules/`),
  the compiled binary, or secrets.

## 7. CI/CD

GitHub Actions (`.github/workflows/deploy.yml`):

- **Triggers:** `push` to `main` (test + build + **deploy** to GitHub Pages) and
  `pull_request` (test + build only).
- **Pipeline:** checkout → Node (npm cache) → Emscripten (cached) → install
  `libcriterion-dev` → `make native` → `make test` (ASan/UBSan; a failure stops
  the pipeline) → `make wasm` → `cd web && npm install && npm run build` →
  upload `web/dist` artifact → `deploy-pages`.
- Vite builds with `base: './'` and assets are referenced with relative paths so
  the site works under the GitHub Pages subpath.
- CI badge lives at the top of `README.md`.

## 8. Key decisions

- [`docs/planning/ENVIRONMENT_DECISION.md`](docs/planning/ENVIRONMENT_DECISION.md) —
  why the hybrid C→WASM + TS/Vite architecture (no graphical deps, no COOP/COEP).
- [`docs/planning/GAME_DESIGN.md`](docs/planning/GAME_DESIGN.md) — engine design and
  scope (phase 1 vs future work).
- [`docs/planning/PHASE0_ENVIRONMENT.md`](docs/planning/PHASE0_ENVIRONMENT.md) —
  exact toolchain versions and validation.
- [`docs/planning/FASE2_TESTS.md`](docs/planning/FASE2_TESTS.md) — test suite and
  coverage.
- [`docs/planning/FASE4_FEATURES.md`](docs/planning/FASE4_FEATURES.md) and
  [`docs/planning/FASE4_DESIGN.md`](docs/planning/FASE4_DESIGN.md) — game content /
  art / UI / audio spec.
- [`docs/planning/FASE5_FRONTEND.md`](docs/planning/FASE5_FRONTEND.md) — frontend
  implementation and README.
- [`docs/planning/FASE6_CI.md`](docs/planning/FASE6_CI.md) — CI/CD pipeline detail.
- [`docs/planning/FASE7_GOVERNANCE.md`](docs/planning/FASE7_GOVERNANCE.md) —
  governance, cleanup and final DoD verification.
