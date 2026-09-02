# Fase 5 — Frontend web + README (detalle)

> **Estado de implementación:** la UI completa (canvas + HUD + menús + input +
> audio + WASM loading) está **implementada y verificada** con un smoke test
> headless (Playwright/Chromium: 5 cards de modo, inicio de partida, HUD, sin
> errores de consola). Assets Kenney CC0 commiteados en `web/public/assets/`.
> **Pendiente:** README raíz (este documento es el spec) y capturas reales para el
> README (se cierran en F6 con el deploy).

> Documento de soporte del [`POLISH_PLAN.md`](./POLISH_PLAN.md). Define la
> implementación del **frontend web** (sobre el spec de
> [`FASE4_DESIGN.md`](./FASE4_DESIGN.md)) y el **README**. La lógica de juego
> está en [`FASE4_FEATURES.md`](./FASE4_FEATURES.md).

## 1. Canvas y layout

- Resolución interna fija: **1920×1080**.
- CSS centering (`object-fit: contain`) para que se adapte a la ventana del
  navegador sin deformarse.
- Fondo oscuro CSS más allá del canvas (extiende el tema neón).
- **Fullscreen toggle** (Fullscreen API): botón discreto que expande el canvas a
  pantalla completa; al volver, restaura el tamaño anterior.

## 2. HUD (HTML overlay)

Barra superior fija fuera del canvas (HTML/CSS):
- **Vidas** (3, iconos del pack Kenney), **puntaje**, **nivel**, **modo** y
  **high score de sesión**.
- Tipografía **Orbitron** (Google Fonts, OFL).
- Transiciones CSS al actualizar valores (fade sutil).

## 3. Menús (HTML/CSS overlays)

```
Mode Select ──▶ Playing ──┬─▶ Pause (ESC)
        │                 ├─▶ Game Over (score + "Restart")
        │                 └─▶ Victory (score + "Play Again")
        └─▶ How to Play (instrucciones/controles)
```

- Overlays modales con fondo semitransparente + transiciones CSS (fade).
- **Mode Select**: 5 botones/cards (una por modo) con nombre + descripción corta
  de la condición de victoria.
- **How to Play**: controles (mouse: mover nave; clic: disparar; ESC: pausa) +
  brief del juego.
- **Game Over**: puntaje final + high score + botón restart.
- **Victory**: puntaje final + high score + botón play again.

## 4. Input

- **Mouse**: puntero sigue la nave (X/Y); **clic izquierdo** dispara.
- **Teclado**: ESC (pausa/reanudar). Soporte táctil = stretch (no comprometido).

## 5. Audio (Web Audio API)

- **Música**: loop synth por estado (menú / partida). Intensidad mayor en oleadas
  (Waves). Desbloqueo en primer gesture (clic del Mode Select) — requisito del
  navegador.
- **SFX**: disparo, explosión, daño, power-up, game over, victoria, click de UI.
- **Toggle de música**: botón altavoz para silenciar/reanudar (UX: la música
  auto-play es molesta sin pedir permiso).
- Todo generado con Web Audio API (osciladores + envelopes, sin ficheros).

## 6. Polish del frontend

- **Pantalla de carga WASM**: spinner o barra de progreso mientras se inicializa
  el módulo WASM; se oculta al estar listo.
- **Manejo de errores WASM**: si la carga falla, muestra mensaje amigable en vez
  de pantalla blanca.
- **Transiciones CSS**: fade/opacidad al cambiar entre pantallas (menú → juego,
  pausa, game over, victory).

## 7. WASM loading

- Emscripten con `MODULARIZE=1` + `-s EXPORT_NAME='createModule'`.
- Vite sirve los archivos `.js` (glue) + `.wasm` como assets estáticos.
- TS importa el módulo y lo inicializa al cargar la página.

## 8. Assets

- **Kenney Space Shooter Redux (CC0)**: selección de naves, enemigos (5 tipos),
  proyectiles, fondo. Descargado como ZIP CC0 desde **OpenGameArt** (kenney.nl
  está detrás de bot-protection); ver `web/public/assets/LICENSE.txt`.
- **Tipografía**: Google Fonts **Orbitron** (OFL).
- Los PNG originales del juego (licencia desconocida) se **sustituyen**.

## 9. README

- **Idioma:** inglés.
- **Timing:** esqueleto completo en F5 (sin screenshots/demo link) → screenshots
  al terminar F5 → demo link tras F6 (deploy a GitHub Pages) → cierre en F7.
- **Contenido:**
  1. Título + descripción de una línea.
  2. Link demo (placeholder F5 → real F6/F7).
  3. Screenshot(s) reales del juego en el navegador.
  4. Features.
  5. Arquitectura (lógica C→WASM + TypeScript/Vite).
  6. Quick-start (clone → build → run, nativo y web).
  7. Controles.
  8. Estructura del repo.
  9. Dependencias.
  10. Limitaciones / futuro.
  11. Créditos: Kenney (CC0) con enlace.

## 10. Dependencias del frontend

- **Node/npm** (ya disponible).
- **Vite** (devDependency, se instala con `npm install`).
- **TypeScript** (devDependency).
- **Kenney Space Shooter Redux** (CC0, ZIP vía OpenGameArt).
- **Google Fonts Orbitron** (OFL, cargado vía CDN en HTML).

## 11. Scripts npm

- `npm run dev` → Vite dev server.
- `npm run build` → build de producción (dist/).
- `npm run preview` → preview del build de producción.

## Validación

- Juego jugable en el navegador (Chrome/Firefox/Edge) via dev server y build
  de producción.
- Fullscreen toggle funciona correctamente.
- Audio: música y SFX se reproducen; toggle silencia/reanuda.
- Pantalla de carga WASM visible; error WASM muestra mensaje amigable.
- Transiciones CSS suaves entre pantallas.
- README completo con screenshot(s) real(es).

## Relacionado

- [`POLISH_PLAN.md`](./POLISH_PLAN.md) — las fases de ejecución.
- [`FASE4_DESIGN.md`](./FASE4_DESIGN.md) — dirección de arte, UI/UX y audio (spec).
- [`FASE4_FEATURES.md`](./FASE4_FEATURES.md) — contenido de juego (lógica).
- [`FASE2_TESTS.md`](./FASE2_TESTS.md) — tests del engine (validan la lógica
  consumida por este frontend).