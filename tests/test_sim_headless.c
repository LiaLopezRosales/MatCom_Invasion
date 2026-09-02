#include <criterion/criterion.h>
#include "game.h"

#define TICKS_MAX (60 * 120)

static int run_sim_to_terminal(game_t *g) {
    int ticks = 0;
    // run until terminal state or tick budget exhausted
    for (; ticks < TICKS_MAX; ticks++) {
        if (g->state != STATE_PLAYING)
            break;
        game_update(g, 1.0f / 60.0f);
    }
    return ticks;
}

static void check_invariants(game_t *g) {
    // active enemy count never exceeds max_enemies
    int active = 0;
    for (int i = 0; i < MAX_ENEMIES; i++)
        if (g->enemies[i].active) active++;
    cr_assert(active <= g->max_enemies,
              "active enemies %d exceed max_enemies %d", active, g->max_enemies);

    // no enemy below the screen floor is left active (should be cleaned up)
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (g->enemies[i].active) {
            cr_assert(g->enemies[i].y >= -50 && g->enemies[i].y <= SCREEN_HEIGHT + 50,
                      "enemy %d out of bounds y=%d", i, g->enemies[i].y);
        }
    }

    // terminal states are always valid
    cr_assert(g->state == STATE_PLAYING || g->state == STATE_GAME_OVER ||
              g->state == STATE_VICTORY,
              "invalid terminal state %d", g->state);
}

Test(sim_progressive, invariants_and_terminal) {
    game_t *g = game_create(12345);
    game_set_mode(g, MODE_PROGRESSIVE);
    game_start(g);
    int ticks = run_sim_to_terminal(g);
    cr_assert(ticks < TICKS_MAX, "progressive should converge to terminal state");
    check_invariants(g);
    cr_assert(g->state == STATE_GAME_OVER || g->state == STATE_VICTORY,
              "expected terminal state, got %d", g->state);
    game_destroy(g);
}

Test(sim_alternate, invariants_and_terminal) {
    game_t *g = game_create(12346);
    game_set_mode(g, MODE_ALTERNATE);
    game_start(g);
    int ticks = run_sim_to_terminal(g);
    cr_assert(ticks < TICKS_MAX, "alternate should converge");
    check_invariants(g);
    game_destroy(g);
}

Test(sim_random, invariants_and_terminal) {
    game_t *g = game_create(12347);
    game_set_mode(g, MODE_RANDOM);
    game_start(g);
    int ticks = run_sim_to_terminal(g);
    cr_assert(ticks < TICKS_MAX, "random should converge");
    check_invariants(g);
    game_destroy(g);
}

Test(sim_waves, invariants_and_terminal) {
    game_t *g = game_create(12348);
    game_set_mode(g, MODE_WAVES);
    game_start(g);
    int ticks = run_sim_to_terminal(g);
    cr_assert(ticks < TICKS_MAX, "waves should converge");
    check_invariants(g);
    game_destroy(g);
}

Test(sim_formations, invariants_and_terminal) {
    game_t *g = game_create(12349);
    game_set_mode(g, MODE_FORMATIONS);
    game_start(g);
    int ticks = run_sim_to_terminal(g);
    cr_assert(ticks < TICKS_MAX, "formations should converge");
    check_invariants(g);
    game_destroy(g);
}

Test(sim_reproducibility, same_seed_yields_same_final_state) {
    game_state_snapshot_t a, b;

    game_t *g1 = game_create(777);
    game_set_mode(g1, MODE_RANDOM);
    game_start(g1);
    for (int i = 0; i < 60 * 20; i++)
        if (g1->state == STATE_PLAYING) game_update(g1, 1.0f / 60.0f);
    game_get_state(g1, &a);
    game_destroy(g1);

    game_t *g2 = game_create(777);
    game_set_mode(g2, MODE_RANDOM);
    game_start(g2);
    for (int i = 0; i < 60 * 20; i++)
        if (g2->state == STATE_PLAYING) game_update(g2, 1.0f / 60.0f);
    game_get_state(g2, &b);
    game_destroy(g2);

    cr_assert(a.state == b.state, "state differs: %d vs %d", a.state, b.state);
    cr_assert(a.score == b.score, "score differs: %d vs %d", a.score, b.score);
    cr_assert(a.lives == b.lives, "lives differ: %d vs %d", a.lives, b.lives);
    cr_assert(a.enemies_destroyed == b.enemies_destroyed,
              "destroyed differs: %d vs %d", a.enemies_destroyed, b.enemies_destroyed);
}

Test(sim_multi_seed, no_crashes) {
    for (unsigned int seed = 1; seed <= 20; seed++) {
        game_t *g = game_create(seed);
        game_set_mode(g, MODE_WAVES);
        game_start(g);
        for (int i = 0; i < 60 * 30; i++)
            if (g->state == STATE_PLAYING) game_update(g, 1.0f / 60.0f);
        check_invariants(g);
        game_destroy(g);
    }
}
