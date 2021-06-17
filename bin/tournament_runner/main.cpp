//
// Created by Egor Chunaev on 16.06.2021.
//

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QProcess>

#include <memory>

struct TTournamentConfig {
    enum EType {
        ONE_VS_ONE_ALL = 0,
    };

    struct TPlayer {
        QString Name_;
        QString GameRunnerInfo_;
    };

    struct TMap {
        QString Path_;
    };

    struct TGameRunnerConfig {
        QString Path_ = "game_runner";
        qint64 StartTimeoutMs_ = 10000;
        qint64 GameTimeoutMs_ = 60000;
        qint64 KillTimeoutMs_ = 1000;
        QString PlayerScoresFileName_ = "game_scores.log";
    };

    struct TResultSaverConfig {
        QString ResultPath_ = "tournament_result.csv";
        qint32 PlayerScoreMultiply_ = 100;
    };

    EType Type_;
    QVector<TPlayer> Players_;
    QVector<TMap> Maps_;

    QString TournamentGameLogsDir_ = "logs";
    TGameRunnerConfig GameRunnerConfig_;
    TResultSaverConfig ResultSaverConfig_;
};

struct TGameRun {
    QVector<qint32> PlayerIds_;
    qint32 MapId_;
};

struct TGameResult {
    QVector<qint32> PlayerIds_;
    QVector<double> PlayerScores_;
    qint32 WinnerId_;
    bool FromCache_;
};

struct TPlayerResult {
    qint32 PlayerId_;
    double TotalScore_;
    qint32 GamesPlayed_;
    qint32 AbsoluteWinnerCount_;
};

TTournamentConfig LoadTournamentConfig(QString path) {
    TTournamentConfig tournamentConfig;

    QFile configFile(path);
    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("failed to open config");
    }

    QTextStream configStream(&configFile);
    configStream.setCodec("UTF-8");

    bool hasGameLogsDirOption = false;
    bool hasTournamentTypeOption = false;
    while (!configStream.atEnd()) {
        QString line = configStream.readLine();
        QStringList lineParts = line.split(",", Qt::SkipEmptyParts);

        if (lineParts.isEmpty()) {
            continue;
        }

        QString optionName = lineParts.at(0).trimmed();
        if (optionName == "MAP") {
            if (lineParts.size() != 2) {
                throw std::runtime_error("wrong MAP option: '" + line.toStdString() + "'");
            }

            tournamentConfig.Maps_.push_back({
                .Path_ = lineParts.at(1).trimmed(),
            });

            qDebug() << "Add map" << tournamentConfig.Maps_.back().Path_;
        } else if (optionName == "PLAYER") {
            if (lineParts.size() != 3) {
                throw std::runtime_error("wrong PLAYER option: '" + line.toStdString() + "'");
            }

            tournamentConfig.Players_.push_back({
                .Name_ = lineParts.at(1).trimmed(),
                .GameRunnerInfo_ = lineParts.at(2).trimmed(),
            });

            qDebug() << "Add player" << tournamentConfig.Players_.back().Name_ << tournamentConfig.Players_.back().GameRunnerInfo_;
        } else if (optionName == "TOURNAMENT_TYPE") {
            if (hasTournamentTypeOption) {
                throw std::runtime_error("two or more TOURNAMENT_TYPE options in config");
            }
            if (lineParts.size() != 2) {
                throw std::runtime_error("wrong TOURNAMENT_TYPE option: '" + line.toStdString() + "'");
            }

            QString tournamentType = lineParts.at(1).trimmed();

            if (tournamentType == "ONE_VS_ONE_ALL") {
                tournamentConfig.Type_ = TTournamentConfig::EType::ONE_VS_ONE_ALL;
                qDebug() << "Tournament type is ONE_VS_ONE_ALL";
            } else {
                throw std::runtime_error("wrong TOURNAMENT_TYPE option: '" + line.toStdString() + "'");
            }

            hasTournamentTypeOption = true;
        } else if (optionName == "GAME_LOGS_DIR") {
            if (hasGameLogsDirOption) {
                throw std::runtime_error("two or more GAME_LOGS_DIR options in config");
            }
            if (lineParts.size() != 2) {
                throw std::runtime_error("wrong GAME_LOGS_DIR option: '" + line.toStdString() + "'");
            }

            tournamentConfig.TournamentGameLogsDir_ = lineParts.at(1).trimmed();

            hasGameLogsDirOption = true;
        } else {
            throw std::runtime_error("unknown option in config: '" + line.toStdString() + "'");
        }
    }

    if (!hasTournamentTypeOption) {
        throw std::runtime_error("TOURNAMENT_TYPE option not defined, but it is required option");
    }

    if (tournamentConfig.Players_.isEmpty()) {
        throw std::runtime_error("empty list of players");
    }

    if (tournamentConfig.Maps_.isEmpty()) {
        throw std::runtime_error("empty list of maps");
    }

    return tournamentConfig;
}

