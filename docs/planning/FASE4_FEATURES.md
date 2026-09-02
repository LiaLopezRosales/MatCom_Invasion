# Fase 4 — Features: contenido de juego (lógica) (detalle)

> Documento de soporte del [`POLISH_PLAN.md`](./POLISH_PLAN.md). Define el contenido
> **lógico** de la Fase 4 (los temas que el usuario listó: scheduling, condición de
> victoria, tipos de enemigos y otros detalles). La **dirección de arte/UI/audio**
> está en [`FASE4_DESIGN.md`](./FASE4_DESIGN.md). Solo lógica C: los tests de la
> Fase 2 y la sim headless validan todo lo decidido aquí.

## 1. Modos de scheduling (5)

Se conservan los 3 modos originales **pulidos** (criterios explícitos y
deterministas a partir de la semilla) y se añaden 2 nuevos. El dispatcher
extensible de la Fase 1 los admite.

| Modo | Comportamiento |
|---|---|
| Progressive (SJF) | Orden de aparición por vida ascendente (de más débil a más fuerte) |
| Alternate (RR) | Alternancia real por tipo (intercala tipos, independiente de la vida) |
| Random (FIFO) | Orden/mezcla aleatoria determinista (semilla) |
| **Waves** (nuevo) | Los enemigos llegan en **oleadas**: ráfagas de spawn con pausas entre ellas |
| **Formations** (nuevo) | Spawn en **formación** (fila/columna) que desciende en bloque |

> **Refactor de arquitectura (depende de Fase 1):** el modelo de aparición pasa de
> *"secuencia de tipos"* a **eventos de spawn** `{tipo, retardo, posición X}`. Los
> modos generan la cola/plan de eventos; `difficulty.c` sitúa los valores.

## 2. Condiciones de victoria (5 tipos de `WinCondition`)

`WinCondition { tipo, parámetros }` desacoplada del scheduling (Fase 1), cada
condición es un predicado determinista evaluado en `game_update` (no en el bucle de
colisiones). Mapping por defecto por modo:

| Modo | Condición | Parámetros |
|---|---|---|
| Progressive | `SCORE_THRESHOLD` | Puntos a alcanzar |
| Alternate | `KILL_X_WITHIN_TIME` | X enemigos en ≤T segundos |
| Random | `SURVIVAL_TIME` | Sobrevivir T segundos |
| Waves | `SURVIVE_N_WAVES` | Completar N oleadas |
| Formations | `REACH_LEVEL` | Completar el nivel N (cada nivel = una formación) |

## 3. Tipos de enemigos (data-driven, 5)

Atributos por tipo: `{vida, velocidad Y, patrón de movimiento, amplitud/frecuencia,
puntaje, tamaño}`. Parámetros iniciales (game feel — se afinan en Fase 5):

| Tipo | Vida | Velocidad Y | Movimiento | Puntaje |
|---|---|---|---|---|
| Grunt | 3 | media | descenso recto con sine leve | 50 |
| Tank | 8 | lenta | sine ancho | 150 |
| Dart | 2 | rápida | diagonal | 100 |
| Hover | 4 | media | **2 fases**: deriva lateral → zambullida | 120 |
| Swarm | 1 | rápida | sine estrecho y frecuente | 25 |

Todos los movimientos son tiempo-real (`dt`) y deterministas (semilla). *Stretch*
pendiente (fuera de alcance): **Splitter** (se divide al morir).

## 4. Vidas, respawn e inmunidad

- **3 vidas**. La nave destruida (colisión con enemigo) resta una vida.
- **Respawn** con posición inicial y **inmunidad breve** (timer basado en `dt`).
- 0 vidas → `GAME_OVER`. Los enemigos que llegan al fondo → `GAME_OVER`
  (comportamiento original, se conserva).

## 5. Power-ups (2 minimales)

- Caen de enemigos destruidos (probabilidad de drop), caída + pickup por la nave,
  efecto con vida limitada:
  - **Double Shot**: ráfaga doble (~8 s).
  - **Shield**: absorbe 1 golpe (interacción definida con la inmunidad del respawn).
- Entidades propias en el engine (drop con posición/velocidad/lifetime), validadas
  por tests.

## 6. Puntaje y high score

- **Puntaje** = suma por tipo destruido.
- **High score de sesión**: el engine trackea el mejor puntaje de la sesión en el
  snapshot; la **persistencia** (localStorage) es responsabilidad del frontend
  (Fase 5).

## 7. Dificultad procedural (`difficulty.c`)

- Genera por nivel: **intervalo de spawn, velocidad global, mezcla de tipos**.
- **Clampeo**: nº enemigos nunca > `MAX_ENEMIES` (test Fase 2); saturación en
  niveles altos.
- Valores iniciales de game feel; fine-tune en la Fase 5 con juego manual.

## 8. Validación

- Extiende `FASE2_TESTS.md`: eventos de spawn (tipo+retardo+posición), cada
  `WinCondition`, cada arquetipo/patrón de movimiento, power-ups
  (drop/pickup/efecto/expiración), vidas/respawn/inmunidad.
- Sim headless con semilla fija por modo, incluyendo los 2 modos nuevos.

## Relacionado

- [`POLISH_PLAN.md`](./POLISH_PLAN.md) — las fases de ejecución.
- [`FASE4_DESIGN.md`](./FASE4_DESIGN.md) — dirección de arte, UI/UX y audio.
- [`FASE2_TESTS.md`](./FASE2_TESTS.md) — cobertura de tests que valida este contenido.
- [`GAME_DESIGN.md`](./GAME_DESIGN.md) — decisiones del engine (Fase 1).