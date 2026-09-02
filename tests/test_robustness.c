#include <criterion/criterion.h>
#include "game.h"
#include "difficulty.h"
#include "balance.h"

/* ── NULL safety ────────────────────────────────────────────────── */

Test(robustness, null_game_create_does_not_crash) {
    /* game_create(0) is valid (seed=0 is a valid xorshift seed) */
    game_t *g = game_create(0);
    cr_assert(g != NULL, "game_create(0) must succeed");
    game_destroy(g);
}

Test(robustness, null_update_is_safe) {
    game_update(NULL, 1.0f / 60.0f);
    /* must not crash */
}

Test(robustness, null_fire_is_safe) {
    game_fire(NULL);
}

Test(robustness, null_set_ship_is_safe) {
    game_set_ship(NULL, 100, 200);
}

Test(robustness, null_get_state_is_safe) {
    game_state_snapshot_t snap;
    game_get_state(NULL, &snap);
}

/* ── Operations outside PLAYING state ──────────────────────────── */

Test(robustness, fire_in_menu_is_noop) {
    game_t *g = game_create(1);
    cr_assert(g->state == STATE_MENU);
    game_fire(g);
    int active = 0;
    for (int i = 0; i < MAX_PROJECTILES; i++)
        if (g->projectiles[i].active) active++;
    cr_assert(active == 0, "no projectiles in MENU");
    game_destroy(g);
}

Test(robustness, set_ship_in_menu_is_noop) {
    game_t *g = game_create(1);
    game_set_ship(g, 500, 500);
    cr_assert(g->ship.x == 0 && g->ship.y == 0,
              "ship must not change outside PLAYING");
    game_destroy(g);
}

Test(robustness, update_in_gameover_is_noop) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_PROGRESSIVE);
    game_start(g);
    g->lives = 0;
    g->enemies[0].active = 1;
    g->enemies[0].type = ENEMY_GRUNT;
    g->enemies[0].x = g->ship.x;
    g->enemies[0].y = g->ship.y;
    g->enemies[0].fx = (float)g->ship.x;
    g->enemies[0].fy = (float)g->ship.y;
    g->invuln_timer = 0.0f;
    for (int i = 0; i < 5; i++) game_update(g, 1.0f / 60.0f);
    cr_assert(g->state == STATE_GAME_OVER);

    int score_before = g->score;
    for (int i = 0; i < 60; i++) game_update(g, 1.0f / 60.0f);
    cr_assert(g->score == score_before, "update in GAME_OVER is a no-op");
    game_destroy(g);
}

/* ── Array bounds: fire with full projectile slots ──────────────── */

Test(robustness, fire_with_full_projectile_slots) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_PROGRESSIVE);
    game_start(g);

    /* fill all projectile slots */
    for (int i = 0; i < MAX_PROJECTILES; i++)
        g->projectiles[i].active = 1;

    /* fire must not crash or corrupt state */
    game_fire(g);
    cr_assert(g->state == STATE_PLAYING, "state unchanged after fire on full slots");

    /* verify no out-of-bounds write */
    int count = 0;
    for (int i = 0; i < MAX_PROJECTILES; i++)
        if (g->projectiles[i].active) count++;
    cr_assert(count == MAX_PROJECTILES, "no projectile added beyond cap");
    game_destroy(g);
}

/* ── High levels: difficulty clamping ────────────────────────────── */

Test(robustness, very_high_level_clamps) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_PROGRESSIVE);
    game_start(g);

    /* simulate to level 500 */
    for (int i = 0; i < 499; i++) {
        g->current_level++;
        difficulty_init(&g->difficulty, g->current_level);
        g->max_enemies = g->difficulty.enemy_count;
    }

    cr_assert(g->max_enemies <= MAX_ENEMIES,
              "max_enemies clamped at high level, got %d", g->max_enemies);
    cr_assert(g->difficulty.spawn_interval >= 0.4f,
              "spawn_interval clamped at high level");
    cr_assert(g->difficulty.type_ratio <= 0.8f,
              "type_ratio clamped at high level");
    game_destroy(g);
}

/* ── Rapid state transitions ────────────────────────────────────── */

Test(robustness, rapid_pause_resume) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_RANDOM);
    game_start(g);

    for (int i = 0; i < 200; i++) {
        game_pause(g);
        cr_assert(g->state == STATE_PAUSED);
        game_resume(g);
        cr_assert(g->state == STATE_PLAYING);
    }
    game_destroy(g);
}

Test(robustness, restart_many_times) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_WAVES);

    for (int i = 0; i < 20; i++) {
        game_start(g);
        cr_assert(g->state == STATE_PLAYING);
        cr_assert(g->lives == INITIAL_LIVES);
        cr_assert(g->score == 0);
        /* advance a bit */
        for (int j = 0; j < 60; j++)
            game_update(g, 1.0f / 60.0f);
    }
    game_destroy(g);
}

/* ── Win + restart cycle through all modes ──────────────────────── */

Test(robustness, win_and_restart_all_modes) {
    for (game_mode_t m = MODE_PROGRESSIVE; m <= MODE_FORMATIONS; m++) {
        game_t *g = game_create(42);
        game_set_mode(g, m);
        game_start(g);

        /* force victory by setting the matching win condition */
        switch (m) {
            case MODE_PROGRESSIVE:
                g->score = WIN_PROGRESSIVE_SCORE; break;
            case MODE_ALTERNATE:
                g->enemies_destroyed = WIN_ALTERNATE_KILLS; break;
            case MODE_RANDOM:
                g->survival_timer = WIN_RANDOM_SURVIVAL; break;
            case MODE_WAVES:
                g->waves_completed = WIN_WAVES_COUNT; break;
            case MODE_FORMATIONS:
                g->current_level = WIN_FORMATIONS_LEVEL + 1; break;
        }
        game_update(g, 1.0f / 60.0f);
        cr_assert(g->state == STATE_VICTORY, "mode %d should reach VICTORY", m);

        /* restart must be clean */
        game_start(g);
        cr_assert(g->state == STATE_PLAYING, "restart after VICTORY, mode %d", m);
        cr_assert(g->lives == INITIAL_LIVES);
        game_destroy(g);
    }
}

/* ── Extreme dt combinations ────────────────────────────────────── */

Test(robustness, alternating_dt_extremes) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_PROGRESSIVE);
    game_start(g);

    /* alternate between 0 and 1 (max) dt — must not corrupt state */
    for (int i = 0; i < 1000; i++) {
        float dt = (i % 2 == 0) ? 0.0f : 1.0f;
        game_update(g, dt);
        cr_assert(g->state == STATE_PLAYING || g->state == STATE_GAME_OVER ||
                  g->state == STATE_VICTORY,
                  "state valid after extreme dt at step %d", i);
    }
    game_destroy(g);
}

/* ── Enemy boundary clamping ────────────────────────────────────── */

Test(robustness, enemy_x_clamped_on_spawn) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_PROGRESSIVE);
    game_start(g);

    /* advance until enemies spawn */
    for (int i = 0; i < 200; i++) game_update(g, 1.0f / 60.0f);

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!g->enemies[i].active) continue;
        cr_assert(g->enemies[i].x >= ENEMY_X_MARGIN,
                  "enemy %d x=%d below margin", i, g->enemies[i].x);
        cr_assert(g->enemies[i].x <= SCREEN_WIDTH - ENEMY_X_MARGIN,
                  "enemy %d x=%d above margin", i, g->enemies[i].x);
    }
    game_destroy(g);
}
