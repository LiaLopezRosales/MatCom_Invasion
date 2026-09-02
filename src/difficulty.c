#include "difficulty.h"
#include "balance.h"

void difficulty_init(difficulty_t *diff, int level) {
    if (!diff) return;

    if (level < 1) level = 1;

    diff->enemy_count = DIFF_BASE_ENEMIES + (level - 1) * DIFF_ENEMIES_PER_LVL;
    if (diff->enemy_count > MAX_ENEMIES)
        diff->enemy_count = MAX_ENEMIES;

    diff->spawn_interval = DIFF_BASE_INTERVAL - (level - 1) * DIFF_INTERVAL_DECAY;
    if (diff->spawn_interval < DIFF_MIN_INTERVAL)
        diff->spawn_interval = DIFF_MIN_INTERVAL;

    diff->base_speed = DIFF_BASE_SPEED + (level - 1) * DIFF_SPEED_PER_LVL;
    diff->type_ratio = DIFF_BASE_TYPE_RATIO + (level - 1) * DIFF_TYPE_RATIO_INC;
    if (diff->type_ratio > DIFF_MAX_TYPE_RATIO)
        diff->type_ratio = DIFF_MAX_TYPE_RATIO;
}
