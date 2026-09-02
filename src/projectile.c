#include "projectile.h"

#define PROJECTILE_SPEED 400.0f
#define PROJECTILE_KILL_Y (-20)  /* deactivate once fully off the top of the screen */

void update_projectiles(game_t *game, float dt) {
    if (!game) return;

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!game->projectiles[i].active) continue;

        game->projectiles[i].y -= (int)(PROJECTILE_SPEED * dt);

        if (game->projectiles[i].y < PROJECTILE_KILL_Y)
            game->projectiles[i].active = 0;
    }
}
