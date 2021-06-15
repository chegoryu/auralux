//
// Created by Egor Chunaev on 14.06.2021.
//

#include "game_logger.h"

#include <sstream>

TGameLogger::TGameLogger(bool logPlanetInfoOnly)
    : LogPlanetInfoOnly_(logPlanetInfoOnly)
{}

void TGameLogger::LogGameTurn(const TGameState& gameState, const TLastShipMoves& lastShipMoves) {
    TGameTurn gameTurn;

    if (LogPlanetInfoOnly_) {
        gameTurn.GameState_.PlanetInfos_ = gameState.PlanetInfos_;
    } else {
        gameTurn.GameState_ = gameState;
    }

    gameTurn.LastShipMoves_ = lastShipMoves;

    GameTurns_.push_back(gameTurn);
}

void TGameLogger::LogFinalGameState(const TGameState& gameState) {
    FinalGameState_ = gameState;
}

void TGameLogger::LogError(const std::string& error) {
    Errors_.push_back(error);
}

void TGameLogger::LogInvalidMove(int playerId, const TPlayerMove::TShipMove& shipMove, const std::string& reason) {
    std::stringstream ss;
    ss
        << "Invalid player '" << std::to_string(playerId)
        << "' move '" << shipMove.FromPlanetId_ << " " << shipMove.ToPlanetId_ << " " << shipMove.Count_ << "': "
        << reason;
    LogError(ss.str());
}

void TGameLogger::LogDisqualifyPlayer(int playerId, const std::string& reason) {
    std::stringstream ss;
    ss
        << "Disqualify player '" << std::to_string(playerId) << "': "
        << reason;
    LogError(ss.str());
}

const std::vector<TGameLogger::TGameTurn>& TGameLogger::GetGameTurns() const {
    return GameTurns_;
}

const TGameState& TGameLogger::GetFinalGameState() const {
    return FinalGameState_;
}

const std::vector<std::string>& TGameLogger::GetErrors() const {
    return Errors_;
}
