# Decisión de entorno — Migración a arquitectura híbrida Web

> Registro de decisión (ADR). Documenta **por qué** y **cómo** se reemplaza el
> entorno original (C + X11 + SDL2, escritorio) por una arquitectura híbrida
> orientada a web. Complementa a [`POLISH_PLAN.md`](./POLISH_PLAN.md) y a
> [`PHASE0_ENVIRONMENT.md`](./PHASE0_ENVIRONMENT.md).

## Contexto

Proyecto original: juego arcade en **C** usando **X11** (entrada/ventana) y
**SDL2 + SDL2_image + SDL2_ttf** (render). Multihilo (`pthreads`). Se persigue
convertirlo en pieza de portafolio para un puesto **junior** con orientación a
**desarrollo web**, conservando **C** como lenguaje del curriculum.

Requisitos del proyecto portafolio:
- **Demoable en navegador** (toca y juega, sin instalación) — compite mejor para
  rol web.
- **Visual moderno** que impacte a primera vista (el usuario prioriza lo visual).
- **Conservar C** y la lógica de juego (scheduling, niveles, enemigos).
- **Build reproducible y sencillo** desde un clone limpio.

## Alternativas consideradas

| Opción | Ventajas | Inconvenientes | Veredicto |
|---|---|---|---|
| **Raylib (C) + Emscripten → WASM** | C puro, gráficos modernos, export web nativo, conserva C y lógica | Visual web limitado por la abstracción de Raylib | Buena opción, descartada por el deseo de máx. "wow" web |
| **Port directo SDL2 + Emscripten** | Fiel al código actual | Arrastra X11 (no soportado en web) y dependencias frágiles | Descartada |
| **Godot / Unity export a Web** | Motor gráfico potente | **Pierde C** (GDScript/C#) | Descartada (contradice conservar C) |
| **Híbrido: lógica C → WASM + UI web JS (ELEGIDA)** | Conserva C y lógica; máx. control visual web; sin dependencias gráficas en C | Reescribir la capa de render en JS | ✔ Elegida |

## Decisión

**Arquitectura híbrida:**

- **`Game_Logic/` (C puro) → WASM** (compilado con **Emscripten/`emcc`**),
  **sin ninguna dependencia gráfica** (en la Fase 1 se reorganiza a `src/`).
  Contiene el scheduler (SJF/RR/Random), niveles, enemigos, proyectiles,
  colisiones y puntaje. Expone funciones que el
  frontend invoca por frame (`update(dt)`, `fire(x,y)`, obtención de estado).
- **UI web en TypeScript + Vite**: render en `<canvas>` 2D con **assets Kenney
  (CC0)** (rectifican la decisión original de "sprites propios"; los PNG existentes
  tienen licencia desconocida y se sustituyen) + glow/partículas/easing; menús y HUD
  con HTML/CSS moderno. Detalle en [`FASE4_DESIGN.md`](./FASE4_DESIGN.md).
- **Scheduler unificado a single-thread**: la lógica es un main-loop por `dt`
  determinista que el frontend impulsa cada frame (patrón `emscripten_set_main_loop`
  / `requestAnimationFrame`). El scheduling conceptual (progresivo/alterno/random)
  se conserva como orden de procesamiento.
- **Doble build de la lógica**:
  - **WASM** (`emcc`) → consumida por la web (Vite).
  - **Nativa** (`gcc`, C puro) → tests rápidos y CI de la lógica.
- **X11 y SDL2 se eliminan por completo** del código de juego.

## Justificación

- **Conserva C** (activo de curriculum) y toda la lógica de juego.
- **"Wow" visual web** real: total control del canvas/partículas/render (sobre
  assets CC0), más alineado con un puesto web junior que la abstracción de un
  engine C.
- **Sin dependencias gráficas frágiles en la lógica** (ni X11 ni SDL), lo que
  hace la lógica pura, testeable y portable.
- **Demoable en GitHub Pages**: por defecto sirve estáticos sin headers
  especiales; al unificar a single-thread evitamos los requisitos de
  `SharedArrayBuffer` + `COOP/COEP` que exigirían los threads en WASM.
- **Instalación reproducible sin sudo** (emsdk vive en el home del usuario).

## Trade-offs asumidos

- El multihilo real (`pthreads`) **no se porta** a la versión final: se sustituye
  por un main-loop de scheduling single-thread, portable a web.
- La capa de render se escribe **en JS/TS** (no en C): se abandona el render C,
  pero la lógica permanece en C.
- Requiere **Emscripten** y **Node/npm** como toolchain (ambos reproducibles y sin
  sudo).

## Estado

De acuerdo con el usuario (fecha: 2026-09-01). Pendiente de implementación.

## Relacionado

- [`POLISH_PLAN.md`](./POLISH_PLAN.md) — las fases de ejecución.
- [`GAME_DESIGN.md`](./GAME_DESIGN.md) — decisiones del engine de juego (Fase 1).
- [`PHASE0_ENVIRONMENT.md`](./PHASE0_ENVIRONMENT.md) — herramientas exactas y
  validación del entorno híbrido.
- [`FASE4_DESIGN.md`](./FASE4_DESIGN.md) — dirección de arte, UI/UX y audio
  (rectifica los assets: Kenney CC0).