QVector<TGameRun> GenerateGameRunsForOneVsOneAll(const TTournamentConfig& tournamentConfig) {
    assert(tournamentConfig.Type_ == TTournamentConfig::EType::ONE_VS_ONE_ALL);

    QVector<TGameRun> gameRuns;
    for (int i = 0; i < tournamentConfig.Maps_.size(); ++i) {
        for (int j = 0; j < tournamentConfig.Players_.size(); ++j) {
            for (int k = j + 1; k < tournamentConfig.Players_.size(); ++k) {
                gameRuns.push_back({
                    .PlayerIds_ = QVector<qint32>({static_cast<qint32>(j), static_cast<qint32>(k)}),
                    .MapId_ = static_cast<qint32>(i),
                });
                gameRuns.push_back({
                   .PlayerIds_ = QVector<qint32>({static_cast<qint32>(k), static_cast<qint32>(j)}),
                   .MapId_ = static_cast<qint32>(i),
               });
            }
        }
    }

    return gameRuns;
}

QVector<TGameRun> GenerateGameRuns(const TTournamentConfig& tournamentConfig) {
    switch (tournamentConfig.Type_) {
        case TTournamentConfig::EType::ONE_VS_ONE_ALL: {
            return GenerateGameRunsForOneVsOneAll(tournamentConfig);
        }
    }

    assert(false);
    return {};
}

std::pair<qint32, QVector<double>> GetPlayerScores(QString playerScoresFilePath, int playerCount) {
    QFile playerScoresFile(playerScoresFilePath);
    if (!playerScoresFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("failed to open game scores file");
    }

    QTextStream gameScoresStream(&playerScoresFile);

    qint32 winnerId;
    QVector<double> playerScores(playerCount);
    if ((gameScoresStream >> winnerId).status() != QTextStream::Ok) {
        throw std::runtime_error("failed to read winner id");
    }

    if (winnerId != -1) {
        --winnerId;
    }

    for (int i = 0; i < playerCount; ++i) {
        if ((gameScoresStream >> playerScores[i]).status() != QTextStream::Ok) {
            throw std::runtime_error("failed to read player score");
        }
    }
    return {winnerId, playerScores};
}

