# Especificación de profesionalidad para portafolio (estándar + checklist)

Define, de forma **genérica**, qué significa que un proyecto académico esté "listo
para portafolio". Independiente del dominio: los criterios se aplican al repo
concreto. Sirve como objetivo de trabajo, checklist de auditoría y rúbrica de
auto-evaluación.

> Complementa a [`PORTFOLIO_AGENTS.md`](./PORTFOLIO_AGENTS.md) (cómo se trabaja) y a
> [`PORTFOLIO_ROADMAP.md`](./PORTFOLIO_ROADMAP.md) (cómo elevar un proyecto básico a
> nivel profesional). Este documento es **el estándar** contra el que se mide.

---

## 1. Principio rector

Se evalúa como lo haría **un reclutador técnico**: abrirá la raíz del repo, leerá el
README, mirará tests/badges/historial, probará el quick start y quizá abra 2 o 3
archivos de código. Si alguno de esos pasos falla o confunde, el proyecto "no está
listo" aunque el código funcione.

## 2. Dimensiones y criterios

Cada dimensión se puntúa 1–5 en la rúbrica (sección 4).

### D1 · README
- Value proposition clara ("¿qué es y por qué es interesante?"), no solo "una app de X".
- Quick start **reproducible desde un clone limpio** (cada archivo referenciado está
  trackeado; los comandos son reales).
- Screenshots/capturas reales del funcionamiento.
- Tabla o sección de features y de arquitectura.
- Datos de tests/CI visibles (cuenta de tests, gate de cobertura, badge).
- License (link o archivo).

### D2 · Higiene del repo
- La raíz muestra **el proyecto activo**; el histórico/archivado vive en una carpeta
  aparte (`legacy/`), con historia preservada y sin mensajes confusos.
- No hay artefactos de build/IDE trackeados (`bin/`, `obj/`, `.vs/`, `.cache/`, ...).
- `.gitignore` y `.editorconfig` existentes y coherentes.
- Docs de planificación/migración fuera de la raíz (`docs/planning/` o similar).
- Links de README/AGENTS/docs internos válidos.

### D3 · Código
- Nullable reference types (o equivalente del lenguaje) activado.
- XML doc / documentación breve en la API pública.
- Analizadores estáticos configurados (al nivel *advisory* no bloqueante).
- **Cero typos en identificadores** y en nombres visibles (clases, enums, namespaces, archivos).
- Nombres con significado; patrones de diseño coherentes (Clean Architecture,
  Visitor, monadas, DI...) en vez de god-objects.
- Sin dependencias innecesarias; el dominio bien aislado si corresponde.

### D4 · Tests
- Suite que mezcle: unitarios + caracterización/integración del pipeline completo
  (+ UI headless si hay interfaz).
- Casos de borde y combinaciones (no solo "el método devuelve algo").
- Cobertura **medida** y, si hay CI, **gateada** con un umbral.
- Los tests son legibles y usan helpers simples; no dependen de la red ni de estado global.

### D5 · CI/CD
- Build + suite de tests en cada push/PR.
- (Opcional pero recomendado) job de cobertura que falle bajo el umbral.
- Badge de CI (y de license/coverage) visible en el README.

### D6 · Historial git
- Conventional commits, mensajes que expliquen el *por qué*.
- Commits pequeños por asunto (sin batches artificiales).
- Actividad reciente (proyecto vivo) y autoría clara.

### D7 · Documentación del proyecto
- `AGENTS.md` (o equivalente) local con arquitectura, convenciones y comandos reales.
- Plan/roadmap de implementación con estado real actualizado (hitos, backlog, deudas).
- Registros de decisiones importantes (migraciones, sprints de deuda, auditorías).

### D8 · Señales de profesionalismo
- LICENSE definida (p. ej. MIT) y mencionada en README.
- Repo con "por encima del promedio": README visual, demos/ejemplos ejecutables,
  estructura limpia, badges.
- Narrativa lista para una entrevista: qué habilita cada decisión técnica del repo.

## 3. Red flags y tratamiento

| Red flag | Mitigación |
|---|---|
| Typos en identificadores públicos | Renombrar con verificación de acoplamiento (ver `PORTFOLIO_AGENTS.md` §5) |
| Raíz con el proyecto "real" mezclado con histórico | Archivar lo legacy en `legacy/`, apuntar el README al proyecto activo |
| Quick start roto en clone limpio | Trackear los assets referenciados; probar el flujo desde cero |
| Archivos monstruosos (/god-files) | Si es coherente (switch de un intérprete) dejarlo y preparar la respuesta; si no, descomponer |
| Gap grande en la historia de git | No ocultarlo; tener una narrativa clara de retoma/refactor |
| Cobertura en el umbral mínimo | Presentar el umbral como piso de sanidad, no como techo |
| Comentarios/README mezclados en varios idiomas | Consistencia: elegir el idioma principal del README y respetarlo |

## 4. Rúbrica de auto-evaluación

Puntúa cada dimensión D1–D8: **1** = ausente, **3** = adecuado, **5** = excelente
para el nivel del proyecto.

- **READY** si: ninguna dimensión < 3 **y** no hay red flags mayores **y** se cumple
  el Definition of Done (sección 5).
- Si alguna dimensión está en 1–2, es el primer foco de trabajo. Si todas están en
  3–4 pero el proyecto es "básico" y no destacaría → aplicar
  [`PORTFOLIO_ROADMAP.md`](./PORTFOLIO_ROADMAP.md) (elevación a nivel profesional).

## 5. Definition of Done (verificable)

- Build: **0 errores** (y sin advertencias de versión/paquete pendientes).
- Suite de tests: **100% verde** (todos los tests de todas las suites).
- Cobertura: gate de CI cumplido (umbral configurado y por encima del mínimo).
- `git status` limpio (salvo lo que el usuario pidió dejar sin commitear).
- Quick start reproducible desde un **clone limpio**.
- Links de README/AGENTS/docs sin romper.
- README con **datos reales** (número de tests actual, fechas, screenshots reales).