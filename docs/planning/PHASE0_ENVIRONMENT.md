# Fase 0 — Entorno y reproducibilidad (detalle)

> Documento de soporte del [`POLISH_PLAN.md`](./POLISH_PLAN.md) y de la
> [`ENVIRONMENT_DECISION.md`](./ENVIRONMENT_DECISION.md). Recoge el estado
> **verificado** del entorno de la arquitectura híbrida (C→WASM + TypeScript/Vite).
> Se actualiza en la medida en que el entorno cambie (p. ej. tras instalar emsdk).

## Entorno híbrido (resumen)

- **Lógica del engine en `src/` (C puro)** → se compila a **WASM** (Emscripten) para
  la web y a **nativa** (gcc) para tests/CI. **Sin dependencias gráficas**.
- **UI web** en **TypeScript + Vite** + `<canvas>` (assets **Kenney CC0**) + menús en
  HTML/CSS.
- **X11 y SDL2 ya NO son dependencias** del proyecto.

## Estado verificado del entorno (fecha: 2026-09-02)

| Componente                | Estado | Detalle |
|---------------------------|--------|---------|
| SO                        | ✅      | Ubuntu 24.04.4 LTS |
| Arquitectura              | ✅      | x86_64 |
| `gcc`                     | ✅      | 13.3.0 (Ubuntu) — build nativa C |
| `make`                    | ✅      | GNU Make 4.3 |
| `git`                     | ✅      | 2.43.0 |
| `node`                    | ✅      | v22.23.0 — frontend tooling |
| `npm`                     | ✅      | 10.9.8 |
| `python3`                 | ✅      | 3.12.3 — dev server / `emrun` |
| **Emscripten (`emcc`)**   | ✅      | emsdk 6.0.9 instalado en `/home/lia/emsdk` (activate + `.bashrc`) |
| SDL2/X11 dev packages     | n/a     | **Ya no se necesitan** (eliminados) |
| `sudo` sin contraseña     | ❌      | Requiere contraseña (no necesario si usamos emsdk en el home) |

Conclusión: la infraestructura base (gcc, make, node, python) está lista. **Falta
instalar Emscripten (emsdk)** para poder compilar la lógica C a WASM.

## Instalación del toolchain

### 1. Emscripten SDK (emsdk) — sin sudo, en el home del usuario

La vía soportada y reproducible es clonar `emsdk` y activarlo (no requiere
contraseña de sudo):

```sh
# desde ~
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
# en cada shell que vaya a compilar:
source ./emsdk_env.sh
```

Tras `source emsdk_env.sh`, `emcc` y `emrun` deben quedar disponibles:

```sh
emcc --version
```

### 2. Frontend (TypeScript + Vite)

Se materializa dentro del proyecto (no es un binario global de sistema). Usamos
`npm` para el scaffolding y dependencias. (El detalle de estructura va en la
Fase 1.)

```sh
# dentro del repo, una vez definida la estructura web/
npm create vite@latest web -- --template vanilla-ts   # o según estructura acordada
cd web && npm install
```

## Validación del entorno

1. Compilar la lógica a **nativa** (C puro, sin gráficas):

   ```sh
   gcc -Wall -Wextra -c src/types.c -o /tmp/types.o   # (nombres de archivos según la
   gcc -Wall -Wextra -c src/game.c -o /tmp/game.o     #  estructura final de la Fase 1)
   ```

   (Debe compilar sin errores ni warnings.)

2. Compilar la lógica a **WASM** con Emscripten:

   ```sh
   emcc -O2 src/*.c -o /tmp/logic.js -sEXPORTED_FUNCTIONS=...   # según la API de la Fase 1
   ```

   > Nota: los puntos de exportación concretos (`-sEXPORTED_FUNCTIONS`,
   > `-sEXPORTED_RUNTIME_METHODS`) se definen en la Fase 1 al diseñar la interfaz
   > C ↔ JS. Aquí solo se valida que `emcc` esté operativo.

3. Frontend: `cd web && npm run build` (o el dev server `npm run dev`).

## Gotchas / riesgos

- **Emscripten no está instalado**: es el primer entregable. Se instala en el
  **home** y NO requiere sudo (a diferencia de los apt de SDL2 que quedaron
  descartados).
- **Credenciales de red**: `emsdk install latest` y `npm install` descargan de
  internet; si el entorno está aislado, fallarán.
- **pthreads NO se porta**: la decisión es unificar a **single-thread** (ver
  [`ENVIRONMENT_DECISION.md`](./ENVIRONMENT_DECISION.md)). Evita los requisitos
  `SharedArrayBuffer`/`COOP`/`COEP` de GitHub Pages.
- **El render pasa a JS/TS**: la lógica es C, pero la capa visual se escribe en el
  frontend web. Los assets PNG existentes (licencia desconocida) se **sustituyen**
  por el pack **Kenney (CC0)** (ver Fase 4).
- **`invasion` (binario antiguo) está trackeado** en git; se decidirá en la Fase 1
  si se retira (higiene de repo) y se ajusta `.gitignore`.

## Relacionado

- [`POLISH_PLAN.md`](./POLISH_PLAN.md) — plan general (fases).
- [`ENVIRONMENT_DECISION.md`](./ENVIRONMENT_DECISION.md) — por qué esta
  arquitectura.
- [`GAME_DESIGN.md`](./GAME_DESIGN.md) — decisiones del engine de juego (Fase 1).
