#include "scheduler.h"
#include "enemy.h"
#include "balance.h"
#include <string.h>

static unsigned int xorshift32(unsigned int *state) {
    unsigned int x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

/* canonical enemy-type ordering (matches enemy_type_t, 0..NUM_ENEMY_TYPES-1) */
static const enemy_type_t ALL_TYPES[NUM_ENEMY_TYPES] = {
    ENEMY_GRUNT, ENEMY_TANK, ENEMY_DART, ENEMY_HOVER, ENEMY_SWARM
};

static enemy_type_t get_type_for_index(int index, int total) {
    if (total <= 0) return ENEMY_GRUNT;
    float ratio = (float)index / (float)total;
    if (ratio < MIX_GRUNT_CUTOFF) return ENEMY_GRUNT;
    if (ratio < MIX_TANK_CUTOFF)  return ENEMY_TANK;
    if (ratio < MIX_DART_CUTOFF)  return ENEMY_DART;
    if (ratio < MIX_HOVER_CUTOFF) return ENEMY_HOVER;
    return ENEMY_SWARM;
}

static void generate_progressive(game_t *game) {
    int count = game->max_enemies;

    // Build a good type distribution, then sort by ascending life (SJF:
    // shortest job first = weaker enemies spawn before stronger ones).
    enemy_type_t order[MAX_ENEMIES];
    for (int i = 0; i < count; i++)
        order[i] = get_type_for_index(i, count);

    // insertion sort by ascending life (stable)
    for (int i = 1; i < count; i++) {
        enemy_type_t key = order[i];
        int key_life = get_enemy_type_data(key)->life;
        int j = i - 1;
        while (j >= 0 && get_enemy_type_data(order[j])->life > key_life) {
            order[j + 1] = order[j];
            j--;
        }
        order[j + 1] = key;
    }

    for (int i = 0; i < count; i++)
        game->enemies[i].type = order[i];
}

static void generate_alternate(game_t *game) {
    int count = game->max_enemies;
    for (int i = 0; i < count; i++)
        game->enemies[i].type = ALL_TYPES[i % NUM_ENEMY_TYPES];
}

static void generate_random(game_t *game) {
    int count = game->max_enemies;
    for (int i = 0; i < count; i++)
        game->enemies[i].type = get_type_for_index(i, count);

    for (int i = count - 1; i > 0; i--) {
        int j = (int)(xorshift32(&game->rng_state) % (unsigned int)(i + 1));
        enemy_type_t temp = game->enemies[i].type;
        game->enemies[i].type = game->enemies[j].type;
        game->enemies[j].type = temp;
    }
}

static void generate_waves(game_t *game) {
    int count = game->max_enemies;
    if (count <= 0) return;

    /* A single wave is one immediate batch. Distribute the five enemy types so
       each wave is a varied mix, and rotate the mix with the wave number so
       successive waves differ (still deterministic from the seed). */
    int rotation = (int)(game->waves_completed % (unsigned int)NUM_ENEMY_TYPES);
    for (int i = 0; i < count; i++)
        game->enemies[i].type = ALL_TYPES[(i + rotation) % NUM_ENEMY_TYPES];
}

static void generate_formations(game_t *game) {
    int count = game->max_enemies;
    for (int i = 0; i < count; i++) {
        int row = i / FORMATION_COLS;
        int col = i % FORMATION_COLS;
        if (row == 0)
            game->enemies[i].type = ENEMY_GRUNT;
        else if (row == 1)
            game->enemies[i].type = ENEMY_TANK;
        else
            game->enemies[i].type = (col % 2 == 0) ? ENEMY_DART : ENEMY_SWARM;
    }
}

void scheduler_generate_order(game_t *game) {
    if (!game) return;

    for (int i = 0; i < MAX_ENEMIES; i++) {
        game->enemies[i].active = 0;
        game->enemies[i].life = 0;
        game->enemies[i].max_life = 0;
        game->enemies[i].phase = 0;
        game->enemies[i].move_timer = 0;
        game->enemies[i].drift_direction = 0;
    }

    switch (game->mode) {
        case MODE_PROGRESSIVE: generate_progressive(game); break;
        case MODE_ALTERNATE:   generate_alternate(game);   break;
        case MODE_RANDOM:      generate_random(game);      break;
        case MODE_WAVES:       generate_waves(game);       break;
        case MODE_FORMATIONS:  generate_formations(game);  break;
    }
}

enemy_type_t scheduler_next_type(game_t *game) {
    if (!game) return ENEMY_GRUNT;
    int idx = game->enemies_spawned;
    if (idx >= MAX_ENEMIES) idx = MAX_ENEMIES - 1;
    return game->enemies[idx].type;
}
