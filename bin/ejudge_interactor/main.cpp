//
// Created by Egor Chunaev on 14.06.2021.
//

#include "library/testlib/testlib.h"

#include "library/game/game.h"

#include "library/game/default_players.h"
#include "library/game/text_player.h"


class TEjudgePlayer : public TTextPlayer {
public:
    using TTextPlayer::TTextPlayer;

    std::string ReadLine() override {
        return ouf.readLine();
    }
    virtual void WriteLine(const std::string& line) override {
        std::cout << line << std::endl;
    }
};

int main(int argc, char *argv[]) {
    registerInteraction(argc, argv);

    TGame::TConfig config;
    config.MaxSteps_ = 10;
    TGame game(config);

    game.AddPlayer(std::make_unique<TEjudgePlayer>());
    game.AddPlayer(std::make_unique<TAFKPlayer>());
    game.Process();

    return 0;
}

