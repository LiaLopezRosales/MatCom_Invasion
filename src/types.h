#ifndef TYPES_H
#define TYPES_H

#define MAX_PROJECTILES 15
#define MAX_ENEMIES 10
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAME_OVER,
    STATE_VICTORY
} game_state_t;

typedef enum {
    MODE_PROGRESSIVE,
    MODE_ALTERNATE,
    MODE_RANDOM,
    MODE_WAVES,
    MODE_FORMATIONS
} game_mode_t;

typedef enum {
    ENEMY_GRUNT,
    ENEMY_TANK,
    ENEMY_DART,
    ENEMY_HOVER,
    ENEMY_SWARM
} enemy_type_t;

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
} enemy_t;

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
} game_state_snapshot_t;

#endif
