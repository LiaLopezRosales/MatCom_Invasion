#ifndef DIFFICULTY_H
#define DIFFICULTY_H

#include "types.h"

/** Initialise difficulty parameters for the given level (used by batch-based modes). */
void difficulty_init(difficulty_t *diff, int level);

#endif
