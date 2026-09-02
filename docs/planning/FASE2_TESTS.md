# Fase 2 — Tests del engine (detalle)

> Documento de soporte del [`POLISH_PLAN.md`](./POLISH_PLAN.md). Detalla la fase de
> **tests del engine C** (unitarios + sim headless). Complementa a
> [`GAME_DESIGN.md`](./GAME_DESIGN.md) (el motor que se testea).

## Objetivo

Verificar el motor (`src/`, migrado en la Fase 1) con **tests unitarios + sim
headless** en un **solo binario** nativo (`make test`), **antes** de ampliarlo con
contenido (Fase 4). Los tests son deterministas gracias a la semilla configurable
de `game_create(seed, ...)`.

## Framework e integración

- **Criterion** (`libcriterion-dev`, 2.4.1 en Ubuntu 24.04 / universe).
  - Disponible en apt; **requiere sudo** (lo ejecuta el usuario).
  - Se integra vía `pkg-config --cflags --libs criterion` en el Makefile.
  - **No** se vincula al WASM: solo al binario de tests nativo.
- **Un solo binario** de tests: contiene los **tests unitarios** (por módulo) + la
  **sim headless** en el mismo ejecutable.
- **Makefile**:
  - `make test` → compila `tests/` + `src/` con gcc + Criterion +
    `-Wall -Wextra -Werror -g -fsanitize=address,undefined`
    (ASan+UBSan, ver Fase 3) y ejecuta.
  - `make coverage` → recompila con instrumentación (`-fprofile-arcs
    -ftest-coverage`) y genera reportes con `gcov` (% de líneas cubiertas) para el
    README del portafolio.

## Determinismo

- `game_create(seed, ...)` acepta la semilla (Fase 1); los tests fijan semillas
  conocidas → resultados reproducibles (mismo estado ⇔ misma semilla).
- La sim headless usa semilla fija por modo.

## Tests unitarios por módulo (cobertura)

### `test_scheduling.c` — scheduling (SJF / RR / Random)
- SJF ordena los enemigos por vida ascendente.
- SJF con vidas iguales → *tie-breaking* estable (no reordena arbitrariamente).
- RR (alternado) intercala tipos 1/2.
- Random/FIFO no reordena el array generado.
- Con 0 enemigos (nivel vacío) → no crashea y el estado es coherente.
- Con el máximo de enemigos (`MAX_ENEMIES`).
- Comportamiento con array ya ordenado / desordenado.

### `test_difficulty.c` — dificultad procedural
- Nivel 1 → dificultad mínima.
- Nivel alto → saturación: parámetros **clampeados** (nº enemigos nunca > `MAX_ENEMIES`).
- Los parámetros crecen con el nivel (monotonicidad razonable dentro de límites).
- Los intervalos de spawn y velocidades se mantienen en rangos válidos.

### `test_collision.c` — colisiones
- Bala alcanza enemigo → daño/destrucción y actualización de puntaje.
- Bala que no toca → permanece activa.
- Bala fuera de los bordes → se elimina.
- Colisión exacta en el límite del *bounding box*.
- Múltiples balas en el mismo frame → sin doble daño sobre el mismo enemigo.

### `test_state_machine.c` — máquina de estados
- Transiciones válidas: `MENU → PLAYING → PAUSED → PLAYING → GAME_OVER`.
- `GAME_OVER → restart → PLAYING` limpio (estado reiniciado).
- `VICTORY → nueva partida` limpio.
- Pausa/reanudar repetido → nunca estado inválido.
- Transiciones ilegales ignoradas (p. ej. `update()` en `MENU` no crashea).

### `test_movement.c` — movimiento y `update(dt)`
- Los enemigos avanzan según `dt` real (independiente del nº de frames).
- `dt` extremos: `0`, negativo y muy grande → clamp/sanitización sin estados rotos.
- Límites: enemigo que llega al fondo → `GAME_OVER`.
- El *spawn* respeta el intervalo (no aparecen todos de golpe al iniciar).

