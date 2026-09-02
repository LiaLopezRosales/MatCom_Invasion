# Evaluación, benchmarking y roadmap de mejora a nivel profesional

Guía **genérica** para elevar un proyecto académico (aunque cumpla el estándar de
`PORTFOLIO_QUALITY.md`) a un nivel **profesional actual** con el que destaque en la
búsqueda de trabajo. Muchos proyectos académicos son correctos pero "básicos": este
proceso los compara con referencias profesionales reales y produce un plan de
elevación accionable.

> Complementa a [`PORTFOLIO_AGENTS.md`](./PORTFOLIO_AGENTS.md) (cómo se trabaja) y a
> [`PORTFOLIO_QUALITY.md`](./PORTFOLIO_QUALITY.md) (el estándar mínimo). Este documento
> define el proceso para **ir más allá del estándar**.

---

## 1. Propósito y alcance

- Determinar el **estado real** del proyecto (código, features, funcionalidades),
  sin idealizarlo.
- Compararlo contra **proyectos similares ya profesionales** del mismo dominio para
  saber qué debería tener.
- Producir un **roadmap de mejora priorizado** hacia nivel profesional actual.
- Proponer **features diferenciadoras** que hagan destacar a quien lo presenta
  ("curriculum-impact"), realistas para el stack y el tiempo disponible.

Norma transversal: **todo el análisis se basa en evidencia** (código, tests, fuentes
con URL). No se opina sin citar.

## 2. Fase A — Estado actual (hechos)

1. **Código**: arquitectura real, deuda, complejidad, puntos débiles (god-objects,
   código inalcanzable, errores silenciosos). Evidencia con `archivo:línea`.
2. **Features y funcionalidades**: inventario de lo que existe **de verdad**
   (verificado en código y tests, no lo que promete el README). Incluir limitaciones
   ("funciona a medias", casos no soportados).
3. **CV del proyecto**: qué demuestra hoy (lenguaje/stack, patrones, tests, CI, docs).
4. **Puntuación** con la rúbrica de `PORTFOLIO_QUALITY.md`.

Salida: sección "Estado actual" dentro del documento de evaluación del proyecto.

## 3. Fase B — Benchmarking contra proyectos profesionales

1. **Investigación web**: buscar proyectos reales/profesionales del **mismo dominio**
   (mismo tipo de producto y mismo stack cuando sea posible). Mínimo 3 referencias.
2. **Criterios de comparación** (tabla):
   - Funcionalidades clave y cobertura del problema.
   - Experiencia de uso / DX (documentación, ejemplos, demo viva).
   - Arquitectura, calidad (tests, cobertura, CI, linting).
   - Publicación y alcance (paquete publicado, demo online, README de nivel).
3. **Gaps** identificados: qué tiene "el profesional" que el proyecto académico no
   tiene. Ordenar por valor (impacto en la impresión) y viabilidad (esfuerzo para
   implementarlo en este stack).
4. Incluir fuentes (URL) para que el usuario pueda verificar y para que las features
   propuestas no parezcan inventadas.

Salida: tabla "Mi proyecto vs A vs B vs C" y lista de gaps priorizada.

## 4. Fase C — Roadmap hacia nivel profesional

1. **Nivel objetivo explícito** (p. ej. "v1 profesional": instalable/exportable,
   demo reproducible, docs completas, cobertura y CI, tests de calidad).
2. **Hitos priorizados** (MoSCoW o P0/P1/P2) combinando impacto para el reclutador
   con esfuerzo. Cada hito:
   - Entregable verificable (comando, test, demo, captura).
   - Cambios concretos (archivos/áreas) sin escribir el código aún.
   - Señal de "hecho" objetiva.
3. **Trazabilidad**: cada hito ataca uno o más gaps de las Fases A/B.
4. Orden pensado para que cada hito habilite al siguiente.
5. Separar explícitamente lo que **entra en el plan** de lo que va a **backlog**
   (fuera de alcance por esfuerzo/valor).

Salida: roadmap con hitos priorizados y trazados, aprobado por el usuario antes de
ejecutarse (los hitos se ejecutan luego siguiendo `PORTFOLIO_AGENTS.md`).

## 5. Fase D — Features destacables para la búsqueda de trabajo

Generar (como parte del roadmap o del backlog) un listado de features de alto
impacto-curriculum. Para cada idea:

| Campo | Qué escribir |
|---|---|
| **Feature** | Nombre corto y objetivo |
| **Por qué destaca** | Qué skill en demanda demuestra (DX, performance, testing, publicación, CI, empaquetado...) |
| **Impacto** | Cómo eleva la impresión del repo (demo, README, entrevista) |
| **Esfuerzo/riesgo** | Estimación honesta y riesgos para el stack |
| **Preguntas que habilita** | De qué podrá hablar el candidato en una entrevista si la implementa |

Criterios de selección:
- **2–4 features realistas** como hitos; el resto a backlog.
- Preferir las que demuestren **skills buscados** (ej.: integración continua real,
  publicación de paquete/demo, tests de calidad, performance medible, DX del API).
- **No prometer** cosas inverosímiles para el stack ni features que el candidato no
  pueda mantener/defender.

Salida: propuesta de features diferenciadoras con las 2–4 seleccionadas integradas
en el roadmap como hitos P0/P1.

## 6. Entregables y normas del proceso

- El resultado del análisis es un documento de evaluación+roadmap **dentro del repo
  que se pule** (p. ej. `docs/ROADMAP_PROFESIONAL.md` o el nombre que siga la
  convención del repo), redactado en el idioma del repo.
- Presentar el análisis y el roadmap al usuario y **pedir aprobación antes de
  ejecutar** cualquier hito.
- Si no existen referencias profesionales del dominio: dejarlo explícito y basar el
  roadmap en criterios generales de calidad de `PORTFOLIO_QUALITY.md`.
- Cualquier cambio de código que execute después este proceso sigue las reglas de
  `PORTFOLIO_AGENTS.md` (auditoría → plan → verificación → commits por lote).