TGameResult ProcessGameRun(const TTournamentConfig& tournamentConfig, const TGameRun& gameRun, int gameRunId) {
    TGameResult gameResult;
    gameResult.PlayerIds_ = gameRun.PlayerIds_;
    gameResult.FromCache_ = false;

    QString logDir = QString("%1/game_id_%2_map_%3")
        .arg(tournamentConfig.TournamentGameLogsDir_)
        .arg(gameRunId)
        .arg(gameRun.MapId_);

    for (int i = 0; i < gameRun.PlayerIds_.size(); ++i) {
        logDir += QString("_player_%1").arg(gameRun.PlayerIds_[i]);
    }

    QString playerScoresFilePath = QDir(logDir).filePath(tournamentConfig.GameRunnerConfig_.PlayerScoresFileName_);

    auto getPlayerScores = [&playerScoresFilePath, &gameRun, &gameResult]() {
        auto [winnerId, playerScores] = GetPlayerScores(playerScoresFilePath, gameRun.PlayerIds_.size());
        gameResult.WinnerId_ = winnerId;
        gameResult.PlayerScores_ = playerScores;
    };

    try {
        // Try read last run result
        getPlayerScores();
        gameResult.FromCache_ = true;
        return gameResult;
    } catch (...) {
        // Well, we need to run game
    }

    QString gameRunnerCfg = QString(
        "MAP %1\n"
        "LOG_DIR %2\n"
    )
        .arg(tournamentConfig.Maps_[gameRun.MapId_].Path_)
        .arg(logDir);

    for (const int playerId : gameRun.PlayerIds_) {
        gameRunnerCfg += QString("PLAYER %1\n").arg(tournamentConfig.Players_[playerId].GameRunnerInfo_);
    }

    std::unique_ptr<QProcess> gameRunnerProcess = std::make_unique<QProcess>();

    gameRunnerProcess->setWorkingDirectory(QDir::currentPath());
    gameRunnerProcess->setProgram(tournamentConfig.GameRunnerConfig_.Path_);
    gameRunnerProcess->setArguments({"-"});
    gameRunnerProcess->start();

    if (!gameRunnerProcess->waitForStarted(tournamentConfig.GameRunnerConfig_.StartTimeoutMs_)) {
        qDebug() << "Failed to start game runner" << gameRunnerProcess->errorString();
        throw std::runtime_error("Failed to start game runner");
    }

    gameRunnerProcess->write(gameRunnerCfg.toUtf8());
    gameRunnerProcess->closeWriteChannel();

    if (!gameRunnerProcess->waitForFinished(tournamentConfig.GameRunnerConfig_.GameTimeoutMs_)) {
        qDebug() << "Failed to game runner wait finished";
        gameRunnerProcess->kill();
        if (!gameRunnerProcess->waitForFinished(tournamentConfig.GameRunnerConfig_.KillTimeoutMs_)) {
            qDebug() << "Failed to kill game runner" << gameRunnerProcess->errorString();
        }

        throw std::runtime_error("Failed to finish game runner");
    }

    if (gameRunnerProcess->exitCode() != 0) {
        qDebug() << "Game runner exit code" << gameRunnerProcess->exitCode();
        qDebug() << "Game runner stdout" << gameRunnerProcess->readAllStandardOutput();
        qDebug() << "Game runner stderr" << gameRunnerProcess->readAllStandardError();
        throw std::runtime_error("Bad game runner exit code");
    }

    getPlayerScores();

    return gameResult;
}

QVector<TGameResult> ProcessGameRuns(const TTournamentConfig& tournamentConfig, const QVector<TGameRun>& gameRuns) {
    QVector<TGameResult> result;

    int fromCache = 0;
    for (int i = 0; i < gameRuns.size(); ++i) {
        if (i && i % 20 == 0) {
            qDebug() << QString("Processed %1/%2, from cache %3, real game runs %4")
                .arg(i)
                .arg(gameRuns.size())
                .arg(fromCache)
                .arg(i - fromCache);
        }

        const auto& gameRun = gameRuns[i];
        try {
            result.push_back(ProcessGameRun(tournamentConfig, gameRun, i));
            fromCache += result.back().FromCache_;
        } catch (const std::exception& e) {
            qDebug() << "Failed to process" << i << "game run:" << e.what();
            throw;
        }
    }
    qDebug() << QString("Processed %1/%2, from cache %3, real game runs %4")
        .arg(gameRuns.size())
        .arg(gameRuns.size())
        .arg(fromCache)
        .arg(gameRuns.size() - fromCache);

    return result;
}

