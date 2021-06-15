#include <QtCore>
#include <QDebug>

#include "library/game/game.h"

struct TRunConfig {

};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        qDebug() << "Usage:" << argv[0] << "<config_file>";
        return 1;
    }

    std::string configPath = argv[1];

    return 0;
}
