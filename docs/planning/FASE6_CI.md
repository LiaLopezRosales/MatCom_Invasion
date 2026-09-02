# Fase 6 — CI + deploy GitHub Pages (detalle)

> Documento de soporte del [`POLISH_PLAN.md`](./POLISH_PLAN.md). Define el
> pipeline de CI/CD: test + build + deploy a GitHub Pages via GitHub Actions.

## Pipeline (`.github/workflows/deploy.yml`)

### Triggers

| Evento | Acción |
|---|---|
| Push a `main` | test + build + **deploy** a GitHub Pages |
| PR (cualquier branch) | test + build (sin deploy) |

### Steps (en orden)

1. **Checkout** del repo.
2. **Setup Node.js** (`actions/setup-node`) con `cache: 'npm'`.
3. **Setup + cache emsdk** (`actions/cache`): key basada en versión de emsdk.
   - Instalación en el home del runner (sin sudo), reproduciendo el entorno local.
   - Primera ejecución: ~5-10 min. Con cache: ~1-2 min.
4. **`make native`** — compila la lógica C nativamente (gcc).
5. **`make test`** — corre Criterion (tests unitarios + sim headless) con
   `-fsanitize=address,undefined` (ASan/UBSan, Fase 3). **Si falla → se detiene**
   (no se continúa con build ni deploy).
6. **`make wasm`** — compila la lógica a WASM (emcc) en `web/public/`.
7. **`cd web && npm install && npm run build`** — Vite construye `dist/`.
8. **Upload artifact** (`actions/upload-pages-artifact`): sube `dist/` como
   artifact desplegable.
9. **Deploy** (`actions/deploy-pages`): publica a GitHub Pages.

### Permisos del workflow

```yaml
permissions:
  pages: write
  id-token: write
```

### Ambiente de deploy

```yaml
environment:
  name: github-pages
  url: ${{ steps.deployment.outputs.page_url }}
```

## Caching

- **emsdk**: `actions/cache` con key `emsdk-<version>`; path: `~/emsdk`.
- **npm**: `actions/setup-node` con `cache: 'npm'`.
- Ambos aceleran runs subsiguientes (~5-8 min menos por run).

## Badge en README

GitHub Actions provee una badge URL:
```markdown
![CI](https://github.com/<user>/<repo>/actions/workflows/deploy.yml/badge.svg)
```
Mostrar en el README como señal de rigor del pipeline.

## Prerequisitos

- **Fase 1 completa**: repo reestructurado a `src/`+`web/`+`tests/`, binario
  `invasion` retirado, Makefile con objetivos `native`, `wasm` y `test`.
- **Fase 2 completa**: tests Criterion funcionando con `make test`.
- **Fase 5 completa**: frontend construible con `cd web && npm run build`.
- **Cuenta de GitHub** con Pages habilitado (Settings → Pages → Source: GitHub
  Actions).

## Validación

- Push a `main` → pipeline verde → demo live en GitHub Pages.
- PR → pipeline verde (test + build) sin deploy.
- Test falla → pipeline rojo, no se deploya.
- Badge visible en el README con estado correcto.

## Estado de implementación

- [x] Workflow `.github/workflows/deploy.yml` definido (triggers main+PRs, build
      C/WASM/web, test con ASan/UBSan, deploy desde Actions a Pages).
- [x] `web/vite.config.ts` crea el build con `base: './'` — los assets de
      `web/public/` se referencian con rutas relativas (`assets/...`) en
      `render.ts`/`main.ts`, para que carguen bajo el subpath de GitHub Pages.
- [x] Badge de CI en el README (repo `LiaLopezRosales/MatCom_Invasion`).
- [ ] **Pendiente (se cierra al primer push a `main`):** ejecución real del
      pipeline en Actions, habilitar Pages en Settings, y añadir screenshots.

## Relacionado

- [`POLISH_PLAN.md`](./POLISH_PLAN.md) — las fases de ejecución.
- [`FASE5_FRONTEND.md`](./FASE5_FRONTEND.md) — el frontend que se deploya.
- [`FASE2_TESTS.md`](./FASE2_TESTS.md) — los tests que el CI ejecuta.
- [`FASE3_QUALITY.md`](./FASE3_QUALITY.md) — sanitizers incluidos en `make test`.