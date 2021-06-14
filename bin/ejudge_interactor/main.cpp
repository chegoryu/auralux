//
// Created by Egor Chunaev on 14.06.2021.
//

#include "library/testlib/testlib.h"

#include "library/game/game.h"
#include "library/game/game_map.h"
#include "library/game/default_players.h"

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

std::vector<std::unique_ptr<IPlayer>> ReadPlayers(int playerCount) {
    std::vector<std::unique_ptr<IPlayer>> players;

    bool hasEjudgePlayer = false;
    for (int i = 0; i < playerCount; ++i) {
        std::string playerType = inf.readToken();

        if (playerType == "afk") {
            players.push_back(std::make_unique<TAFKPlayer>());
        } else if (playerType == "ejudge") {
            assert(!hasEjudgePlayer);
            hasEjudgePlayer = true;
            players.push_back(std::make_unique<TEjudgePlayer>());
        } else {
            assert(false);
        }
    }
    assert(hasEjudgePlayer);

    return players;
}

int main(int argc, char *argv[]) {
    registerInteraction(argc, argv);

    TGame::TConfig config;
    config.GameMap_ = LoadPlanarGraph([]() {
        return inf.readInt();
    });
    TGame game(config);

    auto players = ReadPlayers(static_cast<int>(config.GameMap_.StartPlanets_.size()));
    for (auto& player : players) {
        game.AddPlayer((std::move(player)));
    }

    game.Process();

    return 0;
}

