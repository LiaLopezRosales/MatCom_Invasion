#include "collision.h"
#include "enemy.h"
#include "balance.h"
#include <stdlib.h>

#define PROJECTILE_HIT_RADIUS 15
#define SHIP_HIT_RADIUS 26

static int boxes_overlap(int x1, int y1, int r1, int x2, int y2, int r2) {
    int dx = abs(x1 - x2);
    int dy = abs(y1 - y2);
    return (dx <= r1 + r2) && (dy <= r1 + r2);
}

void check_collisions(game_t *game) {
    if (!game) return;

    int hit_this_frame[MAX_ENEMIES] = {0};

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!game->projectiles[i].active) continue;

        for (int j = 0; j < MAX_ENEMIES; j++) {
            if (!game->enemies[j].active) continue;
            if (hit_this_frame[j]) continue;

            if (boxes_overlap(
                    game->projectiles[i].x, game->projectiles[i].y, PROJECTILE_HIT_RADIUS,
                    game->enemies[j].x, game->enemies[j].y, PROJECTILE_HIT_RADIUS))
            {
                game->projectiles[i].active = 0;
                game->enemies[j].life--;
                hit_this_frame[j] = 1;

                if (game->enemies[j].life <= 0) {
                    game->enemies[j].active = 0;
                    game->enemies_destroyed++;
                    const enemy_type_data_t *data = get_enemy_type_data(game->enemies[j].type);
                    game->score += data->score;
                }
                break;
            }
        }
    }

    if (game->invuln_timer > 0.0f) return;

    for (int j = 0; j < MAX_ENEMIES; j++) {
        if (!game->enemies[j].active) continue;

        if (boxes_overlap(
                game->ship.x, game->ship.y, SHIP_HIT_RADIUS,
                game->enemies[j].x, game->enemies[j].y, SHIP_HIT_RADIUS))
        {
            game->lives--;
            game->invuln_timer = INVULN_DURATION;

            if (game->lives <= 0)
                game->state = STATE_GAME_OVER;
            break;
        }
    }
}
