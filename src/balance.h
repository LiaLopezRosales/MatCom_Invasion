#ifndef BALANCE_H
#define BALANCE_H

/* ── Player ─────────────────────────────────────────────────────── */
#define INITIAL_LIVES          3
#define INVULN_DURATION        2.0f    /* seconds after a ship hit */

/* ── Ship spawn ─────────────────────────────────────────────────── */
#define SHIP_SPAWN_Y_OFFSET    100     /* ship.y = SCREEN_HEIGHT - this */

/* ── Projectile ─────────────────────────────────────────────────── */
#define FIRE_Y_OFFSET          15      /* projectile spawns above ship */

/* ── Enemy spawn ────────────────────────────────────────────────── */
#define ENEMY_SPAWN_Y          (-30)   /* y at spawn (just above screen) */
#define ENEMY_SPAWN_X_MARGIN   50      /* min x = this, max x = WIDTH - this */

/* ── Enemy boundaries ───────────────────────────────────────────── */
#define ENEMY_X_MARGIN         20      /* enemy clamped to [MARGIN, WIDTH-MARGIN] */

/* ── Enemy movement multipliers ─────────────────────────────────── */
#define DART_DRIFT_FACTOR      0.5f    /* dart horizontal speed = speed_y * this */
#define HOVER_DRIFT_TIME       2.0f    /* hover drifts laterally for this many seconds */
#define HOVER_DIVE_FACTOR      2.0f    /* hover dive speed = speed_y * this */
#define SWARM_SWING_FACTOR     3.0f    /* swarm lateral oscillation amplitude factor */

/* ── Difficulty formula constants ───────────────────────────────── */
#define DIFF_BASE_ENEMIES      4       /* enemy_count at level 1 */
#define DIFF_ENEMIES_PER_LVL   2       /* extra enemies per level */
#define DIFF_BASE_INTERVAL     2.0f    /* spawn_interval at level 1 (seconds) */
#define DIFF_INTERVAL_DECAY    0.1f    /* interval reduction per level */
#define DIFF_MIN_INTERVAL      0.4f    /* spawn_interval floor */
#define DIFF_BASE_SPEED        80.0f   /* enemy base speed at level 1 */
#define DIFF_SPEED_PER_LVL     5.0f    /* speed increase per level */
#define DIFF_BASE_TYPE_RATIO   0.4f    /* type_ratio at level 1 */
#define DIFF_TYPE_RATIO_INC    0.03f   /* type_ratio increase per level */
#define DIFF_MAX_TYPE_RATIO    0.8f    /* type_ratio ceiling */

/* ── Win condition thresholds ───────────────────────────────────── */
#define WIN_PROGRESSIVE_SCORE  500     /* SCORE_THRESHOLD for Progressive */
#define WIN_ALTERNATE_KILLS    10      /* KILL_X_WITHIN_TIME for Alternate */
#define WIN_ALTERNATE_TIME     30.0f   /* time window (seconds) */
#define WIN_RANDOM_SURVIVAL    60.0f   /* SURVIVAL_TIME for Random */
#define WIN_WAVES_COUNT        5       /* SURVIVE_N_WAVES target */
#define WIN_FORMATIONS_LEVEL   5       /* REACH_LEVEL target */

/* ── Enemy type mix thresholds (used by the schedulers) ─────────── */
#define NUM_ENEMY_TYPES       5      /* Grunt, Tank, Dart, Hover, Swarm */
#define MIX_GRUNT_CUTOFF     0.4f   /* cumulative ratio below -> Grunt */
#define MIX_TANK_CUTOFF      0.6f
#define MIX_DART_CUTOFF      0.75f
#define MIX_HOVER_CUTOFF     0.9f
/* above MIX_HOVER_CUTOFF -> Swarm */

/* ── Waves / Formations modes ───────────────────────────────────── */
#define WAVE_ENEMIES_PER_WAVE  3       /* enemies spawned per wave burst */
#define FORMATION_COLS         5       /* formation columns */

/* ── Headless runner (main.c) ───────────────────────────────────── */
#define DEFAULT_SEED           12345u
#define MAX_HEADLESS_STEPS     10000

#endif
