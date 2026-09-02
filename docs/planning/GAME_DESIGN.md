# Diseño del Engine de Juego — decisiones y alcance (Fase 1)

> Registro de decisiones de diseño de la lógica de juego. Complementa a
> [`POLISH_PLAN.md`](./POLISH_PLAN.md) (las fases) y a
> [`ENVIRONMENT_DECISION.md`](./ENVIRONMENT_DECISION.md) (el porqué del stack).
> Documenta lo que se **decide ahora** (capacidad del engine) y lo que se
> **debate en fases posteriores** (contenido).

## Principio rector del alcance

- **Fase 1 construye la *capacidad*** del engine: la arquitectura deja los sistemas
  **agregables/extensibles** para que el contenido futuro se implemente sin
  re-trabajo estructural.
- **Fases posteriores definen el *contenido***: los valores, diseños y features
  concretos.

## Decisiones de Fase 1 (fundaciones)

### 1. Scheduling extensible
- El engine tiene un **dispatcher de scheduling** como módulo/tabla que permite
  **agregar modos** nuevos.
- Se conservan los 3 modos actuales como punto de partida: **progresivo (SJF)**,
  **alterno (Round-Robin)** y **random (FIFO)**.
- **Decidido en la Fase 4** (`FASE4_FEATURES.md`): 5 modos (pulir los 3 actuales +
  **Waves** y **Formations**).

### 2. Condición de victoria configurable (desacoplada del modo)
- La victoria se modela como un componente **desacoplado** del scheduling:
  `WinCondition { tipo, parámetros }` + una función `check_win(game, params)`.
- Se incluyen **2–3 tipos** de condición como capacidad de ejemplo (p. ej.
  `SCORE_THRESHOLD`, `KILL_X_WITHIN_TIME`, ...).
- **Por defecto**, cada modo de scheduling se asocia a una condición (la
  experiencia sigue siendo "por modo"), pero el motor las mantiene separadas
  (muestra separación de responsabilidades / diseño por composición).
- **Decidido en la Fase 4** (`FASE4_FEATURES.md`): condición y valores por modo
  (`SCORE_THRESHOLD`, `KILL_X_WITHIN_TIME`, `SURVIVAL_TIME`, `SURVIVE_N_WAVES`,
  `REACH_LEVEL`).

### 3. Enemigos data-driven
- Los enemigos son un **sistema de datos** (tipos parametrizables: vida, velocidad,
  patrón de movimiento, puntaje, tamaño) en lugar de 2 tipos hardcodeados.
- Esto deja el sistema listo para **agregar comportamientos** sin tocar el motor.
- **Decidido en la Fase 4** (`FASE4_FEATURES.md`): 5 tipos concretos (Grunt, Tank,
  Dart, Hover, Swarm) y sus parámetros/comportamientos.

### 4. Estructura y modelo de partida
- Se **reorganiza** el repo: `src/` (C puro) + `web/` (TS/Vite) + `tests/` + Makefile.
- La partida es de **niveles generados proceduralmente e infinitos** (sin tope de
  nivel) como escenario; la **condición de victoria la da el modo** para ganar la
  partida, y se pierde si muere la nave.
- Máquina de estados: `MENU → PLAYING → PAUSED → GAME_OVER` y **`VICTORY`**
  (condición por modo). Se **reintroduce** `VICTORY` (antes se había eliminado al
  pensar el juego como survival sin final).
- **Scheduling gobierna el orden de aparición**; la dificultad procedural define
  cuántos enemigos y su rapidez.

### 5. Resto de la arquitectura (ya consensuada)
- `game_t` con **instancia/ puntero** (testeable; singleton en WASM).
- `update(dt)` determinista (frame-rate independiente, tiempo real en segundos).
- API WASM: `game_create`, `game_set_mode`, `game_start`, `game_update(dt)`,
  `game_set_ship`, `game_fire`, `game_get_state` (snapshot plana para el frontend).
- Runner nativo CLI + **tests unitarios y sim headless** (nativa gcc, sin navegador).
- Se eliminan X11, SDL2, `draw.c`, `xresources.c`; el render vive en web (TS).
- **Idioma y estilo del código: inglés** (identificadores, comentarios y API WASM);
  `snake_case`/`PascalCase`/`UPPER_CASE` (ver Fase 3).

## Contenido decidido en fases posteriores (documentado, no diseño del engine)

1. **Scheduling**: 5 modos (3 pulidos + Waves + Formations). → `FASE4_FEATURES.md`
2. **Condición de victoria por modo**: condiciones específicas y valores. →
   `FASE4_FEATURES.md`
3. **Tipos de enemigos y comportamiento**: tipos concretos (5). → `FASE4_FEATURES.md`
4. **Otros detalles**: vidas (3), power-ups, puntaje/high score, dificultad. →
   `FASE4_FEATURES.md`
5. **Dirección de arte, UI/UX y audio** (spec para el frontend). →
   `FASE4_DESIGN.md`

## Estado

De acuerdo con el usuario (fecha: 2026-09-01). Pendiente de implementación en la
Fase 1.

## Relacionado

- [`POLISH_PLAN.md`](./POLISH_PLAN.md) — las fases de ejecución.
- [`ENVIRONMENT_DECISION.md`](./ENVIRONMENT_DECISION.md) — el porqué del stack
  híbrido.
- [`FASE4_FEATURES.md`](./FASE4_FEATURES.md) — contenido de juego (lógica).
- [`FASE4_DESIGN.md`](./FASE4_DESIGN.md) — dirección de arte, UI/UX y audio.
