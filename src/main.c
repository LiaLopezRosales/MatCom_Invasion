#include "game.h"
#include "balance.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    unsigned int seed = DEFAULT_SEED;
    if (argc > 1)
        seed = (unsigned int)strtoul(argv[1], NULL, 10);

    game_t *game = game_create(seed);
    if (!game) {
        fprintf(stderr, "Failed to create game\n");
        return 1;
    }

    game_set_mode(game, MODE_PROGRESSIVE);
    game_start(game);

    printf("MatCom Invasion — headless simulation (seed=%u)\n", seed);

    float time = 0.0f;
    int steps = 0;
    while (game->state == STATE_PLAYING && steps < MAX_HEADLESS_STEPS) {
        game_update(game, 1.0f / 60.0f);
        game_set_ship(game, game->ship.x + 1, game->ship.y);

        if (steps % 120 == 0) {
            game_state_snapshot_t snap;
            game_get_state(game, &snap);
            printf("[%6.1fs] state=%d lives=%d score=%d level=%d enemies=%d\n",
                   (double)time, snap.state, snap.lives, snap.score,
                   snap.current_level, snap.enemy_count);
        }

        time += 1.0f / 60.0f;
        steps++;
    }

    game_state_snapshot_t snap;
    game_get_state(game, &snap);
    printf("Final: state=%d lives=%d score=%d destroyed=%d (steps=%d)\n",
           snap.state, snap.lives, snap.score, snap.enemies_destroyed, steps);

    game_destroy(game);
    return 0;
}
