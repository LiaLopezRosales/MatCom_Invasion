#ifndef ENEMY_H
#define ENEMY_H

#include "game.h"

/** Per-type static data (HP, speed, wobble params, score value). */
typedef struct {
    int life;
    float speed_y;
    float amplitude;
    float frequency;
    int score;
} enemy_type_data_t;

/** Spawn a single enemy at the top of the screen according to the current difficulty. */
void spawn_enemy(game_t *game);

/** Move and animate all active enemies by @p dt seconds. */
void update_enemies(game_t *game, float dt);

/** Return the static data table for a given enemy type (immutable). */
const enemy_type_data_t *get_enemy_type_data(enemy_type_t type);

#endif
