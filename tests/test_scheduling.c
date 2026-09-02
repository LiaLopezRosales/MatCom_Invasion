#include <criterion/criterion.h>
#include <stdlib.h>
#include "game.h"
#include "enemy.h"
#include "balance.h"

Test(scheduling, progressive_sorts_by_life_ascending) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_PROGRESSIVE);
    game_start(g);

    int count = g->max_enemies;
    for (int i = 1; i < count; i++) {
        const enemy_type_data_t *prev = get_enemy_type_data(g->enemies[i - 1].type);
        const enemy_type_data_t *cur = get_enemy_type_data(g->enemies[i].type);
        cr_assert(prev->life <= cur->life,
                  "Progressive should be non-decreasing life: idx %d life %d > idx %d life %d",
                  i - 1, prev->life, i, cur->life);
    }
    game_destroy(g);
}

Test(scheduling, progressive_starts_with_grunt) {
    game_t *g = game_create(2);
    game_set_mode(g, MODE_PROGRESSIVE);
    game_start(g);
    cr_assert(g->enemies[0].type == ENEMY_GRUNT,
              "Progressive lowest-life enemy should be Grunt");
    game_destroy(g);
}

Test(scheduling, alternate_intercalates_types) {
    game_t *g = game_create(3);
    game_set_mode(g, MODE_ALTERNATE);
    game_start(g);

    // RR should alternate among the 5 types predictably.
    for (int i = 0; i < g->max_enemies && i + 5 < MAX_ENEMIES; i += 5) {
        cr_assert(g->enemies[i].type == ENEMY_GRUNT,
                  "RR cycle position 0 should be GRUNT");
        cr_assert(g->enemies[i + 1].type == ENEMY_TANK,
                  "RR cycle position 1 should be TANK");
    }
    game_destroy(g);
}

Test(scheduling, alternate_produces_balanced_types) {
    game_t *g = game_create(4);
    game_set_mode(g, MODE_ALTERNATE);
    game_start(g);

    int grunt = 0, tank = 0;
    for (int i = 0; i < g->max_enemies; i++) {
        if (g->enemies[i].type == ENEMY_GRUNT) grunt++;
        if (g->enemies[i].type == ENEMY_TANK) tank++;
    }
    // With max_enemies <= MAX_ENEMIES(10) and 5-type cycle, counts differ by at most 1.
    cr_assert(abs(grunt - tank) <= 1, "Alternate should balance types, grunt=%d tank=%d", grunt, tank);
    game_destroy(g);
}

Test(scheduling, random_is_deterministic_per_seed) {
    game_t *a = game_create(42);
    game_set_mode(a, MODE_RANDOM);
    game_start(a);

    game_t *b = game_create(42);
    game_set_mode(b, MODE_RANDOM);
    game_start(b);

    for (int i = 0; i < MAX_ENEMIES; i++)
        cr_assert(a->enemies[i].type == b->enemies[i].type,
                  "Same seed must produce same order at idx %d", i);
    game_destroy(a);
    game_destroy(b);
}

Test(scheduling, random_different_seeds_may_differ) {
    game_t *a = game_create(1);
    game_set_mode(a, MODE_RANDOM);
    game_start(a);

    game_t *b = game_create(2);
    game_set_mode(b, MODE_RANDOM);
    game_start(b);

    int differ = 0;
    for (int i = 0; i < MAX_ENEMIES; i++)
        if (a->enemies[i].type != b->enemies[i].type) differ++;

    cr_assert(differ > 0, "Different seeds should generally produce different orders");
    game_destroy(a);
    game_destroy(b);

}

Test(scheduling, spawn_never_exceeds_max_enemies) {
    game_t *g = game_create(5);
    game_set_mode(g, MODE_PROGRESSIVE);
    game_start(g);
    for (int i = 0; i < 60 * 60; i++)
        game_update(g, 1.0f / 60.0f);
    cr_assert(g->enemies_spawned <= g->max_enemies,
              "spawned=%d must not exceed max_enemies=%d", g->enemies_spawned, g->max_enemies);
    game_destroy(g);
}

Test(scheduling, max_enemies_respected) {
    /* Formations spawns a full immediate formation at level 1:
       5 columns x 1 row = 5 enemies (not the gradual difficulty count). */
    game_t *g = game_create(6);
    game_set_mode(g, MODE_FORMATIONS);
    game_start(g);
    cr_assert(g->max_enemies == FORMATION_COLS,
              "level 1 formation should spawn FORMATION_COLS enemies, got %d",
              g->max_enemies);
    cr_assert(g->max_enemies <= MAX_ENEMIES, "max_enemies must not exceed MAX_ENEMIES");
    game_destroy(g);
}
