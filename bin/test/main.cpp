#include "library/game/game.h"

#include "library/game/default_players.h"

int main(int argc, char *argv[]) {
    TGame::TConfig config;

    config.GameMap_ = {
        .StartPlanets_ = {1, 4},
        .Dists_ = {
            {0, 1, 2, 3},
            {1, 0, 2, 3},
            {2, 2, 0, 3},
            {3, 3, 3, 0},
        },
    };

    config.PlanetProductionMultiply_ = 100;

    TGame game(config);

    game.AddPlayer(std::make_unique<TAFKPlayer>());
    game.AddPlayer(std::make_unique<TStdinStdoutPlayer>());
    game.Process();

    return 0;
}
