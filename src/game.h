#ifndef GAME_H
#define GAME_H

#include "types.h"

typedef struct {
    game_state_t state;
    game_mode_t mode;

    ship_t ship;
    int lives;
    float invuln_timer;

    projectile_t projectiles[MAX_PROJECTILES];
    enemy_t enemies[MAX_ENEMIES];

    win_condition_t win_cond;
    difficulty_t difficulty;

    int current_level;
    int max_enemies;
    int enemies_spawned;
    int enemies_destroyed;
    float spawn_timer;

    int score;
    int high_score;

    float survival_timer;
    int waves_completed;

    unsigned int rng_state;
} game_t;

game_t *game_create(unsigned int seed);
void game_destroy(game_t *game);
void game_set_mode(game_t *game, game_mode_t mode);
void game_start(game_t *game);
void game_update(game_t *game, float dt);
void game_set_ship(game_t *game, int x, int y);
void game_fire(game_t *game);
void game_get_state(const game_t *game, game_state_snapshot_t *out);
void game_pause(game_t *game);
void game_resume(game_t *game);

#endif
