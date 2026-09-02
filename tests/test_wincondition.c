#include <criterion/criterion.h>
#include "game.h"

Test(wincondition, score_threshold_just_below_does_not_trigger) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_RANDOM);
    game_start(g);

    g->win_cond.type = WIN_SCORE_THRESHOLD;
    g->win_cond.param_int_1 = 500;
    g->score = 499;

    game_update(g, 1.0f / 60.0f);
    cr_assert(g->state == STATE_PLAYING, "below threshold should not trigger victory");
    game_destroy(g);
}

Test(wincondition, score_threshold_at_boundary_triggers) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_RANDOM);
    game_start(g);

    g->win_cond.type = WIN_SCORE_THRESHOLD;
    g->win_cond.param_int_1 = 500;
    g->score = 500;

    game_update(g, 1.0f / 60.0f);
    cr_assert(g->state == STATE_VICTORY, "at threshold should trigger victory");
    game_destroy(g);
}

Test(wincondition, kill_x_within_time) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_RANDOM);
    game_start(g);

    g->win_cond.type = WIN_KILL_X_WITHIN_TIME;
    g->win_cond.param_int_1 = 10;
    g->enemies_destroyed = 9;
    game_update(g, 1.0f / 60.0f);
    cr_assert(g->state == STATE_PLAYING, "9 kills < 10 should not trigger");

    g->enemies_destroyed = 10;
    game_update(g, 1.0f / 60.0f);
    cr_assert(g->state == STATE_VICTORY, "10 kills should trigger");
    game_destroy(g);
}

Test(wincondition, survival_time_triggers_at_boundary) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_RANDOM);
    game_start(g);

    g->win_cond.type = WIN_SURVIVAL_TIME;
    g->win_cond.param_float_1 = 60.0f;
    g->survival_timer = 59.0f;
    game_update(g, 1.0f / 60.0f);
    cr_assert(g->state == STATE_PLAYING, "just under 60s should not trigger");

    g->survival_timer = 60.0f;
    game_update(g, 1.0f / 60.0f);
    cr_assert(g->state == STATE_VICTORY, "at 60s should trigger");
    game_destroy(g);
}

Test(wincondition, survive_n_waves) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_WAVES);
    game_start(g);

    g->win_cond.type = WIN_SURVIVE_N_WAVES;
    g->win_cond.param_int_1 = 5;
    g->waves_completed = 4;
    game_update(g, 1.0f / 60.0f);
    cr_assert(g->state == STATE_PLAYING);

    g->waves_completed = 5;
    game_update(g, 1.0f / 60.0f);
    cr_assert(g->state == STATE_VICTORY, "completing 5 waves should trigger");
    game_destroy(g);
}

Test(wincondition, reach_level_triggers_after_target) {
    game_t *g = game_create(1);
    game_set_mode(g, MODE_PROGRESSIVE);
    game_start(g);

    g->win_cond.type = WIN_REACH_LEVEL;
    g->win_cond.param_int_1 = 5;
    g->current_level = 5;
    game_update(g, 1.0f / 60.0f);
    cr_assert(g->state == STATE_PLAYING, "level 5 is not > 5");

    g->current_level = 6;
    game_update(g, 1.0f / 60.0f);
    cr_assert(g->state == STATE_VICTORY, "level 6 > 5 should trigger");
    game_destroy(g);
}
