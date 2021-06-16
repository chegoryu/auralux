//
// Created by Egor Chunaev on 14.06.2021.
//

#include "library/game/game.h"
#include "library/game/game_map.h"
#include "library/game/game_result.h"
#include "library/game/game_visualizer.h"

#include "library/game/default_players.h"

#include <fstream>
#include <iostream>

TGameMap ReadMapFromFile(const std::string fileName) {
    std::ifstream stream(fileName);
    return LoadPlanarGraph([&stream]() {
        int x;
        stream >> x;
        return x;
    });
}

TGame GetSmallGame() {
    TGame::TConfig config;
    config.GameMap_ = ReadMapFromFile("small_map.txt");

    config.PlanetProductionMultiply_ = 100;

    TGame game(config);

    game.AddPlayer(std::make_unique<TAFKPlayer>());
    game.AddPlayer(std::make_unique<TStdinStdoutPlayer>(config.MaxPlayerShipMovesPerStep_));

    return std::move(game);
}

TGame GetBigGame() {
    TGame::TConfig config;
    config.GameMap_ = ReadMapFromFile("symmetrical_big_map.txt");

    TGame game(config);

    game.AddPlayer(std::make_unique<TAggressiveExpansionPlayer>(TAggressiveExpansionPlayer::EGameStyle::NEAREST));
    game.AddPlayer(std::make_unique<TAggressiveExpansionPlayer>(TAggressiveExpansionPlayer::EGameStyle::NEAREST));
    game.AddPlayer(std::make_unique<TAggressiveExpansionPlayer>(TAggressiveExpansionPlayer::EGameStyle::NEAREST));
    game.AddPlayer(std::make_unique<TAFKPlayer>());

    return std::move(game);
}

int main(int argc, char *argv[]) {
    auto startTime = clock();

    TGame game = GetBigGame();
    game.Process();

    auto errors = game.GetGameLogger().GetErrors();
    for (const auto& error : errors) {
        std::cout << error << '\n';
    }

    PrintHumanReadableGameResult(std::cout, GetGameResult(game.GetGameLogger()));
    PrintGameState(std::cout, game.GetGameLogger().GetFinalGameState());

    {
        std::ofstream stream("visualizer_log.txt");
        PrintGameLogInVisualizerFormat(stream, game.GetGameConfig().GameMap_, game.GetGameLogger());
    }

    std::cerr << "Total time: " << (clock() - startTime) / static_cast<double>(CLOCKS_PER_SEC) << '\n';

    return 0;
}
