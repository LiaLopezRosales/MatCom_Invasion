#include "enemy.h"
#include "scheduler.h"
#include <math.h>

static const enemy_type_data_t ENEMY_DATA[] = {
    [ENEMY_GRUNT] = {3, 80.0f, 60.0f, 2.0f, 50},
    [ENEMY_TANK]  = {8, 40.0f, 80.0f, 1.5f, 150},
    [ENEMY_DART]  = {2, 160.0f, 0.0f, 0.0f, 100},
    [ENEMY_HOVER] = {4, 80.0f, 50.0f, 3.0f, 120},
    [ENEMY_SWARM] = {1, 120.0f, 30.0f, 5.0f, 25},
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

    game->enemies[idx].x = (int)(xorshift32(&game->rng_state) % (unsigned int)(SCREEN_WIDTH - 100)) + 50;
    game->enemies[idx].y = -30;
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

        switch (e->type) {
            case ENEMY_GRUNT:
            case ENEMY_TANK:
                e->x += (int)(data->amplitude * sinf(e->phase) * dt);
                e->y += (int)(data->speed_y * dt);
                break;

            case ENEMY_DART:
                e->x += (int)(e->drift_direction * data->speed_y * 0.5f * dt);
                e->y += (int)(data->speed_y * dt);
                break;

            case ENEMY_HOVER:
                e->move_timer += dt;
                if (e->move_timer < 2.0f) {
                    e->x += (int)(e->drift_direction * data->amplitude * dt);
                } else {
                    e->y += (int)(data->speed_y * 2.0f * dt);
                }
                break;

            case ENEMY_SWARM:
                e->x += (int)(data->amplitude * sinf(e->phase) * dt * 3.0f);
                e->y += (int)(data->speed_y * dt);
                break;
        }

        if (e->x < 20) e->x = 20;
        if (e->x > SCREEN_WIDTH - 20) e->x = SCREEN_WIDTH - 20;

        if (e->y > SCREEN_HEIGHT) {
            e->active = 0;
            game->state = STATE_GAME_OVER;
        }
    }
}
