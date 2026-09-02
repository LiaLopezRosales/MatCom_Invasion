# Pautas de trabajo del agente — portafolio

Guía **genérica e independiente del dominio**. Aplica cuando el objetivo sea dejar un
proyecto académico "listo para portafolio" (o elevarlo a nivel profesional). No
presupone ningún lenguaje, framework ni tipo de producto: cada sección se interpreta
sobre el repo concreto con el que se trabaja.

> Complementa a [`PORTFOLIO_QUALITY.md`](./PORTFOLIO_QUALITY.md) (el estándar que
> define "listo") y a [`PORTFOLIO_ROADMAP.md`](./PORTFOLIO_ROADMAP.md) (cómo elevar un
> proyecto básico). Este documento define **cómo se trabaja**; los otros dos definen
> **qué se persigue**.

---

## 1. Propósito y alcance

- Antes de operar, **leer** el `README`, el `AGENTS.md` (si existe) y los docs de
  planificación del repo para conocer: comandos reales de build/test, convenciones,
  estado declarado y deudas conocidas.
- Trabajar **solo sobre el código del proyecto**, sin asumir herramientas
  inexistentes ni versiones que no estén disponibles en el entorno.
- El resultado del trabajo debe ser un repo que un reclutador técnico pueda abrir,
  entender y ejecutar por sí mismo.

## 2. Flujo de trabajo (fases)

| Fase | Qué hace el agente | Regla de salida |
|---|---|---|
| **0. Auditoría** | Explorar solo-lectura: estructura raíz, docs, `git log`, suite de tests, CI, dependencias. Detectar red flags y hallazgos de estado real (con evidencia `archivo:línea`). | Nota de hallazgos |
| **1. Plan** | Presentar plan escrito con pasos, cambios previstos, archivos a tocar y verificaciones. | Aprobación previa del usuario |
| **2. Decisiones** | Preguntar al usuario las decisiones que cambien estructura, API pública, alcance o convenciones **antes** de ejecutarlas. | Opciones con recomendación |
| **3. Ejecutar** | Cambios en pasos pequeños y verificables, un tema a la vez. | Cada paso inconcluso queda documentado |
| **4. Verificar** | Correr build + suite completa (y lint/typecheck si existen). CI debe permanecer verde. | 0 errores, tests verdes, repo limpio |
| **5. Registrar** | Actualizar los docs del repo si cambió el estado real (tablas de deuda, hitos, backlog, convenciones). | Docs coherentes con el código |

## 3. Reglas de commit

- Usar **conventional commits** (`feat:`, `fix:`, `refactor:`, `docs:`, `chore:`,
  `build:`, `test:`, `perf:`), opcionalmente con ámbito (`fix(parser)`).
- **Un asunto por commit**; no juntar cambios no relacionados. El mensaje debe
  explicar el *por qué*, no solo el *qué*.
- Commit **inmediato** tras cada lote coherente de cambios; no dejar trabajo
  sin commitear de un día para otro.
- **Nunca commitear, pushear ni crear PRs sin que el usuario lo pida.**
- Antes de cada commit: revisar `git status` y `git diff`; no incluir secretos,
  artefactos de build ni archivos generados.

## 4. Verificación obligatoria

- **No inventar comandos**: buscarlos en el README, AGENTS.md y CI del repo. Si no
  están, preguntar al usuario en vez de adivinar.
- Correr las verificaciones del repo (build, tests, lint, typecheck) antes de
  declarar terminado cualquier cambio; la suite debe quedar **verde completa**.
- Tras mover/renombrar archivos o docs, comprobar que **ningún link o referencia
  quedó roto** (README, AGENTS, docs internos, CI).
- Al terminar el trabajo: el `git status` debe quedar limpio (o con solo los
  cambios que el usuario pidió dejar sin commitear).

## 5. Precauciones técnicas

- **Renombres seguros**: antes de renombrar un identificador público, verificar que
  no dependa de reflexión, `nameof`, serialización ni mapeo por string. Aplicar
  reemplazos ordenados "longest-first" para evitar colisiones parciales
  (p. ej. `Declared_FuncName` antes que `Declared_Func`) y **revisar el diff**
  antes de continuar.
- **Preservar historia**: mover directorios con `git mv` (no copiar+pegar); no
  renombrar dentro de árboles archivados (legacy se conserva tal como está).
- **Reproducibilidad**: los assets referenciados por el README (ejemplos, imágenes,
  scripts) deben estar realmente trackeados en git; el quick start debe funcionar
  desde un **clone limpio**.
- **Convenciones del repo**: respetar lo existente (nullable, XML docs, analyzers
  advisory, estilo de nombrado); no introducir dependencias nuevas sin sentido.
- **No romper seguridad**: nunca registrar/exponear secretos, cadenas de conexión
  ni claves en el repo.

## 6. Comunicación y decisiones

- Respuestas **tersas**, en el idioma del usuario; resumir lo hecho al terminar
  (qué cambió, commits, verificaciones, hallazgos pendientes).
- Cambios de ámbito (reestructurar repo, renombrar API pública, mover hitos,
  descartar features) → **preguntar antes** con opciones y recomendación.
- Reportar hallazgos que el usuario deba decidir por sí mismo (p. ej., destino de
  ítems históricos sin hito, features diferidas).

## 7. Entorno y gotchas

- Verificar el entorno antes de lanzar comandos (SDK instalado, `PATH`, versiones,
  paquetes disponibles); si hay un gotcha conocido, **documentarlo en el AGENTS.md**
  local (p. ej. `export PATH="$HOME/.dotnet:$PATH"`) para que no se repita.
- Si un paquete/versión no está disponible y NuGet "resuelve otra", fijar la versión
  real resuelta y eliminar la advertencia en vez de ignorarla.