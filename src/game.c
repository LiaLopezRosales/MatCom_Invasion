#include "game.h"
#include "scheduler.h"
#include "enemy.h"
#include "projectile.h"
#include "collision.h"
#include "difficulty.h"
#include <stdlib.h>
#include <string.h>

game_t *game_create(unsigned int seed) {
    game_t *game = (game_t *)calloc(1, sizeof(game_t));
    if (!game) return NULL;

    game->rng_state = seed;
    game->state = STATE_MENU;
    game->lives = 3;
    game->high_score = 0;
    game->current_level = 1;

    return game;
}

void game_destroy(game_t *game) {
    free(game);
}

void game_set_mode(game_t *game, game_mode_t mode) {
    if (!game) return;
    game->mode = mode;
}

static void configure_win_condition(game_t *game) {
    switch (game->mode) {
        case MODE_PROGRESSIVE:
            game->win_cond.type = WIN_SCORE_THRESHOLD;
            game->win_cond.param_int_1 = 500;
            break;
        case MODE_ALTERNATE:
            game->win_cond.type = WIN_KILL_X_WITHIN_TIME;
            game->win_cond.param_int_1 = 10;
            game->win_cond.param_float_1 = 30.0f;
            break;
        case MODE_RANDOM:
            game->win_cond.type = WIN_SURVIVAL_TIME;
            game->win_cond.param_float_1 = 60.0f;
            break;
        case MODE_WAVES:
            game->win_cond.type = WIN_SURVIVE_N_WAVES;
            game->win_cond.param_int_1 = 5;
            break;
        case MODE_FORMATIONS:
            game->win_cond.type = WIN_REACH_LEVEL;
            game->win_cond.param_int_1 = 5;
            break;
    }
}

void game_start(game_t *game) {
    if (!game) return;

    memset(game->projectiles, 0, sizeof(game->projectiles));
    memset(game->enemies, 0, sizeof(game->enemies));

    game->state = STATE_PLAYING;
    game->current_level = 1;
    game->score = 0;
    game->lives = 3;
    game->invuln_timer = 0.0f;
    game->enemies_spawned = 0;
    game->enemies_destroyed = 0;
    game->spawn_timer = 0.0f;
    game->survival_timer = 0.0f;
    game->waves_completed = 0;

    game->ship.x = SCREEN_WIDTH / 2;
    game->ship.y = SCREEN_HEIGHT - 100;

    difficulty_init(&game->difficulty, game->current_level);
    game->max_enemies = game->difficulty.enemy_count;

    configure_win_condition(game);
    scheduler_generate_order(game);
}

void game_fire(game_t *game) {
    if (!game || game->state != STATE_PLAYING) return;

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!game->projectiles[i].active) {
            game->projectiles[i].x = game->ship.x;
            game->projectiles[i].y = game->ship.y - 15;
            game->projectiles[i].active = 1;
            return;
        }
    }
}

void game_set_ship(game_t *game, int x, int y) {
    if (!game || game->state != STATE_PLAYING) return;

    game->ship.x = x;
    game->ship.y = y;

    if (game->ship.x < 0) game->ship.x = 0;
    if (game->ship.x > SCREEN_WIDTH) game->ship.x = SCREEN_WIDTH;
    if (game->ship.y < 0) game->ship.y = 0;
    if (game->ship.y > SCREEN_HEIGHT) game->ship.y = SCREEN_HEIGHT;
}

static void check_win_condition(game_t *game) {
    switch (game->win_cond.type) {
        case WIN_SCORE_THRESHOLD:
            if (game->score >= game->win_cond.param_int_1)
                game->state = STATE_VICTORY;
            break;
        case WIN_KILL_X_WITHIN_TIME:
            if (game->enemies_destroyed >= game->win_cond.param_int_1)
                game->state = STATE_VICTORY;
            break;
        case WIN_SURVIVAL_TIME:
            if (game->survival_timer >= game->win_cond.param_float_1)
                game->state = STATE_VICTORY;
            break;
        case WIN_SURVIVE_N_WAVES:
            if (game->waves_completed >= game->win_cond.param_int_1)
                game->state = STATE_VICTORY;
            break;
        case WIN_REACH_LEVEL:
            if (game->current_level > game->win_cond.param_int_1)
                game->state = STATE_VICTORY;
            break;
    }
}

void game_update(game_t *game, float dt) {
    if (!game || game->state != STATE_PLAYING) return;

    // Sanitize dt: reject negative and cap oversized frames so timers and
    // movement stay deterministic and never run backwards (Phase 2).
    if (dt < 0.0f) dt = 0.0f;
    if (dt > 1.0f) dt = 1.0f;

    game->survival_timer += dt;

    if (game->invuln_timer > 0.0f)
        game->invuln_timer -= dt;

    game->spawn_timer += dt;
    if (game->enemies_spawned < game->max_enemies &&
        game->spawn_timer >= game->difficulty.spawn_interval) {
        game->spawn_timer = 0.0f;
        spawn_enemy(game);
    }

    update_projectiles(game, dt);
    update_enemies(game, dt);
    check_collisions(game);
    check_win_condition(game);
}

void game_pause(game_t *game) {
    if (game && game->state == STATE_PLAYING)
        game->state = STATE_PAUSED;
}

void game_resume(game_t *game) {
    if (game && game->state == STATE_PAUSED)
        game->state = STATE_PLAYING;
}

void game_get_state(const game_t *game, game_state_snapshot_t *snap) {
    if (!game || !snap) return;

    memset(snap, 0, sizeof(*snap));
    snap->state = game->state;
    snap->score = game->score;
    snap->high_score = game->high_score;
    snap->lives = game->lives;
    snap->current_level = game->current_level;
    snap->ship_x = game->ship.x;
    snap->ship_y = game->ship.y;

    snap->projectile_count = 0;
    for (int i = 0; i < MAX_PROJECTILES; i++)
        if (game->projectiles[i].active)
            snap->projectile_count++;

    snap->enemy_count = 0;
    for (int i = 0; i < MAX_ENEMIES; i++)
        if (game->enemies[i].active)
            snap->enemy_count++;

    snap->enemies_destroyed = game->enemies_destroyed;
}
