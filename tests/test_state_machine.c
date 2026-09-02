#include <criterion/criterion.h>
#include "game.h"

Test(state_machine, starts_in_menu) {
    game_t *g = game_create(1);
    cr_assert(g->state == STATE_MENU, "game should start in MENU");
    game_destroy(g);
}

Test(state_machine, start_transitions_to_playing) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_PROGRESSIVE);
    game_start(g);
    cr_assert(g->state == STATE_PLAYING);
    game_destroy(g);
}

Test(state_machine, pause_resume_cycles) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_RANDOM);
    game_start(g);

    for (int i = 0; i < 10; i++) {
        game_pause(g);
        cr_assert(g->state == STATE_PAUSED, "pause should set PAUSED");
        game_resume(g);
        cr_assert(g->state == STATE_PLAYING, "resume should restore PLAYING");
    }
    game_destroy(g);
}

Test(state_machine, update_in_pause_does_not_advance) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_PROGRESSIVE);
    game_start(g);
    game_pause(g);

    float survival_before = g->survival_timer;
    for (int i = 0; i < 60; i++)
        game_update(g, 1.0f / 60.0f);

    cr_assert(g->survival_timer == survival_before,
              "game_update should not advance timers while PAUSED");
    cr_assert(g->state == STATE_PAUSED);
    game_destroy(g);
}

Test(state_machine, update_in_menu_is_ignored) {
    game_t *g = game_create(1);
    cr_assert(g->state == STATE_MENU);
    game_update(g, 1.0f / 60.0f); // must not crash
    cr_assert(g->state == STATE_MENU, "update in MENU should be a no-op");
    game_destroy(g);
}

Test(state_machine, restart_after_game_over_is_clean) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_PROGRESSIVE);
    game_start(g);

    // force game over
    g->lives = 0;
    // simulate by placing an enemy on the ship
    g->enemies[0].active = 1;
    g->enemies[0].type = ENEMY_GRUNT;
    g->enemies[0].x = g->ship.x;
    g->enemies[0].y = g->ship.y;
    g->enemies[0].fx = (float)g->ship.x;
    g->enemies[0].fy = (float)g->ship.y;
    g->invuln_timer = 0.0f;
    for (int i = 0; i < 5; i++)
        game_update(g, 1.0f / 60.0f);

    cr_assert(g->state == STATE_GAME_OVER);

    // restart
    game_start(g);
    cr_assert(g->state == STATE_PLAYING);
    cr_assert(g->lives == 3, "restart should reset lives to 3");
    cr_assert(g->score == 0, "restart should reset score");
    game_destroy(g);
}
