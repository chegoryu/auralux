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

std::pair<std::vector<std::unique_ptr<IPlayer>>, int> ReadPlayers(int playerCount) {
    std::vector<std::unique_ptr<IPlayer>> players;

    int ejudgePlayerId = -1;
    for (int i = 0; i < playerCount; ++i) {
        std::string playerType = inf.readToken();

        if (playerType == "afk") {
            players.push_back(std::make_unique<TAFKPlayer>());
        } else if (playerType == "ejudge") {
            assert(ejudgePlayerId == -1);
            ejudgePlayerId = i;
            players.push_back(std::make_unique<TEjudgePlayer>());
        } else {
            assert(false);
        }
    }
    assert(ejudgePlayerId != -1);

    return {std::move(players), ejudgePlayerId};
}

int main(int argc, char *argv[]) {
    registerInteraction(argc, argv);

    TGame::TConfig config;
    config.GameMap_ = LoadPlanarGraph([]() {
        return inf.readInt();
    });
    TGame game(config);

    auto [players, ejudgePlayerId] = ReadPlayers(static_cast<int>(config.GameMap_.StartPlanets_.size()));
    for (auto& player : players) {
        game.AddPlayer((std::move(player)));
    }
    game.Process();

    const auto& gameLogger = game.GetGameLogger();

    const auto& errors = gameLogger.GetErrors();
    const auto& finalGameState = gameLogger.GetFinalGameState();

    PrintGameState(tout, finalGameState);
    for (const auto& error : errors) {
        tout << error << std::endl;
    }

    if (finalGameState.AlivePlayers_.size() > 1) {
        quitf(_wa, "No winner (too many alive players)");
    }
    if (finalGameState.AlivePlayers_.empty()) {
        quitf(_wa, "No winner (all players are not alive)");
    }
    int winner = *finalGameState.AlivePlayers_.begin();

    if (winner -1 != ejudgePlayerId) {
        quitf(_wa, "Winner is %d, but ejudge player id is %d", winner, ejudgePlayerId + 1);
    }

    if (!errors.empty()) {
        quitf(_wa, "Errors not empty (strict format checking in ejudge)");
    }

    quitf(_ok, "Winner is %d", ejudgePlayerId + 1);
}

