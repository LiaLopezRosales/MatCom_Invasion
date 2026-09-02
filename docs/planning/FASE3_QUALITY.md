# Fase 3 — Catálogo de defectos heredados + normas de calidad (detalle)

> Documento de soporte del [`POLISH_PLAN.md`](./POLISH_PLAN.md). Define la Fase 3
> como el **catálogo de defectos del código original que la migración (Fase 1) no
> debe conservar**, más las **normas de calidad** (idioma, nomenclatura, sanitizers).
> No es "parchear el código viejo": se enumeran los defectos para que el engine
> nuevo (`src/`) los resuelva por diseño y los tests (Fase 2) los validen.

## Enfoque

- La lógica original se sustituye en la Fase 1 por el engine nuevo (`game_t`, data
  driven, `dt`). La Fase 3 **no edita código viejo**.
- Cada defecto A/B/C debe tener **al menos un test** en la Fase 2 (mapeo
  defecto → test al final).
- **Tras la Fase 2 (2026-09-02)** el catálogo A (bugs lógicos) quedó **resuelto
  y validado por tests** (ver tabla en §A). Re-encuadre de Fase 3:
  - §B **Auditoría final de nomenclatura/idioma** (ya en inglés; verificar la
    API WASM y comentarios).
  - §C **Extraer constantes mágicas** restantes a valores con nombre / tabla
    data-driven (radi de colisión, speed, intervalos de spawn, ratio de tipos).
  - **Robustez / fuzzing**: `make test` con ASan+UBSan ya corre; añadir casos
    borde (límites de arrays, niveles altos, modos extremos) y confirmar 0
    fugas.
- Criterio de salida: `make test` verde con **ASan/UBSan** y **0 warnings**
  (`-Wall -Wextra -Werror`).

## A. Bugs lógicos (resolver en el rediseño)

> **Estado tras Fase 2 (2026-09-02):** todos los ítems A se resolvieron por
> diseño en el engine nuevo (`src/`) y sus tests (Fase 2) ya los validan. La
> columna *Test F2* indica el test que lo cubre (ver `FASE2_TESTS.md`).

| # | Defecto (original) | Solución en el engine nuevo | Test F2 | Estado |
|---|---|---|---|---|
| A1 | Doble daño: dos balas golpean al mismo enemigo en un frame (quedaba `active=1` hasta el final del loop) | Marcar enemigo afectado/eliminado y no re-procesarlo en el mismo frame (`hit_this_frame[]`) | `two_projectiles_no_double_damage_same_frame` | ✅ resuelto |
| A2 | `alternateSort` (RR) frágil: ordena por *vida* y asume que eso agrupa por *tipo* | RR = alternancia real por tipo (`generate_alternate` con array de tipos) | `alternate_intercalates_types` | ✅ resuelto |
| A3 | Cajas de colisión desalineadas: nave 80×60 vs choque ±40×±40; disparo ±15×±15 | Áreas de colisión coherentes, definidas en el modelo data-driven | `test_collision.c` | ✅ resuelto |
| A4 | Movimiento por contador de *frames* → no determinista | `update(dt)` con float `fx/fy` + clamp `dt∈[0,1]` | `enemies_move_independent_of_frame_count` | ✅ resuelto (Fase 2) |
| A5 | Intervalo de *spawn* fijo al arrancar el hilo | Intervalo por nivel en `difficulty.c` | `spawn_respects_interval` | ✅ resuelto |
| A6 | `GAME_WIN_LEVEL` evaluado repetitivamente dentro de `checkCollisions` | Condición de victoria evaluada en `game_update` | `test_wincondition.c` / `test_state_machine.c` | ✅ resuelto |
| A7 | Nave sin clamp: usaba posición bruta del ratón | Clamp en `game_set_ship` | `ship_bounded_in_x` | ✅ resuelto |
| A8 | Sorteo de tipos frágil (`rand()%2` + contadores por nivel) | Generación procedural en `difficulty.c` | `test_difficulty.c` | ✅ resuelto |

> Nota: el original además tenía *races* y fugas en la capa X11/SDL (p. ej.
> `XQueryFont` sin `XFreeFont`, lecturas de `game_state` sin mutex). Se resuelven
> por arquitectura (single-thread + sin X11/SDL), no por parche.

## B. Typos y nomenclatura

- `drawWinnLevelOne/Two/Three`, `drawWinn` ("Winn") → el render C se elimina; fijar
  convención clara en el engine (p. ej. `victory_*`).
- "projectil" → en inglés: `projectile`.
- **Política de idioma: TODO en inglés** (identificadores, comentarios y API WASM;
  estándar C y consumida por JS). Se aplica también al código ya migrado.
- Estilo:
  - funciones/variables en `snake_case`;
  - tipos en `PascalCase`;
  - constantes y macros en `UPPER_CASE`;
  - 0 *warnings* (`-Wall -Wextra -Werror`).

## C. Deuda leve (refactors a aplicar en la Fase 1)

- **Constantes mágicas** → al modelo data-driven/`balance`: `±15`, `±40`,
  `HEIGHT-98`, spawn `2/3/4 s`, velocidad `5`, `6.5*sin(...)`, frame counters.
- Velocidades/movimiento ondulado → parámetros por enemigo y por nivel.

## D. Higiene y memoria

- Engine C puro sin fugas/UB: `make test` con **ASan + UBSan**
  (`-fsanitize=address,undefined`).
- Verificar límites de arrays en `game_t` (`MAX_ENEMIES`, `MAX_PROJECTILES`).
- `game_create`/`game_destroy` sin leaks (validado por ASan en los tests).

## E. Validación y criterio de salida

- Mapeo defecto → test (referencia a `FASE2_TESTS.md`) — **completado en la
  Fase 2** (50 tests verdes):
  - A1 → `test_collision.c` ("múltiples balas, sin doble daño").
  - A2 → `test_scheduling.c` (RR intercala tipos).
  - A3/A7 → `test_collision.c` / `test_projectiles.c` (áreas, nave acotada).
  - A4/A5 → `test_movement.c` / `test_difficulty.c` (dt, intervalos).
  - A6 → `test_wincondition.c` / `test_state_machine.c` (victoria en `game_update`).
  - A8 → `test_difficulty.c` (generación procedural).
- Salida: `make test` verde con sanitizers + 0 *warnings* — ya cumplido
  (`-Wall -Wextra -Werror`, ASan+UBSan, 0 fugas; cobertura 95.86%).
- Pendiente real para Fase 3: §B auditoría final de nomenclatura, §C limpieza
  de constantes mágicas residuales y robustez/fuzzing de casos borde.

## Relacionado

- [`POLISH_PLAN.md`](./POLISH_PLAN.md) — plan general (las fases).
- [`FASE2_TESTS.md`](./FASE2_TESTS.md) — los tests que validan los defectos.
- [`GAME_DESIGN.md`](./GAME_DESIGN.md) — decisiones del engine (Fase 1).