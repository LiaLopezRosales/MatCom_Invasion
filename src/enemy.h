#ifndef ENEMY_H
#define ENEMY_H

#include "game.h"

typedef struct {
    int life;
    float speed_y;
    float amplitude;
    float frequency;
    int score;
} enemy_type_data_t;

void spawn_enemy(game_t *game);
void update_enemies(game_t *game, float dt);
const enemy_type_data_t *get_enemy_type_data(enemy_type_t type);

#endif
