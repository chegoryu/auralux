#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QProcess>

#include "library/game/game.h"
#include "library/game/game_map.h"
#include "library/game/game_result.h"
#include "library/game/game_visualizer.h"

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
    bool DisableVisualizerLog_ = false;

    long long int ProcessPlayerStartTimeoutMs_ = 5000;
    long long int ProcessPlayerFinishTimeoutMs_ = 1000;
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
    bool hashDisableVisualizerOption = false;
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
        } else if (command == "DISABLE_VISUALIZER_LOG") {
            if (hashDisableVisualizerOption) {
                throw std::runtime_error("two or more DISABLE_VISUALIZER_LOG options in config");
            }

            runConfig.DisableVisualizerLog_ = true;
            hashDisableVisualizerOption = true;
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

class TIODevicePlayer : public TTextPlayer {
public:
    TIODevicePlayer(
        int maxPlayerMovesPerReadMove,
        QIODevice& ioDevice,
        qint64 timeoutMs,
        qint64 timeoutAdditionPerTurnMs
    )
        : TTextPlayer(maxPlayerMovesPerReadMove)
        , IoDevice_(ioDevice)
        , TimeoutMs_(timeoutMs)
        , TimeoutAdditionPerTurnMs_(timeoutAdditionPerTurnMs)
        , TimeLeftMs_(TimeoutMs_)
        , Timeouted_(false)
    {}

    std::string ReadLine() override {
        if (Timeouted_) {
            return TTextPlayer::DISQUALIFY_ME;
        }

        QDateTime startTime = QDateTime::currentDateTime();
        QDateTime now = startTime;
        QDateTime deadline = now.addMSecs(TimeLeftMs_);

        bool readyRead = true;
        while (!IoDevice_.canReadLine() && readyRead && now < deadline) {
            now = QDateTime::currentDateTime();
            readyRead = IoDevice_.waitForReadyRead(now.msecsTo(deadline));
        }

        if (!IoDevice_.canReadLine()) {
            Timeouted_ = true;
            return TTextPlayer::DISQUALIFY_ME;
        }

        now = QDateTime::currentDateTime();
        qint64 timeElapsedMs = startTime.msecsTo(now);
        if (timeElapsedMs > TimeLeftMs_) {
            Timeouted_ = true;
            return TTextPlayer::DISQUALIFY_ME;
        }
        TimeLeftMs_ -= timeElapsedMs;

        return IoDevice_.readLine().data();
    }
    void WriteLine(const std::string& line) override {
        if (Timeouted_) {
            return;
        }

        IoDevice_.write((line + "\n").c_str());
    }

    void OnTurnEnd() override {
        TimeLeftMs_ += TimeoutAdditionPerTurnMs_;
    }

private:
    QIODevice& IoDevice_;
    const qint64 TimeoutMs_;
    const qint64 TimeoutAdditionPerTurnMs_;

    qint64 TimeLeftMs_;
    bool Timeouted_;
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        qDebug() << "Usage:" << argv[0] << "<config_file>";
        return 1;
    }

    TRunConfig runConfig;
    try {
        runConfig = LoadRunConfig(argv[1]);
    } catch (const std::exception& e) {
        qDebug() << "Failed to load run config:" << e.what();
        return 1;
    }

    // Note: it is important to check log dir before game process
    try {
        QDir logDir(QString::fromStdString(runConfig.LogDir_));
        if (!logDir.makeAbsolute()) {
            throw std::runtime_error("failed to make log dir '" + runConfig.LogDir_ + "' absolute");
        }
        if (!QDir().mkpath(logDir.path())) {
            throw std::runtime_error("failed to create log dir '" + runConfig.LogDir_ + "' absolute");
        }

        runConfig.LogDir_ = logDir.path().toStdString();
        qDebug() << "Current log dir:" << logDir.path();
    } catch (const std::exception& e) {
        qDebug() << "Failed to setup log dir:" << e.what();
        return 1;
    }

    // Setup tmp dir
    QDir tmpDir = QDir(QDir::currentPath()).filePath("tmp");
    try {
        if (!tmpDir.removeRecursively()) {
            throw std::runtime_error("failed to remove old tmp dir '" + tmpDir.path().toStdString() + "'");
        }
        if (!QDir().mkpath(tmpDir.path())) {
            throw std::runtime_error("failed to create tmp dir '" + tmpDir.path().toStdString()+ "'");
        }

        for (size_t i = 0; i < runConfig.Players_.size(); ++i) {
            auto& playerConfig = runConfig.Players_[i];
            if (playerConfig.Type_ == TRunConfig::TPlayer::EType::PROCESS) {
                QDir playerFile(QString::fromStdString(playerConfig.Info_));

                QString targetPath = tmpDir.filePath(playerFile.dirName()) + "_" + QString::number(i);
                if (!QFile::copy(playerFile.path(), targetPath)) {
                    throw std::runtime_error(
                        "failed to copy '"
                        + playerFile.path().toStdString()
                        + "' to '"
                        + targetPath.toStdString()
                        + "'"
                    );
                }

                playerConfig.Info_ = (playerFile.dirName() + "_" + QString::number(i)).toStdString();
            }
        }

        qDebug() << "Current tmp dir:" << tmpDir.path();
    } catch (const std::exception& e) {
        qDebug() << "Failed to setup tmp dir:" << e.what();
        return 1;
    }

    std::vector<std::unique_ptr<IPlayer>> playerEngines;
    std::vector<std::pair<std::unique_ptr<QProcess>, int>> playerProcesses;

    for (const auto& playerConfig : runConfig.Players_) {
        switch (playerConfig.Type_) {
            case TRunConfig::TPlayer::EType::DEFAULT: {
                playerEngines.push_back(CreateDefaultPlayer(playerConfig.Info_));
                break;
            }
            case TRunConfig::TPlayer::EType::PROCESS: {
                std::unique_ptr<QProcess> process = std::make_unique<QProcess>();
                process->setWorkingDirectory(tmpDir.path());
                process->setProgram(QString::fromStdString(playerConfig.Info_));
                process->start();

                if (!process->waitForStarted(runConfig.ProcessPlayerStartTimeoutMs_)) {
                    qDebug() << "Failed to start " << QString::fromStdString(playerConfig.Info_) << ":" << process->errorString();
                    return 1;
                }
                playerEngines.push_back(std::make_unique<TIODevicePlayer>(
                    runConfig.GameConfig_.MaxPlayerShipMovesPerStep_,
                    *process,
                    runConfig.ProcessPlayerTimeoutMs_,
                    runConfig.ProcessPLayerTimeoutAdditionPerTurnMs_
                ));
                playerProcesses.push_back(std::make_pair(std::move(process), static_cast<int>(playerEngines.size())));
                break;
            }
        }
    }

    TGame game(runConfig.GameConfig_);
    for (auto& playerEngine : playerEngines) {
        game.AddPlayer(std::move(playerEngine));
    }

    try {
        qDebug() << "Start game processing";
        game.Process();
        qDebug() << "Game finished";
    } catch (const std::exception& e) {
        qDebug() << "Failed to process game:" << e.what();
        return 1;
    }

    qDebug() << "Start finishing players processes";
    for (auto& [playerProcess, playerId] : playerProcesses) {
        playerProcess->kill();
        if (!playerProcess->waitForFinished(runConfig.ProcessPlayerFinishTimeoutMs_)) {
            qDebug() << "Process of player" << playerId << "did not finish";
        } else {
            qDebug() << "Process of player" << playerId << "finished";
        }
    }
    qDebug() << "Player processes are finished";

    try {
        QDir logDir(QString::fromStdString(runConfig.LogDir_));

        qDebug() << "Start recording log";

        // Create all log files first
        std::ofstream scoresLog(logDir.filePath("game_scores.log").toStdString());
        std::ofstream gameResultLog(logDir.filePath("game_result.log").toStdString());
        std::ofstream gameVisualizerLog(logDir.filePath("game_visualizer.log").toStdString());
        std::ofstream errorLog(logDir.filePath("error.log").toStdString());

        const auto& gameLogger = game.GetGameLogger();
        const auto& finalGameState = gameLogger.GetFinalGameState();
        const auto gameResult = GetGameResult(gameLogger);

        {
            // Scores log
            bool isFirst = true;
            for (const auto& playerScore : gameResult.PlayerScores_) {
                scoresLog << (isFirst ? "" : " ") << playerScore;
                isFirst = false;
            }
            scoresLog << '\n';
        }

        {
            // Result log
            PrintHumanReadableGameResult(gameResultLog, gameResult);
            gameResultLog << '\n';
            PrintGameState(gameResultLog, finalGameState, /* planetInfoOnly = */ true);
        }

        {
            // Visualizer log
            if (runConfig.DisableVisualizerLog_) {
                qDebug() << "Warning: visualizer log is disabled";
                gameVisualizerLog << "disabled\n";
            } else {
                PrintGameLogInVisualizerFormat(gameVisualizerLog, game.GetGameConfig().GameMap_, game.GetGameLogger());
            }
        }

        {
            // Error log
            for (const auto& error : gameLogger.GetErrors()) {
                errorLog << error << '\n';
            }
        }

        qDebug() << "Log is recorded";

    } catch (const std::exception& e) {
        qDebug() << "Failed to save log files:" << e.what();
        return 1;
    }

    return 0;
}
