# Plan de pulido — MatCom Invasion (portafolio)

> Documento de trabajo. Registra el plan de elevación del proyecto a pieza de
> portafolio para la búsqueda de empleo. La numeración de fases = orden de
> ejecución. Se refina fase a fase.

## Contexto y objetivo

- Proyecto original: juego arcade en **C** (X11 + SDL2 + SDL2_image + SDL2_ttf),
  **multihilo** (`pthreads`).
- **Decisión de entorno**: migrar a **arquitectura híbrida** — engine `src/`
  (C puro) → **WASM** (Emscripten) + **UI web en TypeScript/Vite** (canvas con
  assets **Kenney CC0**). X11 y SDL2 se **eliminan**. Scheduler unificado a **single-thread**.
  Ver [`ENVIRONMENT_DECISION.md`](./ENVIRONMENT_DECISION.md).
- Sin build system, sin tests, sin CI, sin README en el árbol actual.
- `docs/portfolio/*.md` definen el estándar y el flujo de trabajo, pero **no están
  trackeados** en git.
- Sin experiencia laboral "real": el objetivo es que este repo brille como proyecto
  de portafolio para un puesto **junior** con orientación a **desarrollo web**,
  conservando **C** en el curriculum.

Tres puertas (gates) de un proyecto de portafolio fuerte:
1. Resuelve un problema real → es un juego jugable ✓, pero falta que sea **fácil de
   construir y ejecutar desde un clone limpio** (hoy no: falta build + README).
2. Se puede demostrar → una **demo web en el navegador** (GitHub Pages) reproducible.
3. Aporta algo más allá del tutorial → justificar el **scheduling** de enemigos
   (SJF/RR/Random), la separación lógica(C)/visual(web) y el port a WASM.

## Fase 0 — Entorno y reproducibilidad (prerequisito)

> Detalle completo y verificado en [`PHASE0_ENVIRONMENT.md`](./PHASE0_ENVIRONMENT.md),
> y decisión en [`ENVIRONMENT_DECISION.md`](./ENVIRONMENT_DECISION.md).

Resumen (entorno híbrido):
- Engine `src/` en **C puro** → build **WASM** (`emcc`) para web + **nativa**
  (`gcc`) para tests/CI. **Sin dependencias gráficas** (X11 y SDL2 eliminados).
- UI web en **TypeScript + Vite**.
- Ya están: `gcc`, `make`, `git`, `node` (v22), `npm`, `python3`.
- **Falta instalar Emscripten (emsdk)**: se instala en el **home**, **sin sudo**
  (`git clone` + `./emsdk install latest` + `source emsdk_env.sh`).
- La instalación es reproducible y NO requiere contraseña (a diferencia de los apt
  de SDL2 descartados).

## Fase 1 — Migración del engine (bloqueante)

> Decisiones de diseño del engine en [`GAME_DESIGN.md`](./GAME_DESIGN.md).

- **Reorganizar el repo a `src/` (C puro) + `web/` (TS/Vite) + `tests/` + Makefile.**
- **Refactorizar la lógica a C puro sin gráficas**: se eliminan X11, SDL2,
  `draw.c` y `xresources.c` (el render pasa a la web). Exponer una API C→WASM:
  `game_create(seed, ...)`, `game_set_mode`, `game_start`, `game_update(dt)`,
  `game_set_ship`, `game_fire`, `game_get_state`.
- **Engine `game_t` con instancia/puntero** (testeable; singleton en WASM) e
  `update(dt)` **determinista** (frame-rate independiente). **Semilla configurable**
  en `game_create` para reproducibilidad.
- **Scheduler single-thread y extensible**: dispatcher que permite agregar modos;
  conserva SJF/RR/Random; gobierna el **orden de aparición**; la dificultad
  procedural define cuántos/velocidad.
- **Niveles infinitos procedurales** (sin tope) como escenario + **condición de
  victoria configurable por modo** (`WinCondition`, desacoplada del scheduling).
  Máquina de estados: `MENU → PLAYING → PAUSED → GAME_OVER` y **`VICTORY`**.
- **Enemigos data-driven** (tipos parametrizables) — listo para agregar
  comportamientos en fases posteriores.
- **Makefile** para la lógica C: objetivos `native` (gcc) y `wasm` (emcc).
  Compilar con **`-Wall -Wextra -Werror`**.
