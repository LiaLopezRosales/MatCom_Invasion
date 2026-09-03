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

/** Allocate and initialise a new game with the given RNG seed. */
game_t *game_create(unsigned int seed);

/** Free all memory owned by @p game. */
void game_destroy(game_t *game);

/** Set the game mode before starting (must be called before game_start). */
void game_set_mode(game_t *game, game_mode_t mode);

/** Start the current mode: reset score/lives and schedule the first batch. */
void game_start(game_t *game);

/** Advance the simulation by @p dt seconds (deterministic, seeded). */
void game_update(game_t *game, float dt);

/** Move the player ship to the given pixel coordinates. */
void game_set_ship(game_t *game, int x, int y);

/** Fire a projectile from the current ship position. */
void game_fire(game_t *game);

/** Write a flat snapshot of the game state into @p out (read by the WASM bridge). */
void game_get_state(const game_t *game, game_state_snapshot_t *out);

/** Pause the game (freezes the update loop). */
void game_pause(game_t *game);

/** Resume a paused game. */
void game_resume(game_t *game);

#endif
