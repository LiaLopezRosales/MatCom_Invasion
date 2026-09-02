#include "game.h"
#include "scheduler.h"
#include "enemy.h"
#include "projectile.h"
#include "collision.h"
#include "difficulty.h"
#include "balance.h"
#include <stdlib.h>
#include <string.h>

game_t *game_create(unsigned int seed) {
    game_t *game = (game_t *)calloc(1, sizeof(game_t));
    if (!game) return NULL;

    game->rng_state = seed;
    game->state = STATE_MENU;
    game->lives = INITIAL_LIVES;
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
            game->win_cond.param_int_1 = WIN_PROGRESSIVE_SCORE;
            break;
        case MODE_ALTERNATE:
            game->win_cond.type = WIN_KILL_X_WITHIN_TIME;
            game->win_cond.param_int_1 = WIN_ALTERNATE_KILLS;
            game->win_cond.param_float_1 = WIN_ALTERNATE_TIME;
            break;
        case MODE_RANDOM:
            game->win_cond.type = WIN_SURVIVAL_TIME;
            game->win_cond.param_float_1 = WIN_RANDOM_SURVIVAL;
            break;
        case MODE_WAVES:
            game->win_cond.type = WIN_SURVIVE_N_WAVES;
            game->win_cond.param_int_1 = WIN_WAVES_COUNT;
            break;
        case MODE_FORMATIONS:
            game->win_cond.type = WIN_REACH_LEVEL;
            game->win_cond.param_int_1 = WIN_FORMATIONS_LEVEL;
            break;
    }
}

static void spawn_immediate_batch(game_t *game);
static int compute_formation_size(int level);
static int count_active_enemies(const game_t *game);
static void check_batch_progression(game_t *game);

void game_start(game_t *game) {
    if (!game) return;

    memset(game->projectiles, 0, sizeof(game->projectiles));
    memset(game->enemies, 0, sizeof(game->enemies));

    game->state = STATE_PLAYING;
    game->current_level = 1;
    game->score = 0;
    game->lives = INITIAL_LIVES;
    game->invuln_timer = 0.0f;
    game->enemies_spawned = 0;
    game->enemies_destroyed = 0;
    game->spawn_timer = 0.0f;
    game->survival_timer = 0.0f;
    game->waves_completed = 0;

    game->ship.x = SCREEN_WIDTH / 2;
    game->ship.y = SCREEN_HEIGHT - SHIP_SPAWN_Y_OFFSET;

    difficulty_init(&game->difficulty, game->current_level);

    configure_win_condition(game);

    if (game->mode == MODE_WAVES) {
        game->max_enemies = WAVE_ENEMIES_PER_WAVE;
        game->waves_completed = 0;
    } else if (game->mode == MODE_FORMATIONS) {
        game->current_level = 1;
        game->max_enemies = compute_formation_size(game->current_level);
    } else {
        game->max_enemies = game->difficulty.enemy_count;
    }
    spawn_immediate_batch(game);
}

static void spawn_immediate_batch(game_t *game) {
    game->enemies_spawned = 0;
    scheduler_generate_order(game);
    for (int i = 0; i < game->max_enemies; i++)
        spawn_enemy(game);
}

static int compute_formation_size(int level) {
    int size = FORMATION_COLS * level;
    if (size > MAX_ENEMIES) size = MAX_ENEMIES;
    if (size < 1) size = 1;
    return size;
}

static int count_active_enemies(const game_t *game) {
    int count = 0;
    for (int i = 0; i < MAX_ENEMIES; i++)
        if (game->enemies[i].active) count++;
    return count;
}

void game_fire(game_t *game) {
    if (!game || game->state != STATE_PLAYING) return;

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!game->projectiles[i].active) {
            game->projectiles[i].x = game->ship.x;
            game->projectiles[i].y = game->ship.y - FIRE_Y_OFFSET;
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

    if (dt < 0.0f) dt = 0.0f;
    if (dt > 1.0f) dt = 1.0f;

    game->survival_timer += dt;

    if (game->invuln_timer > 0.0f)
        game->invuln_timer -= dt;

    update_projectiles(game, dt);
    update_enemies(game, dt);
    check_collisions(game);
    check_batch_progression(game);
    check_win_condition(game);
}

static void check_batch_progression(game_t *game) {
    if (game->state != STATE_PLAYING) return;
    if (count_active_enemies(game) > 0) return;
    if (game->enemies_spawned < game->max_enemies) return;

    switch (game->mode) {
        case MODE_WAVES:
            game->waves_completed++;
            break;

        case MODE_FORMATIONS:
            game->current_level++;
            game->max_enemies = compute_formation_size(game->current_level);
            break;

        case MODE_PROGRESSIVE:
        case MODE_ALTERNATE:
        case MODE_RANDOM:
            game->current_level++;
            difficulty_init(&game->difficulty, game->current_level);
            game->max_enemies = game->difficulty.enemy_count;
            break;
    }

    spawn_immediate_batch(game);
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
    for (int i = 0; i < MAX_ENEMIES; i++) {
        snap->enemy_x[i] = game->enemies[i].x;
        snap->enemy_y[i] = game->enemies[i].y;
        snap->enemy_active[i] = game->enemies[i].active;
        snap->enemy_type[i] = (int)game->enemies[i].type;
        snap->enemy_life[i] = game->enemies[i].life;
        if (game->enemies[i].active)
            snap->enemy_count++;
    }

    snap->projectile_count = 0;
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        snap->projectile_x[i] = game->projectiles[i].x;
        snap->projectile_y[i] = game->projectiles[i].y;
        snap->projectile_active[i] = game->projectiles[i].active;
        if (game->projectiles[i].active)
            snap->projectile_count++;
    }

    snap->enemies_destroyed = game->enemies_destroyed;
    snap->survival_timer = game->survival_timer;
    snap->mode = game->mode;
}