- **Frontend web (scaffold)**: **TypeScript + Vite** (`web/`), `<canvas>`, carga del
  WASM/estado, assets Kenney CC0, menús/HUD en HTML/CSS (la UI completa es la Fase 5).
- **Runner nativo CLI**: ejecutable de consola (gcc) para ejercitar el engine.
- Ajustar `.gitignore` y decidir si el binario `invasion` (viejo) se retira del
  control de versiones.

## Fase 2 — Tests del engine

> Detalle y tabla de cobertura en [`FASE2_TESTS.md`](./FASE2_TESTS.md).

- **Framework**: Criterion (`libcriterion-dev`, disponible en Ubuntu 24.04; requiere
  **sudo**). **Un solo binario** de tests con los **tests unitarios + sim headless**.
- **Cobertura**: scheduling (SJF/RR/Random + edge), dificultad procedural (crecimiento
  y clamp), colisiones (bordes, bounding box, múltiples balas), máquina de estados
  (transiciones válidas/ilegales), movimiento/`update(dt)` (dt extremos, límites),
  `WinCondition` (umbrales y tiempos), proyectiles/nave (límites, cooldown, vidas).
- **Sim headless**: para cada modo corre N ticks con semilla fija y valida invariantes
  (activos ≤ max, límites, estado válido, convergencia a estado terminal).
- **Determinismo**: semilla fija en `game_create`.
- **Makefile**: `make test` (gcc + Criterion + `-Wall -Wextra -Werror -g`) y
  `make coverage` (gcov, % de cobertura para el README).

## Fase 3 — Catálogo de defectos heredados + normas de calidad (lógica C)

- **Enfoque:** no editar el código viejo (se sustituye en la Fase 1); la Fase 3
  cataloga los defectos del original que la migración NO debe conservar y fija las
  normas de calidad. Detalle en [`FASE3_QUALITY.md`](./FASE3_QUALITY.md).
- **Bugs lógicos** (resueltos por diseño en `src/`, validados por los tests F2):
  - `alternateSort` (RR) frágil (TODO "comprobar") → RR = alternancia real por tipo.
  - `initEnemies()` mezclaba sorteo con inicialización → generación procedural en
    `difficulty.c`.
  - Doble daño al mismo enemigo en un frame (colisión re-procesada).
  - Cajas de colisión desalineadas (nave 80×60 vs ±40²; disparo ±15).
  - Movimiento por contador de frames (no determinista) → `update(dt)`.
  - Intervalo de spawn fijo al arrancar el hilo → por nivel desde `difficulty.c`.
  - Win check repetido dentro del loop de colisiones → a `game_update`.
  - Nave sin clamp a bordes.
  - `Enemy enemies[MAX_ENEMIES]` global estático → encapsulado en `game_t`.
- **Typos y nomenclatura:** `drawWinn*` (render C retirado), "projectil" →
  `projectile`. Estilo `snake_case`/`PascalCase`/`UPPER_CASE`; 0 *warnings*
  (`-Wall -Wextra -Werror`).
- **Política de idioma: TODO en inglés** (identificadores, comentarios, API WASM).
- **Deuda leve:** constantes mágicas al modelo data-driven (`FASE3_QUALITY.md` §C).
- **Higiene/memoria:** engine C puro sin fugas/UB → `make test` con **ASan+UBSan**;
  límites de arrays verificados.
- **Criterio de salida:** `make test` verde con sanitizers y 0 *warnings*.

## Fase 4 — Contenido de juego y dirección (spec)

> La Fase 4 define el **contenido** (ninguna implementación de UI). Lógica en
> [`FASE4_FEATURES.md`](./FASE4_FEATURES.md); dirección de arte/audio en
> [`FASE4_DESIGN.md`](./FASE4_DESIGN.md).

- **Scheduling (5 modos):** SJF/RR/FIFO pulidos (deterministas) + **Waves**
  (batches inmediatos + wave cleared detection) y **Formations** (formación en
  bloque + level advancement).
- **Condiciones de victoria (5 `WinCondition`):** `SCORE_THRESHOLD` (Progressive),
  `KILL_X_WITHIN_TIME` (Alternate), `SURVIVAL_TIME` (Random),
  `SURVIVE_N_WAVES` (Waves), `REACH_LEVEL` (Formations).
