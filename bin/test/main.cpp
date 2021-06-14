#include "library/game/game.h"

#include "library/game/default_players.h"

int main(int argc, char *argv[]) {
    TGame::TConfig config;
    config.MaxSteps_ = 10;

    TGame game(config);

    game.AddPlayer(std::make_unique<TAFKPlayer>());
    game.AddPlayer(std::make_unique<TStdinStdoutPlayer>());
    game.Process();

    return 0;
}
