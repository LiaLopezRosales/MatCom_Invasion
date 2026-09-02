#include <criterion/criterion.h>
#include "game.h"
#include "balance.h"

/* helper: deactivate every currently active enemy (as if destroyed) */
static void kill_all_enemies(game_t *g) {
    for (int i = 0; i < MAX_ENEMIES; i++)
        g->enemies[i].active = 0;
}

Test(waves, starts_with_immediate_full_batch) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_WAVES);
    game_start(g);

    /* all WAVE_ENEMIES_PER_WAVE enemies spawn immediately, no timer */
    cr_assert(g->enemies_spawned == WAVE_ENEMIES_PER_WAVE,
              "waves should spawn the full batch immediately, got %d",
              g->enemies_spawned);
    int active = 0;
    for (int i = 0; i < MAX_ENEMIES; i++)
        if (g->enemies[i].active) active++;
    cr_assert(active == WAVE_ENEMIES_PER_WAVE,
              "expected %d active enemies, got %d", WAVE_ENEMIES_PER_WAVE, active);
    game_destroy(g);
}

Test(waves, clearing_wave_increments_waves_completed) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_WAVES);
    game_start(g);

    cr_assert(g->waves_completed == 0);
    kill_all_enemies(g);
    game_update(g, 1.0f / 60.0f);

    cr_assert(g->waves_completed == 1,
              "clearing wave 1 should increment to 1, got %d", g->waves_completed);
    game_destroy(g);
}

Test(waves, next_wave_spawns_after_clear) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_WAVES);
    game_start(g);

    kill_all_enemies(g);
    game_update(g, 1.0f / 60.0f);

    /* a fresh batch should now be active */
    int active = 0;
    for (int i = 0; i < MAX_ENEMIES; i++)
        if (g->enemies[i].active) active++;
    cr_assert(active == WAVE_ENEMIES_PER_WAVE,
              "after clear, next wave of %d should spawn, got %d",
              WAVE_ENEMIES_PER_WAVE, active);
    cr_assert(g->enemies_spawned == WAVE_ENEMIES_PER_WAVE);
    game_destroy(g);
}

Test(waves, victory_after_target_waves) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_WAVES);
    game_start(g);

    /* clear WIN_WAVES_COUNT waves; must not hit GAME_OVER meanwhile */
    int expected = WAVE_ENEMIES_PER_WAVE;
    for (int w = 0; w < WIN_WAVES_COUNT; w++) {
        kill_all_enemies(g);
        game_update(g, 1.0f / 60.0f);
        expected += WAVE_ENEMIES_PER_WAVE;
        (void)expected;
    }

    cr_assert(g->state == STATE_VICTORY,
              "after clearing %d waves should be VICTORY, got state=%d",
              WIN_WAVES_COUNT, g->state);
    cr_assert(g->waves_completed >= WIN_WAVES_COUNT,
              "waves_completed=%d should be >= %d", g->waves_completed, WIN_WAVES_COUNT);
    game_destroy(g);
}

Test(waves, game_over_if_wave_enemy_reaches_bottom) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_WAVES);
    game_start(g);

    /* teleport an active enemy to the bottom (Grunt speed_y=80) */
    int idx = -1;
    for (int i = 0; i < MAX_ENEMIES; i++)
        if (g->enemies[i].active) { idx = i; break; }
    cr_assert(idx >= 0, "wave should have active enemies");

    g->enemies[idx].y = SCREEN_HEIGHT - 5;
    g->enemies[idx].fy = (float)(SCREEN_HEIGHT - 5);
    for (int i = 0; i < 60; i++) game_update(g, 1.0f / 60.0f);

    cr_assert(g->state == STATE_GAME_OVER,
              "enemy reaching bottom in waves triggers GAME_OVER, state=%d", g->state);
    game_destroy(g);
}

Test(waves, game_over_does_not_advance_remaining_waves) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_WAVES);
    game_start(g);

    /* Drop every enemy past the bottom edge. One large clamped frame (dt=1.0)
       moves even the slowest enemy (Tank, 40px/s) past SCREEN_HEIGHT, so all
       of them are cleaned up and GAME_OVER is set in that same frame. */
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!g->enemies[i].active) continue;
        g->enemies[i].y = SCREEN_HEIGHT - 5;
        g->enemies[i].fy = (float)(SCREEN_HEIGHT - 5);
    }
    game_update(g, 1.0f);

    cr_assert(g->state == STATE_GAME_OVER);
    /* wave progression must NOT run once the game has ended; otherwise a
       fresh wave would spawn after GAME_OVER mutating game state */
    cr_assert(g->waves_completed == 0,
              "game over should not advance waves_completed (got %d)",
              g->waves_completed);
    int active = 0;
    for (int i = 0; i < MAX_ENEMIES; i++)
        if (g->enemies[i].active) active++;
    cr_assert(active == 0, "no new wave should spawn after GAME_OVER, active=%d", active);
    game_destroy(g);
}

Test(formations, starts_with_full_formation) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_FORMATIONS);
    game_start(g);

    cr_assert(g->max_enemies == FORMATION_COLS,
              "level 1 formation size should be FORMATION_COLS, got %d", g->max_enemies);
    cr_assert(g->enemies_spawned == g->max_enemies,
              "formation should spawn immediately");
    int active = 0;
    for (int i = 0; i < MAX_ENEMIES; i++)
        if (g->enemies[i].active) active++;
    cr_assert(active == g->max_enemies);
    game_destroy(g);
}

