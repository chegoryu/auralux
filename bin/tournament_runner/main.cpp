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
        QString RunnerInfo_;
        QString Name_;
    };

    struct TMap {
        QString Path_;
    };

    EType Type_;
    QVector<TPlayer> Players_;
    QVector<TMap> Maps_;
};

struct TGameRun {
    QVector<qint32> PlayerIds_;
    QString MapPath_;
};

struct TGameResult {
    QVector<double> PlayerScores_;
};

TTournamentConfig LoadTournamentConfig(QString path) {
    // TODO
    return {};
}

QVector<TGameRun> GenerateGameRuns(const TTournamentConfig& tournamentConfig) {
    // TODO
    return {};
}

TGameResult ProcessGameRun(const TGameRun& gameRuns) {
    // TODO
    return {};
}

QVector<TGameResult> ProcessGameRuns(const QVector<TGameRun>& gameRuns) {
    QVector<TGameResult> result;
    for (const auto& gameRun : gameRuns) {
        result.push_back(ProcessGameRun(gameRun));
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

    QVector<TGameRun> gameRuns = GenerateGameRuns(tournamentConfig);
    QVector<TGameResult> gameResults = ProcessGameRuns(gameRuns);

    SaveTournamentResult(gameResults);

    return 0;
}