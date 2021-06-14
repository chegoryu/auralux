// #include <QCoreApplication>
// #include <QDebug>

#include "library/game/game.h"
#include "library/game/player.h"

class TFakePlayer : public TPlayer {
public:
    using TPlayer::TPlayer;

    void SendGameInfo(const TGameInfo& gameInfo) override {

    }
    TPlayerMove GetMove(const TGameState& gameState) override {
        return {};
    }
};

int main(int argc, char *argv[]) {
    TGame::TConfig config;
    config.MaxSteps_ = 10;

    TGame game(config);

    game.AddPlayer(std::make_unique<TFakePlayer>());
    game.Process();

    return 0;
    // QCoreApplication a(argc, argv);
    // qDebug() << "Hello World";
    // return QCoreApplication::exec();


}
