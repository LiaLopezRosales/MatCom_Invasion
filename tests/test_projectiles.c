#include <criterion/criterion.h>
#include "game.h"

Test(projectiles, fire_spawns_projectile_at_ship) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_RANDOM);
    game_start(g);

    g->ship.x = 500;
    g->ship.y = 900;
    game_fire(g);

    cr_assert(g->projectiles[0].active == 1, "fire should activate a projectile");
    cr_assert(g->projectiles[0].x == 500, "projectile x should match ship x");
    cr_assert(g->projectiles[0].y == 900 - 15, "projectile should spawn just above ship");
    game_destroy(g);
}

Test(projectiles, fire_ignored_outside_playing) {
    game_t *g = game_create(1);
    cr_assert(g->state == STATE_MENU);
    game_fire(g);
    for (int i = 0; i < MAX_PROJECTILES; i++)
        cr_assert(g->projectiles[i].active == 0, "no projectile in MENU");
    game_destroy(g);
}

Test(projectiles, fire_respects_projectile_cap) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_RANDOM);
    game_start(g);

    for (int i = 0; i < MAX_PROJECTILES + 5; i++)
        game_fire(g);

    int active = 0;
    for (int i = 0; i < MAX_PROJECTILES; i++)
        if (g->projectiles[i].active) active++;

    cr_assert(active == MAX_PROJECTILES, "fire must not exceed MAX_PROJECTILES, got %d", active);
    game_destroy(g);
}

Test(projectiles, projectile_deactivates_at_top) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_RANDOM);
    game_start(g);

    g->projectiles[0].active = 1;
    g->projectiles[0].x = 100;
    g->projectiles[0].y = -21;

    for (int i = 0; i < 5; i++)
        game_update(g, 1.0f / 60.0f);

    cr_assert(g->projectiles[0].active == 0, "projectile past top should deactivate");
    game_destroy(g);
}

Test(projectiles, ship_bounded_in_x) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_RANDOM);
    game_start(g);

    game_set_ship(g, -100, 500);
    cr_assert(g->ship.x >= 0, "ship x should be clamped, got %d", g->ship.x);

    game_set_ship(g, 10000, 500);
    cr_assert(g->ship.x <= SCREEN_WIDTH, "ship x should be clamped high, got %d", g->ship.x);
    game_destroy(g);
}

Test(projectiles, repeated_ship_hits_remove_lives) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_RANDOM);
    game_start(g);

    int lives = g->lives;
    g->enemies[0].active = 1;
    g->enemies[0].type = ENEMY_GRUNT;
    g->enemies[0].life = 3;
    g->enemies[0].x = g->ship.x;
    g->enemies[0].y = g->ship.y;
    g->enemies[0].fx = (float)g->ship.x;
    g->enemies[0].fy = (float)g->ship.y;
    g->invuln_timer = 0.0f;

    g->projectiles[0].x = -999; // no projectile interference
    for (int i = 0; i < 30; i++) game_update(g, 1.0f / 60.0f);

    cr_assert(g->lives == lives - 1,
              "one ship contact should remove a life, got %d (was %d)", g->lives, lives);
    game_destroy(g);
}
