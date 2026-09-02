#include <criterion/criterion.h>
#include "difficulty.h"
#include "types.h"

Test(difficulty, level1_is_minimum) {
    difficulty_t d;
    difficulty_init(&d, 1);
    cr_assert(d.enemy_count == 6, "level 1 should have 6 enemies");
    cr_assert(d.enemy_count <= MAX_ENEMIES);
    cr_assert(d.spawn_interval > 0.0f, "spawn interval must be positive");
    cr_assert(d.base_speed > 0.0f, "base speed must be positive");
}

Test(difficulty, high_level_saturates_enemy_count) {
    difficulty_t d;
    difficulty_init(&d, 1000);
    cr_assert(d.enemy_count == MAX_ENEMIES,
              "very high level should be clamped to MAX_ENEMIES, got %d", d.enemy_count);
    cr_assert(d.enemy_count <= MAX_ENEMIES);
}

Test(difficulty, parameters_clamped_to_valid_ranges) {
    for (int level = 1; level <= 500; level++) {
        difficulty_t d;
        difficulty_init(&d, level);
        cr_assert(d.enemy_count >= 1 && d.enemy_count <= MAX_ENEMIES,
                  "enemy_count out of range at level %d: %d", level, d.enemy_count);
        cr_assert(d.spawn_interval >= 0.4f && d.spawn_interval <= 2.0f,
                  "spawn_interval out of range at level %d: %f", level, (double)d.spawn_interval);
        cr_assert(d.base_speed > 0.0f, "speed must be positive at level %d", level);
    }
}

Test(difficulty, parameters_grow_monotonically) {
    difficulty_t prev;
    difficulty_init(&prev, 1);
    int prev_count = prev.enemy_count;
    float prev_interval = prev.spawn_interval;
    float prev_speed = prev.base_speed;

    for (int level = 2; level <= 20; level++) {
        difficulty_t d;
        difficulty_init(&d, level);
        cr_assert(d.enemy_count >= prev_count,
                  "enemy_count should not decrease: level %d %d < prev %d",
                  level, d.enemy_count, prev_count);
        cr_assert(d.spawn_interval <= prev_interval + 1e-6,
                  "spawn_interval should not increase: level %d %f > prev %f",
                  level, (double)d.spawn_interval, (double)prev_interval);
        cr_assert(d.base_speed >= prev_speed,
                  "base_speed should not decrease: level %d %f < prev %f",
                  level, (double)d.base_speed, (double)prev_speed);
        prev_count = d.enemy_count;
        prev_interval = d.spawn_interval;
        prev_speed = d.base_speed;
    }
}

Test(difficulty, level_below_one_is_clamped) {
    difficulty_t d;
    difficulty_init(&d, 0);
    cr_assert(d.enemy_count >= 1, "level 0 should be clamped to at least 1 enemy");
    cr_assert(d.spawn_interval > 0.0f);
}
