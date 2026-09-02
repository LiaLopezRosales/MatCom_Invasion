#include <criterion/criterion.h>
#include <stdlib.h>
#include "game.h"

Test(movement, enemies_move_independent_of_frame_count) {
    game_t *a = game_create(1);
    game_set_mode(a, MODE_PROGRESSIVE);
    game_start(a);

    // Place an active Grunt (speed_y=80) at a known position in both games
    int start_y = 500;
    a->enemies[0].active = 1;
    a->enemies[0].type = ENEMY_GRUNT;
    a->enemies[0].life = 3;
    a->enemies[0].max_life = 3;
    a->enemies[0].x = 300;
    a->enemies[0].y = start_y;
    a->enemies[0].fx = 300.0f;
    a->enemies[0].fy = (float)start_y;

    // advance 60 small frames (1.0s total)
    for (int i = 0; i < 60; i++) game_update(a, 1.0f / 60.0f);
    int y_many_frames = a->enemies[0].y;

    game_t *b = game_create(1);
    game_set_mode(b, MODE_PROGRESSIVE);
    game_start(b);
    b->enemies[0].active = 1;
    b->enemies[0].type = ENEMY_GRUNT;
    b->enemies[0].life = 3;
    b->enemies[0].max_life = 3;
    b->enemies[0].x = 300;
    b->enemies[0].y = start_y;
    b->enemies[0].fx = 300.0f;
    b->enemies[0].fy = (float)start_y;

    // advance 2 big frames (same 1.0s total)
    for (int i = 0; i < 2; i++) game_update(b, 0.5f);
    int y_few_frames = b->enemies[0].y;

    cr_assert(abs(y_many_frames - y_few_frames) <= 1,
              "Y depends only on elapsed time, not frame count: %d vs %d",
              y_many_frames, y_few_frames);
    cr_assert(y_many_frames > start_y, "enemy should have moved downward");
    game_destroy(a);
    game_destroy(b);
}

Test(movement, enemies_reaching_bottom_cause_game_over) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_PROGRESSIVE);
    game_start(g);

    // advance until enemy spawns
    for (int i = 0; i < 200; i++) game_update(g, 1.0f / 60.0f);
    cr_assert(g->enemies[0].active);

    // teleport enemy near bottom (Grunt speed_y=80)
    g->enemies[0].y = SCREEN_HEIGHT - 5;
    g->enemies[0].fy = (float)(SCREEN_HEIGHT - 5);
    for (int i = 0; i < 60; i++) game_update(g, 1.0f / 60.0f); // ~1s more

    cr_assert(g->state == STATE_GAME_OVER,
              "Enemy reaching bottom should trigger GAME_OVER, state=%d", g->state);
    game_destroy(g);
}

Test(movement, starts_with_immediate_full_batch) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_PROGRESSIVE);
    game_start(g);

    /* the batch modes spawn their entire batch instantly (no gradual timer). */
    cr_assert(g->enemies_spawned == g->max_enemies,
              "full batch should spawn immediately: spawned=%d max=%d",
              g->enemies_spawned, g->max_enemies);

    int active = 0;
    for (int i = 0; i < MAX_ENEMIES; i++)
        if (g->enemies[i].active) active++;
    cr_assert(active == g->max_enemies,
              "all spawned enemies should be active right away");

    /* after clearing the batch, a fresh one is respawned. */
    for (int i = 0; i < MAX_ENEMIES; i++)
        g->enemies[i].active = 0;
    game_update(g, 1.0f / 60.0f);
    cr_assert(g->enemies_spawned >= g->max_enemies,
              "clearing a batch should recycle and respawn, spawned=%d max=%d",
              g->enemies_spawned, g->max_enemies);
    game_destroy(g);
}

Test(movement, zero_dt_is_safe) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_RANDOM);
    game_start(g);
    game_update(g, 0.0f); // must not crash or divide by zero
    cr_assert(g->state == STATE_PLAYING);
    game_destroy(g);
}

Test(movement, large_dt_is_clamped) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_RANDOM);
    game_start(g);
    // A huge dt must be clamped (engine caps frames) without corrupting state.
    float survival_before = g->survival_timer;
    game_update(g, 10.0f);
    // clamped to 1.0, so survival advances by at most 1s
    cr_assert(g->survival_timer - survival_before <= 1.0f + 1e-6,
              "large dt should be clamped to 1.0s, advanced by %f",
              (double)(g->survival_timer - survival_before));
    cr_assert(g->state == STATE_PLAYING || g->state == STATE_GAME_OVER ||
              g->state == STATE_VICTORY, "state should remain valid after large dt");
    game_destroy(g);
}

Test(movement, negative_dt_is_sanitized) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_RANDOM);
    game_start(g);

    float survival_before = g->survival_timer;
    float spawn_before = g->spawn_timer;
    game_update(g, -5.0f); // must not run timers backwards

    cr_assert(g->survival_timer >= survival_before,
              "negative dt must not decrease survival_timer");
    cr_assert(g->spawn_timer >= spawn_before,
              "negative dt must not decrease spawn_timer");
    cr_assert(g->state == STATE_PLAYING, "state stays valid after negative dt");
    game_destroy(g);
}
