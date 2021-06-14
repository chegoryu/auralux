//
// Created by Egor Chunaev on 14.06.2021.
//

#include "default_players.h"

#include <iostream>

void TAFKPlayer::SendGameInfo(const TGameInfo& gameInfo) {
    (void)gameInfo;
}

TPlayerMove TAFKPlayer::GetMove(const TGameState& gameState, const TLastShipMoves& lastShipMoves) {
    (void)gameState;
    (void)lastShipMoves;

    TPlayerMove playerMove;
    playerMove.DisqualifyMe_ = false;
    return playerMove;
}

void TDisqualifyPlayer::SendGameInfo(const TGameInfo& gameInfo) {
    (void)gameInfo;
}

TPlayerMove TDisqualifyPlayer::GetMove(const TGameState& gameState, const TLastShipMoves& lastShipMoves) {
    (void) gameState;
    (void) lastShipMoves;

    TPlayerMove playerMove;
    if (--StepToDisqualify_ <= 0) {
        playerMove.DisqualifyMe_ = true;
        playerMove.DisqualifyReason_ = "I'm too lazy";
    } else {
        playerMove.DisqualifyMe_ = false;
    }
    return playerMove;
}

void TUpgradeAndRepairMainPlayer::SendGameInfo(const TGameInfo& gameInfo) {
    PlayerId_ = gameInfo.PlayerId_;
}

TPlayerMove TUpgradeAndRepairMainPlayer::GetMove(const TGameState& gameState, const TLastShipMoves& lastShipMoves) {
    (void)lastShipMoves;

    TPlayerMove playerMove;
    playerMove.DisqualifyMe_ = false;
    for (size_t i = 0; i < gameState.PlanetInfos_.size(); ++i) {
        const auto& planetInfo = gameState.PlanetInfos_[i];
        if (planetInfo.PlayerId_ == PlayerId_) {
            playerMove.ShipMoves_.push_back({
                .FromPlanetId_ = static_cast<int>(i + 1),
                .ToPlanetId_ = static_cast<int>(i + 1),
                .Count_ = planetInfo.ShipCount_,
            });
        }
    }

    return playerMove;
}

std::string TStdinStdoutPlayer::ReadLine() {
    std::string line;
    getline(std::cin, line);

    return line;
}

void TStdinStdoutPlayer::WriteLine(const std::string& line) {
    std::cout << line << std::endl;
}