//
// Created by Egor Chunaev on 14.06.2021.
//

#include "text_player.h"

#include <sstream>

/*
    INIT INFO START
    PlayerId PlayerCount PlanetCount
    <Matrix PlanetCount x PlanetCount>
    INIT INFO END
*/

/*
    PER MOVE INFO START

    <PlanetCount lines>
    Captured PlayerId ShipCount Level Armor

    <number of moves>
    PlayerId FromPlanetId ToPlanetId Count

    PER MOVE INFO END
*/

/*
    PLAYER MOVE BEGIN

    <number of moves>
    FromPlanetId ToPlanetId Count

    PLAYER MOVE END
*/

void TTextPlayer::SendGameInfo(const TGameInfo& gameInfo) {
    WriteGameInfo(gameInfo);
}

TPlayerMove TTextPlayer::GetMove(const TGameState& gameState, const TLastShipMoves& lastShipMoves) {
    WriteGameState(gameState, lastShipMoves);
    return ReadPlayerMove();
}

void TTextPlayer::WriteGameInfo(const TGameInfo& gameInfo) {
    {
        std::stringstream ss;
        ss << gameInfo.PlayerId_ << " " << gameInfo.PlayerCount_ << " " << gameInfo.GameMap_.Dists_.size();
        WriteLine(ss.str());
    }

    {
        for (size_t i = 0; i < gameInfo.GameMap_.Dists_.size(); ++i) {
            std::stringstream ss;
            ss << gameInfo.GameMap_.Dists_[i][0];
            for (size_t j = 1; j < gameInfo.GameMap_.Dists_.size(); ++j) {
                ss << " " << gameInfo.GameMap_.Dists_[i][j];
            }
            WriteLine(ss.str());
        }
    }
}

void TTextPlayer::WriteGameState(const TGameState& gameState, const TLastShipMoves& lastShipMoves) {
    for (const auto& planetInfo : gameState.PlanetInfos_) {
        std::stringstream ss;
        ss
            << planetInfo.PlayerId_ << " "
            << planetInfo.ShipCount_ << " "
            << planetInfo.Level_ << " "
            << planetInfo.Armor_;

        WriteLine(ss.str());
    }

    {
        std::stringstream ss;
        ss << lastShipMoves.ShipMoves_.size();
        WriteLine(ss.str());
    }

    for (const auto& shipMove : lastShipMoves.ShipMoves_) {
        std::stringstream ss;
        ss
            << shipMove.PlayerId_ << " "
            << shipMove.FromPlanetId_ << " "
            << shipMove.ToPlanetId_ << " "
            << shipMove.Count_;
    }
}

#include <iostream>
TPlayerMove TTextPlayer::ReadPlayerMove() {
    TPlayerMove playerMove;
    playerMove.DisqualifyMe_ = false;

    int moveCount = 0;
    {
        std::stringstream ss(ReadLine());
        if (!(ss >> moveCount)) {
            playerMove.DisqualifyMe_ = true;
            return playerMove;
        }
    }

    for (int i = 0; i < moveCount; ++i) {
        std::stringstream ss(ReadLine());

        TPlayerMove::TShipMove shipMove;
        if (!(ss >> shipMove.FromPlanetId_) || !(ss >> shipMove.ToPlanetId_) || !(ss >> shipMove.Count_)) {
            playerMove.DisqualifyMe_ = true;
            return playerMove;
        }

        playerMove.ShipMoves_.push_back(shipMove);
    }

    return playerMove;
}

