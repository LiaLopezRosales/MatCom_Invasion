#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "game.h"

/**
 * Fill the spawn-order buffer for the current mode and difficulty level.
 * Called once per batch (Progressive/Alternate/Random) or per wave (Waves/Formations).
 */
void scheduler_generate_order(game_t *game);

/** Return the next enemy type from the pre-generated spawn order (FIFO). */
enemy_type_t scheduler_next_type(game_t *game);

#endif
