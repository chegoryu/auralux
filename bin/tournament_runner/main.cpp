//
// Created by Egor Chunaev on 16.06.2021.
//

#include <QDebug>
#include <QDir>
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
    QString GameRunnerPath_ = "game_runner";
};

struct TGameRun {
    QVector<qint32> PlayerIds_;
    qint32 MapId_;
};

struct TGameResult {
    QVector<double> PlayerScores_;
};

TTournamentConfig LoadTournamentConfig(QString path) {
    TTournamentConfig tournamentConfig;

    QFile configFile(path);
    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("failed to open config");
    }

    QTextStream configStream(&configFile);
    configStream.setCodec("UTF-8");

    while (!configStream.atEnd()) {
        QString line = configStream.readLine();
        QStringList lineParts = line.split(",");

        if (lineParts.isEmpty()) {
            continue;
        }

        if (lineParts.at(0) == "MAP") {
            if (lineParts.size() != 2u) {
                throw std::runtime_error("wrong MAP option: '" + line.toStdString() + "'");
            }

            tournamentConfig.Maps_.push_back({
                .Path_ = lineParts.at(2),
            });
        } else if (lineParts.at(0) == "PLAYER") {
            if (lineParts.size() != 3u) {
                throw std::runtime_error("wrong PLAYER option: '" + line.toStdString() + "'");
            }

            tournamentConfig.Players_.push_back({
                .Name_ = lineParts.at(1),
                .GameRunnerInfo_ = lineParts.at(2)
            });
        } else {
            throw std::runtime_error("unknown option in config: '" + line.toStdString() + "'");
        }
    }

    return tournamentConfig;
}

QVector<TGameRun> GenerateGameRunsForOneVsOneAll(const TTournamentConfig& tournamentConfig) {
    assert(tournamentConfig.Type_ == TTournamentConfig::EType::ONE_VS_ONE_ALL);

    QVector<TGameRun> gameRuns;
    for (size_t i = 0; i < tournamentConfig.Maps_.size(); ++i) {
        for (size_t j = 0; j < tournamentConfig.Players_.size(); ++j) {
            for (size_t k = j + 1; k < tournamentConfig.Players_.size(); ++k) {
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

TGameResult ProcessGameRun(const TTournamentConfig& tournamentConfig, const TGameRun& gameRuns) {
    // TODO
    return {};
}

QVector<TGameResult> ProcessGameRuns(const TTournamentConfig& tournamentConfig, const QVector<TGameRun>& gameRuns) {
    QVector<TGameResult> result;
    for (const auto& gameRun : gameRuns) {
        result.push_back(ProcessGameRun(tournamentConfig, gameRun));
    }

    return result;
}

void SaveTournamentResult(const QVector<TGameResult>& gameResults) {
    // TODO
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

    SaveTournamentResult(gameResults);

    return 0;
}