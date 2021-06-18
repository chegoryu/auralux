//
// Created by Egor Chunaev on 16.06.2021.
//

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QHash>
#include <QProcess>

#include <cassert>
#include <memory>
#include <random>
#include <stdexcept>

struct TTournamentConfig {
    enum EType {
        ONE_VS_ONE_ALL = 0,
        FREE_FOR_ALL = 1,
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
        QString TmpDir_ = "tmp";
    };

    struct TResultSaverConfig {
        QString ResultFilePath_ = "tournament_result.csv";
        qint32 PlayerScoreMultiply_ = 100;
    };

    EType Type_;
    QVector<TPlayer> Players_;
    QVector<TMap> Maps_;

    QString TournamentGameLogsDir_ = "logs";
    // For FREE_FOR_ALL
    qint32 TournamentSteps_ = 1000;

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
    QVector<bool> IsPlayerDisqualified_;
    QVector<bool> IsPlayerDead_;
    qint32 WinnerId_;
    bool FromCache_;
};

struct TPlayerResult {
    qint32 PlayerId_;
    double TotalScore_;
    qint32 GamesPlayed_;
    qint32 AbsoluteWinnerCount_;
    qint32 AliveInDrawCount_;
    qint32 DeadCount_;
    qint32 DisqualifiedCount_;
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
    bool hasTournamentStepsOption = false;
    bool hasTournamentResultFilePathOption = false;
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
            } else if (tournamentType == "FREE_FOR_ALL") {
                tournamentConfig.Type_ = TTournamentConfig::EType::FREE_FOR_ALL;
                qDebug() << "Tournament type is FREE_FOR_ALL";
            } else {
                throw std::runtime_error("wrong TOURNAMENT_TYPE option: '" + line.toStdString() + "'");
            }

            hasTournamentTypeOption = true;
        } else if (optionName == "TOURNAMENT_STEPS") {
            if (hasTournamentStepsOption) {
                throw std::runtime_error("two or more TOURNAMENT_STEPS options in config");
            }
            if (lineParts.size() != 2) {
                throw std::runtime_error("wrong TOURNAMENT_STEPS option: '" + line.toStdString() + "'");
            }

            bool ok;
            tournamentConfig.TournamentSteps_ = lineParts.at(1).trimmed().toInt(&ok);
            if (!ok) {
                throw std::runtime_error("wrong TOURNAMENT_STEPS option: '" + line.toStdString() + "'");
            }
            hasTournamentStepsOption = true;
        } else if (optionName == "TOURNAMENT_RESULT_FILE_PATH") {
            if (hasTournamentResultFilePathOption) {
                throw std::runtime_error("two or more TOURNAMENT_RESULT_FILE_PATH options in config");
            }
            if (lineParts.size() != 2) {
                throw std::runtime_error("wrong TOURNAMENT_RESULT_FILE_PATH option: '" + line.toStdString() + "'");
            }

            tournamentConfig.ResultSaverConfig_.ResultFilePath_ = lineParts.at(1).trimmed();
            hasTournamentResultFilePathOption = true;
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

    if (tournamentConfig.Type_ == TTournamentConfig::EType::ONE_VS_ONE_ALL) {
        if (hasTournamentStepsOption) {
            throw std::runtime_error("TOURNAMENT_STEPS is useless with ONE_VS_ONE_ALL tournament type");
        }
    }

    if (tournamentConfig.Type_ == TTournamentConfig::EType::FREE_FOR_ALL) {
        if (tournamentConfig.Players_.size() % 4 != 0) {
            throw std::runtime_error("number of players must be divisible by 4 for FREE_FOR_ALL tournament type");
        }
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

QVector<TGameRun> GenerateGameRunsForFreeForAll(const TTournamentConfig& tournamentConfig) {
    assert(tournamentConfig.Type_ == TTournamentConfig::EType::FREE_FOR_ALL);
    assert(tournamentConfig.Players_.size() % 4 == 0);

    qDebug() << "Free for all steps:" << tournamentConfig.TournamentSteps_;

    quint32 seed = 0;
    for (const auto& playerConfig : tournamentConfig.Players_) {
        seed ^= qHash(playerConfig.Name_);
    }
    std::mt19937 rng(seed);
    qDebug() << "Rnd seed" << seed;

    QVector<qint32> order(tournamentConfig.Players_.size());
    for (int i = 0; i < tournamentConfig.Players_.size(); ++i) {
        order[i] = i;
    }

    QVector<TGameRun> gameRuns;
    for (int i = 0; i < tournamentConfig.TournamentSteps_; ++i) {
        std::shuffle(order.begin(), order.end(), rng);

        for (int j = 0; j < tournamentConfig.Players_.size(); j += 4) {
            gameRuns.push_back({
               .PlayerIds_ = QVector<qint32>({order[j], order[j + 1], order[j + 2], order[j + 3]}),
               .MapId_ = static_cast<qint32>(rng() % tournamentConfig.Maps_.size()),
           });
        }
    }

    return gameRuns;
}

QVector<TGameRun> GenerateGameRuns(const TTournamentConfig& tournamentConfig) {
    switch (tournamentConfig.Type_) {
        case TTournamentConfig::EType::ONE_VS_ONE_ALL: {
            return GenerateGameRunsForOneVsOneAll(tournamentConfig);
        }
        case TTournamentConfig::EType::FREE_FOR_ALL: {
            return GenerateGameRunsForFreeForAll(tournamentConfig);
        }
    }

    assert(false);
    return {};
}

TGameResult GetGameResult(QString playerScoresFilePath, QVector<qint32> playerIds) {
    TGameResult gameResult;
    gameResult.PlayerIds_ = playerIds;
    gameResult.FromCache_ = false;

    QFile playerScoresFile(playerScoresFilePath);
    if (!playerScoresFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("failed to open game scores file");
    }

    QTextStream gameScoresStream(&playerScoresFile);

    {
        // Winner
        if ((gameScoresStream >> gameResult.WinnerId_).status() != QTextStream::Ok) {
            throw std::runtime_error("failed to read winner id");
        }

        if (gameResult.WinnerId_ != -1) {
            --gameResult.WinnerId_;
        }
    }

    {
        // Scores
        gameResult.PlayerScores_.resize(playerIds.size());
        for (int i = 0; i < playerIds.size(); ++i) {
            if ((gameScoresStream >> gameResult.PlayerScores_[i]).status() != QTextStream::Ok) {
                throw std::runtime_error("failed to read player score");
            }
        }
    }

    {
        // Disqualified players
        gameResult.IsPlayerDisqualified_.resize(playerIds.size());
        std::fill(gameResult.IsPlayerDisqualified_.begin(), gameResult.IsPlayerDisqualified_.end(), false);
        qint32 count;
        if ((gameScoresStream >> count).status() != QTextStream::Ok) {
            throw std::runtime_error("failed to read disqualified player count");
        }
        for (int i = 0; i < count; ++i) {
            int playerId;
            if ((gameScoresStream >> playerId).status() != QTextStream::Ok) {
                throw std::runtime_error("failed to read disqualified player id");
            }
            gameResult.IsPlayerDisqualified_[playerId - 1] = true;
        }
    }

    {
        // Dead players
        gameResult.IsPlayerDead_.resize(playerIds.size());
        std::fill(gameResult.IsPlayerDead_.begin(), gameResult.IsPlayerDead_.end(), false);
        qint32 count;
        if ((gameScoresStream >> count).status() != QTextStream::Ok) {
            throw std::runtime_error("failed to read dead player count");
        }
        for (int i = 0; i < count; ++i) {
            int playerId;
            if ((gameScoresStream >> playerId).status() != QTextStream::Ok) {
                throw std::runtime_error("failed to read deadplayer id");
            }
            gameResult.IsPlayerDead_[playerId - 1] = true;
        }
    }

    return gameResult;
}

TGameResult ProcessGameRun(const TTournamentConfig& tournamentConfig, const TGameRun& gameRun, int gameRunId) {
    QString runDirName = QString("game_id_%2_map_%3")
        .arg(gameRunId, 5, 10, QChar('0'))
        .arg(gameRun.MapId_);

    for (int i = 0; i < gameRun.PlayerIds_.size(); ++i) {
        runDirName += QString("_player_%1").arg(gameRun.PlayerIds_[i]);
    }

    QString logDir = QString("%1/%2")
        .arg(tournamentConfig.TournamentGameLogsDir_)
        .arg(runDirName);

    QString gameRunnerTmpDir = QString("%1/%2")
        .arg(tournamentConfig.GameRunnerConfig_.TmpDir_)
        .arg(runDirName);

    QString playerScoresFilePath = QDir(logDir).filePath(tournamentConfig.GameRunnerConfig_.PlayerScoresFileName_);

    try {
        // Try read last run result
        TGameResult gameResult = GetGameResult(playerScoresFilePath, gameRun.PlayerIds_);
        gameResult.FromCache_ = true;
        return gameResult;
    } catch (...) {
        // Well, we need to run game
    }

    QString gameRunnerCfg = QString(
        "MAP %1\n"
        "LOG_DIR %2\n"
        "TMP_DIR %3\n"
    )
        .arg(tournamentConfig.Maps_[gameRun.MapId_].Path_)
        .arg(logDir)
        .arg(gameRunnerTmpDir);

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

    TGameResult gameResult = GetGameResult(playerScoresFilePath, gameRun.PlayerIds_);
    gameResult.FromCache_ = false;
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
            .AliveInDrawCount_ = 0,
            .DeadCount_ = 0,
            .DisqualifiedCount_ = 0,
        };
    }

    for (const auto& gameResult : gameResults) {
        for (int i = 0; i < gameResult.PlayerIds_.size(); ++i) {
            auto& playerResult = playerResults[gameResult.PlayerIds_[i]];

            playerResult.TotalScore_ += gameResult.PlayerScores_[i];
            ++playerResult.GamesPlayed_;

            if (gameResult.WinnerId_ == i) {
                ++playerResult.AbsoluteWinnerCount_;
            } else {
                if (gameResult.IsPlayerDisqualified_[i]) {
                    ++playerResult.DisqualifiedCount_;
                } else if (gameResult.IsPlayerDead_[i]) {
                    ++playerResult.DeadCount_;
                } else {
                    assert(gameResult.WinnerId_ == -1);
                    ++playerResult.AliveInDrawCount_;
                }
            }
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

    QFileInfo resultFilePath(tournamentConfig.ResultSaverConfig_.ResultFilePath_);
    if (!resultFilePath.makeAbsolute()) {
        throw std::runtime_error(
            "failed to make result file path '"
            + tournamentConfig.ResultSaverConfig_.ResultFilePath_.toStdString()
            + "' absolute"
        );
    }

    QFile resultFile(resultFilePath.filePath());
    if (!resultFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw std::runtime_error("failed to open result file " + resultFilePath.filePath().toStdString());
    }

    QTextStream resultStream(&resultFile);
    resultStream << "Name\tScore\tAbsoluteWinner\tAliveInDraw\tDead\tDisqualified\n";
    for (const auto& playerResult : playerResults) {
        assert(playerResult.PlayerId_ != -1);
        resultStream
            << tournamentConfig.Players_[playerResult.PlayerId_].Name_ << "\t"
            << tournamentConfig.ResultSaverConfig_.PlayerScoreMultiply_ * (playerResult.TotalScore_ / playerResult.GamesPlayed_) << "\t"
            << playerResult.AbsoluteWinnerCount_ << '\t'
            << playerResult.AliveInDrawCount_ << "\t"
            << playerResult.DeadCount_ << '\t'
            << playerResult.DisqualifiedCount_ << '\n';

    }

    qDebug() << "Tournament result file:" << resultFile.fileName();
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