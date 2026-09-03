#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "game.h"

/** Move all active projectiles upward by @p dt seconds; deactivate off-screen ones. */
void update_projectiles(game_t *game, float dt);

#endif
