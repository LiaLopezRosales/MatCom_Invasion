# Fase 7 — Governance y verificación final (detalle)

> Documento de soporte del [`POLISH_PLAN.md`](./POLISH_PLAN.md). Define la
> última fase: limpieza del repo, documentación, LICENSE y verificación contra el
> DoD de [`PORTFOLIO_QUALITY.md`](../portfolio/PORTFOLIO_QUALITY.md).

## 1. LICENSE

- Archivo `LICENSE` con licencia **MIT** en la raíz del repo.
- Referenciado en el README (sección de licencias/créditos).

## 2. Limpieza del repo

### Eliminar (git preserva la historia)

- `Game_Logic/` (directorio completo, post-migración a `src/`).
- `Visual/` (directorio completo, post-migración render a web).
- `main.c` (entry point original, reemplazado por el runner CLI en `src/`).
- `invasion` (binario compilado trackeado en git).
- PNGs originales (licencia desconocida, sustituidos por Kenney CC0):
  `ship.png`, `creature_T1-sheet.png`, `creature_T2-sheet.png`,
  `galaxy.jpeg`, `night_sky.png`.

### `.gitignore` completo

```
# Build artifacts
dist/
*.o
*.wasm

# Dependencies
node_modules/

# Emscripten (instalado localmente)
emsdk/

# IDE / cache
.vscode/
.cache/
.cache/
*.swp
*~

# Binarios
invasion
```

### `.editorconfig`

```ini
root = true

[*]
indent_style = space
indent_size = 2
end_of_line = lf
charset = utf-8
trim_trailing_whitespace = true
insert_final_newline = true
```

## 3. Trackear docs

- `docs/portfolio/*.md` → commit a git (PORTFOLIO_AGENTS.md, PORTFOLIO_QUALITY.md,
  PORTFOLIO_ROADMAP.md).
- `docs/planning/*` → commit a git (todos los docs de planificación creados durante
  el proyecto).

## 4. AGENTS.md (completo)

Estructura del AGENTS.md:

1. **Project overview**: qué es el juego, por qué es interesante.
2. **Architecture**: C (engine puro, determinista, single-thread) → WASM
   (Emscripten) + TypeScript/Vite (frontend web, canvas, Web Audio).
3. **Repo structure**: `src/`, `web/`, `tests/`, `docs/`.
4. **Setup**: prerequisites (gcc, make, node, npm, emsdk sin sudo en home).
5. **Build commands**: `make native`, `make wasm`, `make test`, `make coverage`,
   `cd web && npm install && npm run dev`.
6. **Coding conventions**: inglés, snake_case/PascalCase/UPPER_CASE,
   `-Wall -Wextra -Werror`, conventional commits.
7. **CI/CD**: GitHub Actions (test + build + deploy a Pages), triggers, cache.
8. **Key decisions**: links a `docs/planning/` (ENVIRONMENT_DECISION, GAME_DESIGN,
   FASE2-7).

## 5. Verificación DoD (checklist final)

Basado en [`PORTFOLIO_QUALITY.md`](../portfolio/PORTFOLIO_QUALITY.md) §5:

| # | Criterio | Verificación |
|---|---|---|
| 1 | Build 0 errores + 0 warnings | `make native` y `cd web && npm run build` en clone limpio |
| 2 | Tests 100% verdes | `make test` (Criterion + ASan/UBSan) |
| 3 | Cobertura gateada | `make coverage` → umbral configurado y superado |
| 4 | `git status` limpio | Verificar post-commit |
| 5 | Quick-start reproducible | Seguir el README desde un clone limpio |
| 6 | Links markdown válidos | Verificar todos los enlaces internos |
| 7 | README con datos reales | Screenshots, demo link, conteo de tests, badge CI |
| 8 | AGENTS.md existente y completo | Revisar estructura y contenido |
| 9 | LICENSE presente | MIT, referenciado en README |
| 10 | `.gitignore` y `.editorconfig` | Existentes y coherentes |
| 11 | Legacy code eliminado | `Game_Logic/`, `Visual/`, `main.c`, PNGs, `invasion` |
| 12 | Docs trackeados | `docs/portfolio/*` y `docs/planning/*` en git |

## 6. Commits

- No reorganizar historial. Los commits se hacen con convenciones durante la
  implementación. Esta fase solo verifica que no haya commits rotos y que el
  `git status` esté limpio.

## Relacionado

- [`POLISH_PLAN.md`](./POLISH_PLAN.md) — las fases de ejecución.
- [`PORTFOLIO_QUALITY.md`](../portfolio/PORTFOLIO_QUALITY.md) — el DoD contra el
  que se verifica.
- [`PORTFOLIO_AGENTS.md`](../portfolio/PORTFOLIO_AGENTS.md) — convenciones de
  trabajo del repo.
- [`FASE6_CI.md`](./FASE6_CI.md) — el pipeline que validan los tests en CI.