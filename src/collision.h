#ifndef COLLISION_H
#define COLLISION_H

#include "game.h"

/** Check projectile-enemy and enemy-ship collisions; update score and state. */
void check_collisions(game_t *game);

#endif
