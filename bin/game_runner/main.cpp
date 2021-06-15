#include <QtCore>
#include <QDebug>

#include "library/game/game.h"
#include "library/game/game_map.h"

#include "library/game/default_players.h"
#include "library/game/text_player.h"

#include <exception>
#include <fstream>
#include <memory>

struct TRunConfig {
    struct TPlayer {
        enum EType {
            DEFAULT = 0,
            PROCESS = 1,
        };

        EType Type_;
        std::string Info_;
    };

    TGame::TConfig GameConfig_;
    std::vector<TPlayer> Players_;

    std::string LogDir_ = "logs";
    long long int ProcessPlayerTimeoutMs_ = 2000;
    long long int ProcessPLayerTimeoutAdditionPerTurnMs_ = 1;
};

TRunConfig LoadRunConfig(const std::string& path) {
    TRunConfig runConfig;

    std::ifstream runConfigStream(path);
    std::string command;
    bool hasGameMapOption = false;
    bool hasMaxStepsOption = false;
    bool hasLogDirOption = false;
    while (runConfigStream >> command) {
        if (command == "MAP") {
            if (hasGameMapOption) {
                throw std::runtime_error("two or more MAP options in config");
            }

            std::string mapPath;
            if (!(runConfigStream >> mapPath)) {
                throw std::runtime_error("failed to read map path");
            }

            std::ifstream mapStream(mapPath);
            runConfig.GameConfig_.GameMap_ =  LoadPlanarGraph([&mapStream]() {
                int x;
                if (!(mapStream >> x)) {
                    throw std::runtime_error("failed to read map");
                }
                return x;
            });
            hasGameMapOption = true;
        } else if (command == "PLAYER") {
            std::string playerType, playerInfo;
            if (!(runConfigStream >> playerType) || !(runConfigStream >> playerInfo)) {
                throw std::runtime_error("failed to read player type or player info");
            }

            if (playerType == "DEFAULT") {
                if (!HasDefaultPlayer(playerInfo)) {
                    std::runtime_error("default player with type: '" + playerInfo + "' doesn't exist");
                }

                runConfig.Players_.push_back({
                   .Type_ = TRunConfig::TPlayer::EType::DEFAULT,
                   .Info_ = playerInfo,
                });
            } else if (playerType == "PROCESS") {
                runConfig.Players_.push_back({
                     .Type_ = TRunConfig::TPlayer::EType::PROCESS,
                     .Info_ = playerInfo,
                 });
            } else {
                throw std::runtime_error("unknown player type: '" + playerType + "'");
            }
        } else if (command == "MAX_STEPS") {
            if (hasMaxStepsOption) {
                throw std::runtime_error("two or more MAX_STEPS options in config");
            }

            if (!(runConfigStream >> runConfig.GameConfig_.MaxSteps_)) {
                throw std::runtime_error("failed to read max steps");
            }
            hasMaxStepsOption = true;
        } else if (command == "LOG_DIR") {
            if (hasLogDirOption) {
                throw std::runtime_error("two or more LOG_DIR options in config");
            }

            if (!(runConfigStream >> runConfig.LogDir_)) {
                throw std::runtime_error("failed to read log dir");
            }
            hasLogDirOption = true;
        } else {
            throw std::runtime_error("unknown option in config: '" + command + "'");
        }
    }

    if (!hasGameMapOption) {
        throw std::runtime_error("MAP option not defined, but it is required option");
    }

    if (runConfig.Players_.size() != runConfig.GameConfig_.GameMap_.StartPlanets_.size()) {
        throw std::runtime_error("number of players and start planets are different");
    }

    return runConfig;
}

class TProcessPlayer : public TTextPlayer {
    std::string ReadLine() override {
        return "0"; // TODO
    }
    void WriteLine(const std::string& line) override {
        // TODO
    }
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        qDebug() << "Usage:" << argv[0] << "<config_file>";
        return 1;
    }

    try {
        TRunConfig runConfig = LoadRunConfig(argv[1]);
    } catch (const std::exception& e) {
        qDebug() << "Failed to load run config:" << e.what();
        return 1;
    }

    return 0;
}
