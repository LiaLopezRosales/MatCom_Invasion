#include "difficulty.h"

void difficulty_init(difficulty_t *diff, int level) {
    if (!diff) return;

    if (level < 1) level = 1;

    diff->enemy_count = 4 + (level - 1) * 2;
    if (diff->enemy_count > MAX_ENEMIES)
        diff->enemy_count = MAX_ENEMIES;

    diff->spawn_interval = 2.0f - (level - 1) * 0.1f;
    if (diff->spawn_interval < 0.4f)
        diff->spawn_interval = 0.4f;

    diff->base_speed = 80.0f + (level - 1) * 5.0f;
    diff->type_ratio = 0.4f + (level - 1) * 0.03f;
    if (diff->type_ratio > 0.8f)
        diff->type_ratio = 0.8f;
}