Test(formations, clearing_advances_level) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_FORMATIONS);
    game_start(g);

    cr_assert(g->current_level == 1);
    kill_all_enemies(g);
    game_update(g, 1.0f / 60.0f);

    cr_assert(g->current_level == 2,
              "clearing level 1 formation should advance to level 2, got %d",
              g->current_level);
    game_destroy(g);
}

Test(formations, next_formation_spawns_after_clear) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_FORMATIONS);
    game_start(g);

    kill_all_enemies(g);
    game_update(g, 1.0f / 60.0f);

    /* level 2 formation: FORMATION_COLS * 2 enemies (<= MAX_ENEMIES) */
    int expected = FORMATION_COLS * 2;
    if (expected > MAX_ENEMIES) expected = MAX_ENEMIES;
    cr_assert(g->max_enemies == expected,
              "level 2 formation size should be %d, got %d", expected, g->max_enemies);
    int active = 0;
    for (int i = 0; i < MAX_ENEMIES; i++)
        if (g->enemies[i].active) active++;
    cr_assert(active == expected,
              "level 2 should spawn %d enemies, got %d", expected, active);
    game_destroy(g);
}

Test(formations, victory_after_target_level) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_FORMATIONS);
    game_start(g);

    /* advance from level 1 up to WIN_FORMATIONS_LEVEL clearing each */
    for (int lvl = 1; lvl <= WIN_FORMATIONS_LEVEL; lvl++) {
        kill_all_enemies(g);
        game_update(g, 1.0f / 60.0f);
    }

    cr_assert(g->state == STATE_VICTORY,
              "clearing through level %d should be VICTORY, got state=%d",
              WIN_FORMATIONS_LEVEL, g->state);
    cr_assert(g->current_level > WIN_FORMATIONS_LEVEL,
              "current_level=%d should exceed target", g->current_level);
    game_destroy(g);
}

Test(formations, size_caps_at_max_enemies) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_FORMATIONS);
    game_start(g);

    /* force very high level and re-advance to check cap inline */
    for (int i = 0; i < 50; i++) {
        kill_all_enemies(g);
        game_update(g, 1.0f / 60.0f);
    }

    cr_assert(g->max_enemies <= MAX_ENEMIES,
              "formation size must never exceed MAX_ENEMIES, got %d", g->max_enemies);
    game_destroy(g);
}

/* ── enriched snapshot ──────────────────────────────────────────── */

Test(snapshot, contains_render_data) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_PROGRESSIVE);
    game_start(g);

    /* forced a precise scene: clear the batch-spawned enemies, then drop
       exactly one active enemy and projectile into known positions. */
    for (int i = 0; i < MAX_ENEMIES; i++)
        g->enemies[i].active = 0;
    g->enemies[0].active = 1;
    g->enemies[0].type = ENEMY_GRUNT;
    g->enemies[0].life = 3;
    g->enemies[0].max_life = 3;
    g->enemies[0].x = 100;
    g->enemies[0].y = 100;
    g->enemies[0].fx = 100.0f;
    g->enemies[0].fy = 100.0f;

    g->projectiles[0].active = 1;
    g->projectiles[0].x = 200;
    g->projectiles[0].y = 300;

    game_state_snapshot_t snap;
    game_get_state(g, &snap);

    cr_assert(snap.mode == MODE_PROGRESSIVE, "snapshot mode should match");
    cr_assert(snap.enemy_count == 1, "snapshot enemy_count should be 1");
    cr_assert(snap.enemy_x[0] == 100 && snap.enemy_y[0] == 100,
              "snapshot enemy position should be copied");
    cr_assert(snap.enemy_type[0] == (int)ENEMY_GRUNT);
    cr_assert(snap.enemy_active[0] == 1);
    cr_assert(snap.projectile_count == 1, "snapshot projectile_count should be 1");
    cr_assert(snap.projectile_x[0] == 200 && snap.projectile_y[0] == 300,
              "snapshot projectile position should be copied");
    cr_assert(snap.projectile_active[0] == 1);
    game_destroy(g);
}

Test(snapshot, inactive_slots_are_zeroed) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_RANDOM);
    game_start(g);

    /* the fresh batch has spawned enemies; deactivate them to assert that
       inactive slots are always surfaced as inactive/zero in the snapshot. */
    for (int i = 0; i < MAX_ENEMIES; i++)
        g->enemies[i].active = 0;
    for (int i = 0; i < MAX_PROJECTILES; i++)
        g->projectiles[i].active = 0;

    game_state_snapshot_t snap;
    game_get_state(g, &snap);

    cr_assert(snap.enemy_count == 0, "no enemies should be reported yet");
    cr_assert(snap.projectile_count == 0, "no projectiles should be reported yet");
    for (int i = 0; i < MAX_ENEMIES; i++)
        cr_assert(snap.enemy_active[i] == 0, "enemy %d should be inactive", i);
    for (int i = 0; i < MAX_PROJECTILES; i++)
        cr_assert(snap.projectile_active[i] == 0, "projectile %d should be inactive", i);
    game_destroy(g);
}
