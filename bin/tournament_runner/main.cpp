//
// Created by Egor Chunaev on 16.06.2021.
//

#include <QDebug>
#include <QFile>
#include <QProcess>

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

    EType Type_;
    QVector<TPlayer> Players_;
    QVector<TMap> Maps_;
    qint32 PlayerScoreMultiply_ = 100;
    QString GameRunnerPath_ = "game_runner";
    QString TournamentResultPath_ = "tournament_result.csv";
};

struct TGameRun {
    QVector<qint32> PlayerIds_;
    qint32 MapId_;
};

struct TGameResult {
    QVector<qint32> PlayerIds_;
    QVector<double> PlayerScores_;
    qint32 WinnerId_;
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

TGameResult ProcessGameRun(const TTournamentConfig& tournamentConfig, const TGameRun& gameRun) {
    TGameResult gameResult;
    gameResult.PlayerIds_ = gameRun.PlayerIds_;

    // TODO
    gameResult.PlayerScores_ = {0.75, 0.25};
    gameResult.WinnerId_ = gameRun.PlayerIds_[0];

    return gameResult;
}

QVector<TGameResult> ProcessGameRuns(const TTournamentConfig& tournamentConfig, const QVector<TGameRun>& gameRuns) {
    QVector<TGameResult> result;
    for (const auto& gameRun : gameRuns) {
        result.push_back(ProcessGameRun(tournamentConfig, gameRun));
    }

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
            ++playerResults[gameResult.WinnerId_].AbsoluteWinnerCount_;
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
                "Bad number of played games: "
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

    QFile resultFile(tournamentConfig.TournamentResultPath_);
    if (!resultFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw std::runtime_error("failed to open result file");
    }

    QTextStream resultStream(&resultFile);
    resultStream << "Name\tScore\tAbsoluteWinner\n";
    for (const auto& playerResult : playerResults) {
        assert(playerResult.PlayerId_ != -1);
        resultStream
            << tournamentConfig.Players_[playerResult.PlayerId_].Name_ << "\t"
            << tournamentConfig.PlayerScoreMultiply_ * (playerResult.TotalScore_ / playerResult.GamesPlayed_) << "\t"
            << playerResult.AbsoluteWinnerCount_ << '\n';
    }
}

int main(int argc, char *argv[]) {
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

    qDebug() << "Total players:" << tournamentConfig.Players_.size();
    qDebug() << "Total maps:" << tournamentConfig.Maps_.size();

    QVector<TGameRun> gameRuns = GenerateGameRuns(tournamentConfig);
    qDebug() << "Total game runs:" << gameRuns.size();

    QVector<TGameResult> gameResults = ProcessGameRuns(tournamentConfig, gameRuns);

    qDebug() << "Start recording result";
    SaveTournamentResult(tournamentConfig, gameResults);
    qDebug() << "Tournament result is recorded";

    return 0;
}