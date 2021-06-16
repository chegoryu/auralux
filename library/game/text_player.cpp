//
// Created by Egor Chunaev on 14.06.2021.
//

#include "text_player.h"

#include <sstream>

TTextPlayer::TTextPlayer(int maxPlayerMovesPerReadMove)
    : MaxPlayerMovesPerReadMove_(maxPlayerMovesPerReadMove)
{}

void TTextPlayer::SendGameInfo(const TGameInfo& gameInfo) {
    WriteGameInfo(gameInfo);
}

TPlayerMove TTextPlayer::GetMove(const TGameState& gameState, const TLastShipMoves& lastShipMoves) {
    WriteGameState(gameState, lastShipMoves);
    TPlayerMove playerMove = ReadPlayerMove();
    OnTurnEnd();

    return playerMove;
}

void TTextPlayer::SendGameOver() {
    WriteGameOver();
}

TPlayerMove TTextPlayer::DisqualifyMe(const std::string& reason) const {
    TPlayerMove playerMove;
    playerMove.DisqualifyMe_ = true;
    playerMove.DisqualifyReason_ = reason;
    return playerMove;
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

        WriteLine(ss.str());
    }

    for (const auto& planetInfo : gameState.PlanetInfos_) {
        std::stringstream ss;
        ss
            << planetInfo.PlayerId_ << " "
            << planetInfo.ShipCount_ << " "
            << planetInfo.Level_ << " "
            << planetInfo.Armor_;

        WriteLine(ss.str());
    }
}

void TTextPlayer::WriteGameOver() {
    WriteLine("-1");
}

TPlayerMove TTextPlayer::ReadPlayerMove() {
    TPlayerMove playerMove;
    playerMove.DisqualifyMe_ = false;

    int moveCount = 0;
    {
        std::string currentLine = ReadLine();
        if (currentLine == TTextPlayer::DISQUALIFY_ME) {
            return DisqualifyMe("Disqualify by player request");
        }
        std::stringstream ss(currentLine);
        if (!(ss >> moveCount)) {
            return DisqualifyMe("Failed to read move count from '" + currentLine + "'");
        }
    }

    if (moveCount < 0 || moveCount > MaxPlayerMovesPerReadMove_) {
        return DisqualifyMe("Invalid move count (negative or too big): " + std::to_string(moveCount));
    }

    for (int i = 0; i < moveCount; ++i) {
        std::string currentLine = ReadLine();
        if (currentLine == TTextPlayer::DISQUALIFY_ME) {
            return DisqualifyMe("Disqualify by player request");
        }
        std::stringstream ss(currentLine);

        TPlayerMove::TShipMove shipMove;
        if (!(ss >> shipMove.FromPlanetId_) || !(ss >> shipMove.ToPlanetId_) || !(ss >> shipMove.Count_)) {
            return DisqualifyMe("Failed to read ship move from '" + currentLine + "'");
        }

        playerMove.ShipMoves_.push_back(shipMove);
    }

    return playerMove;
}

