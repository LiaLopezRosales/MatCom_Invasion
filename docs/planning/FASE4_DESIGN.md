# Fase 4 — Dirección de arte, UI/UX y audio (spec) (detalle)

> Documento de soporte del [`POLISH_PLAN.md`](./POLISH_PLAN.md). Define las
> **elecciones de dirección** (no-lógica) de la Fase 4: tema visual, assets,
> UI/UX y audio. La **implementación** del render, HUD, menús y audio vive en la
> Fase 5 (frontend). El contenido **lógico** está en
> [`FASE4_FEATURES.md`](./FASE4_FEATURES.md).

## 1. Tema: "neón espacial moderno"

- Cosmos oscuro con acentos **neón** (cian / magenta / violeta).
- Estética **moderna** (no retro pixel-art): formas limpias, glow, trazos afilados.
- Sprites de *Space Shooter Redux* (estilo flat color arcade) **teñidos y glow-eados**
  por el render para encajar con la paleta neón.

## 2. Assets (Kenney, CC0)

- **Fuente:** Kenney (kenney.nl), packs **CC0** — sin atribución obligatoria.
  - **Space Shooter Redux**: nave, enemigos, proyectiles, power-ups, **fondo**.
- **Política de licencias: solo CC0** → README sin sección de créditos obligatoria;
  por buena práctica se cita "Kenney (CC0)" con enlace en el README.
- Los **PNG originales** del juego (galaxy.jpeg, ship.png, creature_T1/T2-sheet.png,
  night_sky.png — licencia desconocida) se **sustituyen** por el pack.
- **Tipografía:** Google Fonts, licencia **OFL** — p. ej. **Orbitron** (sci-fi
  geométrica) para títulos/HUD.
- Neón por render, no por el arte: **glow**, partículas de explosión, estelas de
  proyectiles y **tint/filtros canvas** sobre los sprites limpios.

## 3. UI/UX (spec)

### Pantallas y jerarquía

```
Mode Select ──▶ Playing ──┬─▶ Pause
        │                 ├─▶ Game Over (0 vidas / enemigos al fondo)
        │                 └─▶ Victory (condición del modo cumplida)
        └─▶ How to Play (instrucciones/controles)
```

- Overlays modales (menús sobre el canvas del juego).
- `Mode Select`: 5 modos legibles y diferenciados.

### HUD (barra superior)

- **Vidas** (3), **puntaje**, **nivel**, **modo** y **high score persistente por modo**.
- Tipografía Orbitron; iconos pequeños del pack (vidas = icono nave).

### Controles (concepto)

- Nave sigue el **puntero** (X/Y); **clic izquierdo** dispara; teclas para **pausa**
  (y soporte táctil como stretch). Validación de input es Fase 5.

## 4. Audio (Web Audio API, sintetizado, sin archivos)

- **Música**: loop synth por estado — tema de **menú** y tema de **partida**
  (intensidad mayor en oleadas Waves).
- **SFX**: disparo, explosión, daño, power-up, game over, victoria, click de UI.
- Todo generado con Web Audio API (osciladores/env compensated sin ficheros ni
  licencias). Implementación en Fase 5.

## 5. Validación

- Spec consumido por la Fase 5 (render/HUD/menús/audio). Implementación detallada
  en [`FASE5_FRONTEND.md`](./FASE5_FRONTEND.md).
- Los valores de game feel (4.1) y la estética final se verifican con **juego
  manual** en la Fase 5; los valores lógicos se validan con la sim (Fase 2).

## Relacionado

- [`POLISH_PLAN.md`](./POLISH_PLAN.md) — las fases de ejecución.
- [`FASE4_FEATURES.md`](./FASE4_FEATURES.md) — contenido de juego (lógica).
- [`FASE2_TESTS.md`](./FASE2_TESTS.md) — tests que validan el contenido lógico.
- [`ENVIRONMENT_DECISION.md`](./ENVIRONMENT_DECISION.md) — ADR del stack híbrido.
- [`FASE5_FRONTEND.md`](./FASE5_FRONTEND.md) — implementación del frontend y
  README (Fase 5).