#ifndef TYPES_H
#define TYPES_H

#define MAX_PROJECTILES 15
#define MAX_ENEMIES 10
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

/** Finite-state machine that drives the frontend. */
typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAME_OVER,
    STATE_VICTORY
} game_state_t;

/** Five scheduling modes, each with a distinct spawn pattern. */
typedef enum {
    MODE_PROGRESSIVE,   /* weakest-first batches that scale each wave */
    MODE_ALTERNATE,     /* round-robin interleaving of all types */
    MODE_RANDOM,        /* deterministic shuffled hordes */
    MODE_WAVES,         /* timed burst waves */
    MODE_FORMATIONS     /* grid formations that grow per level */
} game_mode_t;

/** The five enemy archetypes. */
typedef enum {
    ENEMY_GRUNT,
    ENEMY_TANK,
    ENEMY_DART,
    ENEMY_HOVER,
    ENEMY_SWARM
} enemy_type_t;

/** Win-condition discriminant for each game mode. */
typedef enum {
    WIN_SCORE_THRESHOLD,
    WIN_KILL_X_WITHIN_TIME,
    WIN_SURVIVAL_TIME,
    WIN_SURVIVE_N_WAVES,
    WIN_REACH_LEVEL
} win_type_t;

typedef struct {
    int x;
    int y;
} ship_t;

typedef struct {
    int x;
    int y;
    int active;
} projectile_t;

typedef struct {
    int x;
    int y;
    int life;
    int max_life;
    int active;
    float phase;
    enemy_type_t type;
    float move_timer;
    int drift_direction;
    float fx;
    float fy;
} enemy_t;

/** Data-driven win condition.  Interpretation of the int/float params depends on type. */
typedef struct {
    win_type_t type;
    int param_int_1;
    int param_int_2;
    float param_float_1;
} win_condition_t;

typedef struct {
    float spawn_interval;
    float base_speed;
    int enemy_count;
    float type_ratio;
} difficulty_t;

typedef struct {
    game_state_t state;
    int score;
    int high_score;
    int lives;
    int current_level;
    int ship_x;
    int ship_y;
    int projectile_count;
    int enemy_count;
    int enemies_destroyed;

    /* ── gameplay data (Fase 5) ── */
    float survival_timer;

    /* ── render data (Fase 4) ── */
    game_mode_t mode;
    int enemy_x[MAX_ENEMIES];
    int enemy_y[MAX_ENEMIES];
    int enemy_active[MAX_ENEMIES];
    int enemy_type[MAX_ENEMIES];
    int enemy_life[MAX_ENEMIES];
    int projectile_x[MAX_PROJECTILES];
    int projectile_y[MAX_PROJECTILES];
    int projectile_active[MAX_PROJECTILES];
} game_state_snapshot_t;

/* Layout guard: the web loader (web/src/game.ts) reads this snapshot as a flat
   sequence of int32 words. All fields are int/enum (4 bytes) with no padding, so
   the byte size must equal (11 scalars + 5*MAX_ENEMIES + 3*MAX_PROJECTILES) * 4.
   If this ever changes, update SNAPSHOT_FIELDS in web/src/game.ts in sync. */
_Static_assert(sizeof(game_state_snapshot_t) ==
               (12 + 5 * MAX_ENEMIES + 3 * MAX_PROJECTILES) * sizeof(int),
               "game_state_snapshot_t layout must match web/src/game.ts SNAPSHOT_FIELDS");

#endif