- **Tipos de enemigos (5 data-driven):** Grunt, Tank, Dart, Hover (2 fases),
  Swarm. *Stretch:* Splitter.
- **Vidas (3)** + respawn con inmunidad breve; 0 vidas o enemigos al fondo →
  `GAME_OVER` (implementado y testeado en Fases 1–2).
- **~~Power-ups~~**: retirados del scope — ver `FASE4_FEATURES.md` §5.
- **Puntaje** por tipo + **high score de sesión** (persistencia en Fase 5).
- **Snapshot enriquecido** (Fase 4): posiciones/counts de enemigos y proyectiles
  + modo actual, para que el frontend pueda renderizar (Fase 5).
- **Dificultad procedural** en `difficulty.c` (intervalo de spawn, velocidad
  global, mezcla de tipos; clampeada a `MAX_ENEMIES`).
- **Dificultad procedural** en `difficulty.c` (intervalo de spawn, velocidad
  global, mezcla de tipos; clampeada a `MAX_ENEMIES`).
- **Dirección de arte** — "neón espacial moderno": assets **Kenney (CC0)**
  (Space Shooter Redux), neón vía render (glow/partículas/tint), tipografía
  **Google Fonts OFL (Orbitron)**. Sustituye a los PNG originales.
- **UI/UX (spec):** Mode Select → Playing → Pause/Game Over/Victory + How to
  Play; HUD superior (vidas, puntaje, nivel, modo, high score).
- **Audio:** Web Audio **sintetizado** (sin archivos): música por estado + SFX.

## Fase 5 — Frontend web + README (presentación)

> Implementación del frontend según el spec de la Fase 4. Detalle en
> [`FASE5_FRONTEND.md`](./FASE5_FRONTEND.md).

- **Canvas:** 1920×1080 internos, CSS centering (`object-fit: contain`), fondo
  oscuro CSS, **fullscreen toggle** (Fullscreen API).
- **HUD:** HTML overlay (barra superior): vidas, puntaje, nivel, modo, high score.
- **Menús:** HTML/CSS overlays con transiciones CSS — Mode Select (5 modos),
  How to Play, Pause (ESC), Game Over, Victory.
- **Input:** mouse (nave + disparo), ESC (pausa). Touch = stretch.
- **Polish:** pantalla de carga WASM (spinner), toggle de música (altavoz),
  manejo de errores WASM (mensaje amigable), transiciones CSS entre pantallas.
- **Audio:** Web Audio sintetizado, AudioContext desbloqueado en el primer gesture;
  música por estado + SFX.
- **Assets:** Kenney Space Shooter Redux (CC0), tipografía **Orbitron** (Google
  Fonts, OFL). PNGs originales sustituidos.
- **README** en **inglés**: esqueleto en F5 → screenshots al terminar F5 → demo
  link tras F6 (deploy) → cierre en F7. Contenido: título, features, arquitectura
  C→WASM, quick-start, controles, estructura, dependencias, limitaciones, cita
  Kenney (CC0).

### Correcciones post-revisión (feedback visual)

Aplicadas tras revisión del frontend desplegado (concisa; toda lógica en `src/`):

1. **Tarjetas de modo** en negro ilegibles → `color: var(--text)`; **layout vertical**
   (`flex-direction: column`, una debajo de otra). → `web/src/styles.css`
2. **Música no repetitiva**: motor reescrito — bucle de 64 pasos (~15 s), bass
   drone + pads en rotación (Am→F→C→G) + arpegios pentatónicos; menú = drone+pads,
   juego = + arpegio; Waves = BPM 150 mayor; volumen ambiental reducido.
   → `web/src/audio.ts`
3. **Modos originales rediseñados** al patrón "batch → clear → reponer → escalar":
   Progressive = SJF (más débil primero), Alternate = Round Robin, Random = shuffle
   determinista; spawn del batch completo al iniciar. → `src/game.c`
4. **Condición de victoria alcanzable**: `DIFF_BASE_ENEMIES 6`, `DIFF_ENEMIES_PER_LVL 1`
   (Progressive 6 enemigos = 520 pts ≥ 500; Alternate 10 kills; Random sobrevivir 45 s).
   → `src/balance.h`
