#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "game.h"

void scheduler_generate_order(game_t *game);
enemy_type_t scheduler_next_type(game_t *game);

#endif
