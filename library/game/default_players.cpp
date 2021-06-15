//
// Created by Egor Chunaev on 14.06.2021.
//

#include "default_players.h"

#include <iostream>
#include <random>

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
        if (planetInfo.ShipCount_ == 0) {
            continue;
        }
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

TAggressiveExpansionPlayer::TAggressiveExpansionPlayer(EGameStyle gameStyle)
    : GameStyle_(gameStyle)
{}

void TAggressiveExpansionPlayer::SendGameInfo(const TGameInfo& gameInfo) {
    PlayerId_ = gameInfo.PlayerId_;
    PlanetOrder_.resize(gameInfo.GameMap_.Dists_.size());
    for (size_t i = 0; i < PlanetOrder_.size(); ++i) {
        PlanetOrder_[i] = i;
    }

    switch (GameStyle_) {
        case EGameStyle::RANDOM: {
            std::mt19937 rng;
            rng.seed(PlayerId_ + PlanetOrder_.size());
            std::shuffle(PlanetOrder_.begin(), PlanetOrder_.end(), rng);
        }
        case EGameStyle::NEAREST: {
            int startPlanet = gameInfo.GameMap_.StartPlanets_.at(PlayerId_ - 1) - 1;
            std::sort(PlanetOrder_.begin(), PlanetOrder_.end(), [&startPlanet, &gameInfo](int a, int b) {
                return gameInfo.GameMap_.Dists_.at(startPlanet).at(a) < gameInfo.GameMap_.Dists_.at(startPlanet).at(b);
            });
        }
    }
}

TPlayerMove TAggressiveExpansionPlayer::GetMove(const TGameState& gameState, const TLastShipMoves& lastShipMoves) {
    (void)lastShipMoves;

    int targetPlanet = ChooseTargetPlanet(gameState);

    TPlayerMove playerMove;
    playerMove.DisqualifyMe_ = false;
    for (size_t i = 0; i < gameState.PlanetInfos_.size(); ++i) {
        const auto& planetInfo = gameState.PlanetInfos_[i];
        if (planetInfo.ShipCount_ == 0) {
            continue;
        }
        if (planetInfo.PlayerId_ == PlayerId_) {
            if (planetInfo.Level_ < TAggressiveExpansionPlayer::MAX_PLANET_LEVEL
                || planetInfo.Armor_ < TAggressiveExpansionPlayer::MIN_ARMOR_TO_REPAIR[planetInfo.Level_]
            ) {
                playerMove.ShipMoves_.push_back({
                    .FromPlanetId_ = static_cast<int>(i + 1),
                    .ToPlanetId_ = static_cast<int>(i + 1),
                    .Count_ = planetInfo.ShipCount_,
                });
            } else if (planetInfo.ShipCount_ > TAggressiveExpansionPlayer::MIN_SHIPS_TO_MOVE) {
                playerMove.ShipMoves_.push_back({
                    .FromPlanetId_ = static_cast<int>(i + 1),
                    .ToPlanetId_ = targetPlanet + 1,
                    .Count_ = planetInfo.ShipCount_,
                });
            }
        }
    }

    return playerMove;
}

int TAggressiveExpansionPlayer::ChooseTargetPlanet(const TGameState& gameState) {
    for (size_t i = 0; i < gameState.PlanetInfos_.size(); ++i) {
        int currentId = PlanetOrder_[i];
        const auto& planetInfo = gameState.PlanetInfos_[currentId];
        if (planetInfo.PlayerId_ == PlayerId_ && planetInfo.Level_ == 0) {
            return currentId;
        }
    }

    for (size_t i = 0; i < gameState.PlanetInfos_.size(); ++i) {
        int currentId = PlanetOrder_[i];
        const auto& planetInfo = gameState.PlanetInfos_[currentId];
        if (planetInfo.PlayerId_ == 0) {
            return currentId;
        }
    }

    std::pair<int, int> mn = {-1, -1};
    for (size_t i = 0; i < gameState.PlanetInfos_.size(); ++i) {
        int currentId = PlanetOrder_[i];
        const auto& planetInfo = gameState.PlanetInfos_[currentId];
        if (planetInfo.PlayerId_ != PlayerId_ && (mn.first == -1 || planetInfo.ShipCount_ < mn.first)) {
            mn = {planetInfo.ShipCount_, currentId};
        }
    }

    if (mn.first != -1) {
        return mn.second;
    }

    return 0;
}

std::string TStdinStdoutPlayer::ReadLine() {
    std::string line;
    getline(std::cin, line);

    return line;
}

void TStdinStdoutPlayer::WriteLine(const std::string& line) {
    std::cout << line << std::endl;
}