### `test_wincondition.c` — `WinCondition`
- Umbral de puntos: justo antes / justo en el umbral / justo después.
- `KILL_X_WITHIN_TIME`: en el límite de tiempo exacto y pasada la ventana.
- Cada tipo de condición (p. ej. `SCORE_THRESHOLD`, `KILL_X_WITHIN_TIME`) se evalúa
  de forma independiente y correcta.

### `test_projectiles.c` — proyectiles y nave
- Nave acotada en `x` (0..WIDTH).
- Disparo respeta *cooldown* (no dispara durante la recarga).
- Vidas: golpes consecutivos hasta `GAME_OVER`.

## Cobertura ampliada (contenido de la Fase 4)

> Los tests siguientes corresponden al contenido decido en
> [`FASE4_FEATURES.md`](./FASE4_FEATURES.md).

### `test_spawn_events.c` — eventos de spawn
- El plan de spawn son eventos `{tipo, retardo, posición X}` (no una mera secuencia
  de tipos).
- Cada modo genera su plan: SJF (vida ascendente), RR (alternancia real por tipo),
  FIFO (aleatorio determinista), Waves (ráfagas con pausas), Formations
  (fila/columna con retardo coherente).
- Determinismo: misma semilla → mismo plan de eventos.

### `test_wincondition_f4.c` — condiciones de la Fase 4
- `SURVIVE_N_WAVES`: completa N oleadas → `VICTORY`; no antes.
- `REACH_LEVEL`: llega al nivel N (cada nivel = formación) → `VICTORY`.
- Mapping modo→condición por defecto (tabla del plan) evaluado correctamente.
- Re-evaluación en `game_update`, no dentro del bucle de colisiones (sin
  disparadores repetidos).

### `test_enemy_types.c` — arquetipos data-driven
- Grunt/Tank/Dart/Hover/Swarm: parámetros (vida, velocidad, patrón, puntaje)
  respetados según su tipo.
- Hover: transición de fase (deriva lateral → zambullida) en `dt`.
- Rutas/movimientos dentro de los límites según cada patrón.

### `test_powerups.c` — power-ups
- Drop: la probabilidad respeta el valor configurado; cae y desaparece si no se
  recoge (por el fondo o por expiración).
- Pickup por la nave → efecto aplicado.
- **Double Shot**: dispara 2 balas mientras dura (~8 s) y vuelve a 1 tras expirar.
- **Shield**: absorbe 1 golpe (interacciona con la inmunidad del respawn).

### `test_lives.c` — vidas / respawn / inmunidad
- Golpe → resta 1 vida y nave reaparece en posición inicial.
- Respawn con **inmunidad breve** (basada en `dt`): durante la inmunidad no se
  pierden vidas.
- 0 vidas → `GAME_OVER`.

## Sim headless (invariantes, en el mismo binario)

- **Por cada modo** (progresivo/alterno/random/waves/formations): corre `game_t`
  durante **N ticks** con semilla fija y valida:
  - nº de enemigos activos nunca > `max_enemies`;
  - ningún enemigo fuera de los límites (o reflejado correctamente);
  - el estado final es siempre uno válido (`PLAYING`/`GAME_OVER`/`VICTORY`);
  - **convergencia**: una sim de larga duración termina en un estado terminal
    (game over o victory) sin crashear.
- **Multi-semilla**: con la misma semilla → mismo estado (reproducibilidad);
  varias semillas → sin regresiones.

## Notas / riesgos

- **`libcriterion-dev` requiere sudo**: es el primer entregable de la fase (igual
  que emsdk en la Fase 0, pero emsdk no necesita sudo).
- Los nombres/ruta exactos de los tests (`tests/test_*.c`) y el arranque de
  Criterion (`main`) se fijan al implementar (depende de los tipos finales del
  engine de la Fase 1).
- `make coverage` compila de nuevo con instrumentación: mantener los objetivos
  separados de `make test` (no mezclar objetos instrumentados y no instrumentados).

## Relacionado

- [`POLISH_PLAN.md`](./POLISH_PLAN.md) — plan general (las fases).
- [`GAME_DESIGN.md`](./GAME_DESIGN.md) — el engine que se testea (Fase 1).
- [`FASE4_FEATURES.md`](./FASE4_FEATURES.md) — contenido de juego (lógica) que
  estos tests validan.