5. **Velocidades rediseñadas** (no triviales): Grunt 120, Tank 80, Dart 240,
   Hover 110, Swarm 180 px/s. → `src/enemy.c`
6. **Temporizador visible** en Random/Alternate (`survival_timer` en el snapshot,
   ahora 12 escalares + mode; `_Static_assert` actualizado). → `src/types.h`,
   `web/src/game.ts`, `web/index.html`, `web/src/main.ts`, `web/src/render.ts`
7. **Feedback de colisión**: `SHIP_HIT_RADIUS 26` (detección más temprana), flash
   rojo de daño, parpadeo de invulnerabilidad y explosiones de partículas al
   destruir enemigos. → `src/collision.c`, `web/src/render.ts`

Validation: `make native` + `make test` + `make wasm` + `npm run build` verdes;
smoke test headless (`web/smoke.test.mjs`, Playwright) confirma batch de 6 en
Progressive, timer en Random/Alternate y sin errores de consola.

## Fase 6 — CI + deploy GitHub Pages

> Pipeline de CI/CD via GitHub Actions. Detalle en [`FASE6_CI.md`](./FASE6_CI.md).

- **GitHub Actions** (`.github/workflows/deploy.yml`):
  - **Triggers:** push a `main` + PRs; deploy solo en push a `main`.
  - **Steps:** checkout → Node.js (cache npm) → emsdk (cache) → `make native` →
    `make test` (Criterion + ASan/UBSan) → `make wasm` → `cd web && npm run build`
    → **deploy `dist/`** a GitHub Pages (desde Actions).
  - **Si los tests fallan → se detiene** (no deploy).
  - Permisos: `pages: write` + `id-token: write`.
- **Badge** de CI en el README (señal de rigor).
- **Prerequisito:** Fase 1 completa (repo reestructurado, binario `invasion`
  retirado).

## Fase 7 — Governance y verificación final

> Verificación contra el DoD y limpieza del repo. Detalle en
> [`FASE7_GOVERNANCE.md`](./FASE7_GOVERNANCE.md).

- **LICENSE MIT** en la raíz del repo; referenciado en el README.
- **Eliminar legacy:** `Game_Logic/`, `Visual/`, `main.c`, `invasion` (binario),
  PNGs originales (git preserva la historia).
- `.gitignore` completo (dist, node_modules, *.o, *.wasm, emsdk, .vscode, .cache)
  y `.editorconfig` (indent 2, UTF-8, LF).
- **Trackear docs:** `docs/portfolio/*` y `docs/planning/*` → commit a git.
- **AGENTS.md completo:** arquitectura, estructura, setup, build, test, convenciones,
  CI/CD, decisiones clave (links a docs).
- **Verificación DoD:** build 0 errores + 0 warnings, tests verdes, `make coverage`
  supera umbral, `git status` limpio, quick-start reproducible desde clone limpio,
  links markdown válidos, README con datos reales (screenshots, demo link, badge CI).
- **Commits:** no reorganizar historial; esta fase solo verifica integridad.

## Orden de ejecución (numeración = orden)

1. **Fase 0** — Instalar Emscripten (emsdk) + validar toolchain.
2. **Fase 1** — Migración: reorganizar `src/`+`web/`+`tests/`, refactor engine C
   (`game_t`, single-thread, scheduling extensible, enemigos data-driven,
   `WinCondition`), Makefile (native/wasm) + scaffold web (TS/Vite) + `.gitignore`.
3. **Fase 2** — Tests del engine (Criterion) + sim headless + `make test`/`coverage`.
4. **Fase 3** — Bugs y typos (verificados por los tests).
5. **Fase 4** — Contenido y dirección (spec): modos/condiciones/enemigos/power-ups,
   dirección de arte, UI/UX y audio. → `FASE4_FEATURES.md` + `FASE4_DESIGN.md`.
6. **Fase 5** — Frontend completo + README + capturas reales.
7. **Fase 6** — CI + deploy GitHub Pages.
8. **Fase 7** — LICENSE + eliminar legacy + trackear docs + AGENTS.md + verificación
   final (DoD).

## Progreso de fases

