//
// Created by Egor Chunaev on 14.06.2021.
//

#include "library/game/game.h"
#include "library/game/game_map.h"
#include "library/game/default_players.h"

#include "library/ejudge/ejudge_player.h"

class TEjudgePlayer : public TTextPlayer {
public:
    using TTextPlayer::TTextPlayer;

    std::string ReadLine() override {
        return ouf.readLine();
    }
    void WriteLine(const std::string& line) override {
        std::cout << line << std::endl;
    }
};

int main(int argc, char *argv[]) {
    registerInteraction(argc, argv);

    TGame::TConfig config;
    config.GameMap_ = LoadPlanarGraph([]() {
        return inf.readInt();
    });
    TGame game(config);

    game.AddPlayer(std::make_unique<TEjudgePlayer>());
    for (size_t i = 0; i < config.GameMap_.StartPlanets_.size() - 1; ++i) {
        game.AddPlayer(std::make_unique<TAFKPlayer>());
    }

    game.Process();

    return 0;
}