void SaveTournamentResult(const TTournamentConfig& tournamentConfig, const QVector<TGameResult>& gameResults) {
    QVector<TPlayerResult> playerResults(tournamentConfig.Players_.size());
    for (int i = 0; i < playerResults.size(); ++i) {
        playerResults[i] = {
            .PlayerId_ = i,
            .TotalScore_ = 0.0,
            .GamesPlayed_ = 0,
            .AbsoluteWinnerCount_ = 0,
        };
    }

    for (const auto& gameResult : gameResults) {
        assert(gameResult.PlayerIds_.size() == gameResult.PlayerScores_.size());
        if (gameResult.WinnerId_ != -1) {
            ++playerResults[gameResult.PlayerIds_[gameResult.WinnerId_]].AbsoluteWinnerCount_;
        }
        for (int i = 0; i < gameResult.PlayerIds_.size(); ++i) {
            auto& playerResult = playerResults[gameResult.PlayerIds_[i]];
            playerResult.TotalScore_ += gameResult.PlayerScores_[i];
            ++playerResult.GamesPlayed_;
        }
    }

    // Sanity check
    for (int i = 0; i < playerResults.size() - 1; ++i) {
        if (playerResults[i].GamesPlayed_ != playerResults[i + 1].GamesPlayed_) {
            throw std::runtime_error(
                "bad number of played games: "
                + std::to_string(playerResults[i].GamesPlayed_)
                + " for player " + std::to_string(i + 1)
                + ", " + std::to_string(playerResults[i + 1].GamesPlayed_)
                + " for player " + std::to_string(i + 2)
            );
        }
    }

    qDebug() << "Games per player:" << playerResults[0].GamesPlayed_;

    std::sort(playerResults.begin(), playerResults.end(), [](const auto& a, const auto& b){
       return a.TotalScore_ > b.TotalScore_;
    });

    QFile resultFile(tournamentConfig.ResultSaverConfig_.ResultPath_);
    if (!resultFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw std::runtime_error("failed to open result file");
    }

    QTextStream resultStream(&resultFile);
    resultStream << "Name\tScore\tAbsoluteWinner\n";
    for (const auto& playerResult : playerResults) {
        assert(playerResult.PlayerId_ != -1);
        resultStream
            << tournamentConfig.Players_[playerResult.PlayerId_].Name_ << "\t"
            << tournamentConfig.ResultSaverConfig_.PlayerScoreMultiply_ * (playerResult.TotalScore_ / playerResult.GamesPlayed_) << "\t"
            << playerResult.AbsoluteWinnerCount_ << '\n';
    }
}

int main(int argc, char *argv[]) {
    QDateTime startTime = QDateTime::currentDateTime();

    if (argc < 2) {
        qDebug() << "Usage:" << argv[0] << "<config_file>";
        return 1;
    }

    TTournamentConfig tournamentConfig;
    try {
        tournamentConfig = LoadTournamentConfig(argv[1]);
    } catch (const std::exception& e) {
        qDebug() << "Failed to load tournament config:" << e.what();
        return 1;
    }

    try {
        QDir gameLogsDir(tournamentConfig.TournamentGameLogsDir_);
        if (!gameLogsDir.makeAbsolute()) {
            throw std::runtime_error(
                "failed to make game logs dir path '"
                + tournamentConfig.TournamentGameLogsDir_.toStdString()
                + "' absolute"
            );
        }
        if (!QDir().mkpath(gameLogsDir.path())) {
            throw std::runtime_error("failed to create log dir '" + gameLogsDir.path().toStdString() + "'");
        }

        tournamentConfig.TournamentGameLogsDir_ = gameLogsDir.path();
        qDebug() << "Current game logs dir:" << gameLogsDir.path();
    } catch (const std::exception& e) {
        qDebug() << "Failed to setup game logs dir:" << e.what();
        return 1;
    }

    qDebug() << "Total players:" << tournamentConfig.Players_.size();
    qDebug() << "Total maps:" << tournamentConfig.Maps_.size();

    QVector<TGameRun> gameRuns = GenerateGameRuns(tournamentConfig);
    qDebug() << "Total game runs:" << gameRuns.size();

    QVector<TGameResult> gameResults;
    try {
        qDebug() << "Start processing game runs";
        gameResults = ProcessGameRuns(tournamentConfig, gameRuns);
        qDebug() << "Game runs processing finished";
    } catch (const std::exception& e) {
        qDebug() << "Failed process game runs:" << e.what();
        return 1;
    }

    try {
        qDebug() << "Start recording result";
        SaveTournamentResult(tournamentConfig, gameResults);
        qDebug() << "Tournament result is recorded";
    } catch (const std::exception& e) {
        qDebug() << "Failed to save result file:" << e.what();
        return 1;
    }

    QDateTime endTime = QDateTime::currentDateTime();
    qDebug() << "Total time:" << startTime.msecsTo(endTime) << "ms";

    return 0;
}