| Fase | Estado |
|---|---|
| 0 — Entorno | ✅ completa (emsdk 6.0.9, `PHASE0_ENVIRONMENT.md`) |
| 1 — Migración del engine | ✅ completa (`fe1db18`, `2299172`) |
| 2 — Tests del engine | ✅ completa (`33bf89e`, `fccf09e`; 50 tests, cobertura 95.86%) |
| 3 — Calidad / bugs | ✅ completa (§B audit 0 issues, §C balance.h, §D 65 tests, ASan clean) |
| 4 — Contenido y dirección | ✅ completa (mecánica Waves/Formations + snapshot enriquecido; 78 tests, cobertura ~97%; deuda auditada y cerrada) |
| 5 — Frontend + README | ⏳ casi completo (UI funcional y verificada en navegador headless; falta el README raíz + screenshots, demo link en F6) |
| 6 — CI + GitHub Pages | ⏳ pendiente |
| 7 — Governance y verificación | ⏳ pendiente |

## Decisiones abiertas / por confirmar

- [x] Entorno: arquitectura híbrida (C→WASM + TypeScript/Vite), single-thread,
      assets Kenney CC0, doble build. → `ENVIRONMENT_DECISION.md`
- [x] Fase 1 (fundaciones del engine): estructura `src/`+`web/`+`tests/`, `game_t`
      con puntero + semilla, `update(dt)` determinista, scheduling extensible,
      enemigos data-driven, `WinCondition` desacoplada, `VICTORY` reintroducida.
      → `GAME_DESIGN.md`
- [x] Fase 2 (tests): Criterion, un solo binario (unitarios + sim headless),
      semilla configurable, `make test` + `make coverage`. → `FASE2_TESTS.md`
- [x] Idioma del README: inglés. → `FASE5_FRONTEND.md`
- [x] Quitar el binario `invasion` del control de versiones (eliminar en Fase 7).
- [x] LICENSE: MIT (Fase 7). → `FASE7_GOVERNANCE.md`
- [x] Fase 5 (frontend): canvas 1920×1080, HTML overlay HUD, audio Web Audio,
      fullscreen toggle, polish (loading/error/toggle/transiciones). →
      `FASE5_FRONTEND.md`
- [x] Correcciones post-revisión (F5): tarjetas verticales legibles, música no
      repetitiva, modos originales → batch/clear/escalar, balance alcanzable
      (enemigos/velocidades), timer en Random/Alternate, feedback de colisión.
      → sección "Correcciones post-revisión" de `POLISH_PLAN.md`
- [x] Fase 6 (CI): GitHub Actions (test+build+deploy), triggers main+PRs, cache
      emsdk, deploy desde Actions a Pages. → `FASE6_CI.md`
- [x] Fase 7 (governance): MIT, eliminar legacy, AGENTS.md completo, DoD final.
      → `FASE7_GOVERNANCE.md`
- [x] Fase 4 (contenido y dirección): 5 modos de scheduling, 5 `WinCondition` por
      modo, 5 tipos de enemigos, 3 vidas, dificultad procedural, dirección
      arte/UI/audio ("neón espacial moderno" + Kenney CC0 + Web Audio).
      **Power-ups retirados del scope** (deferido). → `FASE4_FEATURES.md` + `FASE4_DESIGN.md`

## Planes relacionados

- `ENVIRONMENT_DECISION.md` — ADR del entorno híbrido (por qué esta arquitectura).
- `GAME_DESIGN.md` — decisiones y alcance del engine de juego (Fase 1 vs futuro).
- `PHASE0_ENVIRONMENT.md` — herramientas exactas y validación del entorno.
- `FASE2_TESTS.md` — detalle y cobertura de los tests del engine.
- `FASE3_QUALITY.md` — catálogo de defectos heredados y normas de calidad.
- `FASE4_FEATURES.md` — contenido de juego (lógica) de la Fase 4.
- `FASE4_DESIGN.md` — dirección de arte, UI/UX y audio (spec) de la Fase 4.
- `FASE5_FRONTEND.md` — implementación del frontend y README (Fase 5).
- `FASE6_CI.md` — pipeline de CI/CD y deploy (Fase 6).
- `FASE7_GOVERNANCE.md` — governance, limpieza y verificación final (Fase 7).
- `../portfolio/PORTFOLIO_AGENTS.md` — cómo se trabaja el repo.
- `../portfolio/PORTFOLIO_QUALITY.md` — estándar de "listo para portafolio".
- `../portfolio/PORTFOLIO_ROADMAP.md` — cómo elevar un proyecto básico.