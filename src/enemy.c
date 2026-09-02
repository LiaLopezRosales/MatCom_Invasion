#include "enemy.h"
#include "scheduler.h"
#include "balance.h"
#include <math.h>

static const enemy_type_data_t ENEMY_DATA[] = {
    [ENEMY_GRUNT] = {3, 80.0f, 120.0f, 2.0f, 50},
    [ENEMY_TANK]  = {8, 40.0f,  80.0f, 1.5f, 150},
    [ENEMY_DART]  = {2, 160.0f, 240.0f, 0.0f, 100},
    [ENEMY_HOVER] = {4, 80.0f, 110.0f, 3.0f, 120},
    [ENEMY_SWARM] = {1, 120.0f, 180.0f, 5.0f, 25},
};

const enemy_type_data_t *get_enemy_type_data(enemy_type_t type) {
    if (type < 0 || type > ENEMY_SWARM) return &ENEMY_DATA[ENEMY_GRUNT];
    return &ENEMY_DATA[type];
}

static unsigned int xorshift32(unsigned int *state) {
    unsigned int x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

void spawn_enemy(game_t *game) {
    if (!game) return;
    if (game->enemies_spawned >= game->max_enemies) return;

    int idx = game->enemies_spawned;
    if (idx >= MAX_ENEMIES) return;

    enemy_type_t type = game->enemies[idx].type;
    const enemy_type_data_t *data = get_enemy_type_data(type);

    game->enemies[idx].x = (int)(xorshift32(&game->rng_state) % (unsigned int)(SCREEN_WIDTH - ENEMY_SPAWN_X_MARGIN * 2)) + ENEMY_SPAWN_X_MARGIN;
    game->enemies[idx].y = ENEMY_SPAWN_Y;
    game->enemies[idx].fx = (float)game->enemies[idx].x;
    game->enemies[idx].fy = (float)ENEMY_SPAWN_Y;
    game->enemies[idx].life = data->life;
    game->enemies[idx].max_life = data->life;
    game->enemies[idx].active = 1;
    game->enemies[idx].phase = 0.0f;
    game->enemies[idx].type = type;
    game->enemies[idx].move_timer = 0.0f;
    game->enemies[idx].drift_direction = (xorshift32(&game->rng_state) % 2 == 0) ? 1 : -1;

    game->enemies_spawned++;
}

void update_enemies(game_t *game, float dt) {
    if (!game) return;

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!game->enemies[i].active) continue;

        enemy_t *e = &game->enemies[i];
        const enemy_type_data_t *data = get_enemy_type_data(e->type);

        e->phase += data->frequency * dt;

        // Accumulate position in float for frame-rate independent movement,
        // then derive the integer coordinates used by collision and rendering.
        switch (e->type) {
            case ENEMY_GRUNT:
            case ENEMY_TANK:
                e->fx += data->amplitude * sinf(e->phase) * dt;
                e->fy += data->speed_y * dt;
                break;

            case ENEMY_DART:
                e->fx += (float)e->drift_direction * data->speed_y * DART_DRIFT_FACTOR * dt;
                e->fy += data->speed_y * dt;
                break;

            case ENEMY_HOVER:
                e->move_timer += dt;
                if (e->move_timer < HOVER_DRIFT_TIME) {
                    e->fx += (float)e->drift_direction * data->amplitude * dt;
                } else {
                    e->fy += data->speed_y * HOVER_DIVE_FACTOR * dt;
                }
                break;

            case ENEMY_SWARM:
                e->fx += data->amplitude * sinf(e->phase) * dt * SWARM_SWING_FACTOR;
                e->fy += data->speed_y * dt;
                break;
        }

        e->x = (int)e->fx;
        e->y = (int)e->fy;

        if (e->x < ENEMY_X_MARGIN) { e->x = ENEMY_X_MARGIN; e->fx = (float)ENEMY_X_MARGIN; }
        if (e->x > SCREEN_WIDTH - ENEMY_X_MARGIN) { e->x = SCREEN_WIDTH - ENEMY_X_MARGIN; e->fx = (float)(SCREEN_WIDTH - ENEMY_X_MARGIN); }

        if (e->y > SCREEN_HEIGHT) {
            e->active = 0;
            game->state = STATE_GAME_OVER;
        }
    }
}
