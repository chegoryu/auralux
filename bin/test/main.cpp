// #include <QCoreApplication>
// #include <QDebug>

#include "library/game/game.h"
#include "library/game/player.h"

class TFakePlayer : public TPlayer {
public:
    using TPlayer::TPlayer;

    TPlayerMove GetMove(const TGameInfo& gameInfo) override {
        return {};
    }
};

int main(int argc, char *argv[]) {
    TGame::TConfig config;
    config.SomeVal_ = -1;

    TGame game(config);

    game.AddPlayer(std::make_unique<TFakePlayer>());
    game.Step();

    return 0;
    // QCoreApplication a(argc, argv);
    // qDebug() << "Hello World";
    // return QCoreApplication::exec();


}
