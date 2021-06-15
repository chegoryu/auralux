//
// Created by Egor Chunaev on 15.06.2021.
//

#ifndef AURALUX_GAME_RESULT_H
#define AURALUX_GAME_RESULT_H

#include "game_logger.h"

#include <ostream>

struct TGameResult {
    enum EResult {
        ONE_WINNER = 0,
        NO_WINNER = 1,
        DRAW = 2,
    };

    EResult Result_ = EResult::DRAW;
    int WinnerId_ = -1;

    int PlayerCount_ = 0;
    int PlanetCount_ = 0;

    int FullSteps_ = 0;
    int TurnsInLastStep_ = 0;

    int ShipCount_ = 0;
    int NonDisqualifiedPlayersShipCount_ = 0;
    std::vector<int> PlayerPlanetCounts_;
    std::vector<int> PlayerShipCounts_;
    std::vector<double> PlayerScores_;

    std::vector<int> AlivePlayers_;
    std::vector<int> DisqualifiedPlayers_;
    std::vector<int> DeadPlayers_;
};

TGameResult GetGameResult(const TGameLogger& gameLogger);

void PrintHumanReadableGameResult(std::ostream& stream, const TGameResult& gameResult);

#endif // AURALUX_GAME_RESULT_H
