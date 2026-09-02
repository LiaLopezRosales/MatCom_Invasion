#include <criterion/criterion.h>
#include "collision.h"
#include "game.h"
#include "enemy.h"

Test(collision, projectile_damages_enemy) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_RANDOM);
    game_start(g);

    // position a projectile on an active grunt
    g->enemies[0].active = 1;
    g->enemies[0].type = ENEMY_GRUNT; // life 3
    g->enemies[0].life = 3;
    g->enemies[0].x = 500;
    g->enemies[0].y = 200;

    g->projectiles[0].active = 1;
    g->projectiles[0].x = 500;
    g->projectiles[0].y = 200;

    int destroyed_before = g->enemies_destroyed;
    check_collisions(g);

    cr_assert(g->enemies[0].life == 2, "Grunt should lose 1 life on hit, got %d", g->enemies[0].life);
    cr_assert(g->projectiles[0].active == 0, "Projectile should deactivate on hit");
    cr_assert(g->enemies_destroyed == destroyed_before, "not destroyed yet (life>0)");
    game_destroy(g);
}

Test(collision, projectile_destroys_enemy_and_adds_score) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_RANDOM);
    game_start(g);

    g->enemies[0].active = 1;
    g->enemies[0].type = ENEMY_SWARM; // life 1
    g->enemies[0].life = 1;
    g->enemies[0].x = 300;
    g->enemies[0].y = 300;

    g->projectiles[0].active = 1;
    g->projectiles[0].x = 300;
    g->projectiles[0].y = 300;

    int score_before = g->score;
    check_collisions(g);

    cr_assert(g->enemies[0].active == 0, "Swarm (life 1) should be destroyed");
    cr_assert(g->enemies_destroyed == 1, "enemies_destroyed should increment");
    cr_assert(g->score > score_before, "score should increase on kill");
    game_destroy(g);
}

Test(collision, non_overlapping_projectile_passes) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_RANDOM);
    game_start(g);

    g->enemies[0].active = 1;
    g->enemies[0].type = ENEMY_GRUNT;
    g->enemies[0].life = 3;
    g->enemies[0].x = 100;
    g->enemies[0].y = 100;

    g->projectiles[0].active = 1;
    g->projectiles[0].x = 900; // far away
    g->projectiles[0].y = 100;

    check_collisions(g);

    cr_assert(g->projectiles[0].active == 1, "non-hitting projectile stays active");
    cr_assert(g->enemies[0].life == 3, "enemy unaffected");
    game_destroy(g);
}

Test(collision, two_projectiles_no_double_damage_same_frame) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_RANDOM);
    game_start(g);

    g->enemies[0].active = 1;
    g->enemies[0].type = ENEMY_GRUNT;
    g->enemies[0].life = 3;
    g->enemies[0].x = 400;
    g->enemies[0].y = 400;

    // two projectiles overlapping the same enemy in the same frame
    g->projectiles[0].active = 1; g->projectiles[0].x = 400; g->projectiles[0].y = 400;
    g->projectiles[1].active = 1; g->projectiles[1].x = 401; g->projectiles[1].y = 400;

    check_collisions(g);

    cr_assert(g->enemies[0].life == 2,
              "enemy should only take 1 hit per frame, got life %d (expected 2)", g->enemies[0].life);
    game_destroy(g);
}

Test(collision, ship_collision_removes_life) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_RANDOM);
    game_start(g);

    int lives_before = g->lives;
    g->enemies[0].active = 1;
    g->enemies[0].type = ENEMY_GRUNT;
    g->enemies[0].life = 3;
    // overlap the ship exactly
    g->enemies[0].x = g->ship.x;
    g->enemies[0].y = g->ship.y;
    g->invuln_timer = 0.0f;

    check_collisions(g);

    cr_assert(g->lives == lives_before - 1, "ship collision should cost 1 life");
    cr_assert(g->invuln_timer > 0.0f, "invulnerability should be granted after hit");
    game_destroy(g);
}

Test(collision, ship_dies_at_zero_lives) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_PROGRESSIVE);
    game_start(g);

    // force lives to 1 then collide
    g->lives = 1;
    g->enemies[0].active = 1;
    g->enemies[0].type = ENEMY_GRUNT;
    g->enemies[0].x = g->ship.x;
    g->enemies[0].y = g->ship.y;
    g->invuln_timer = 0.0f;

    check_collisions(g);

    cr_assert(g->lives == 0, "lives should reach 0");
    cr_assert(g->state == STATE_GAME_OVER, "0 lives should trigger GAME_OVER");
    game_destroy(g);
}
