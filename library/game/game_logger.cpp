//
// Created by Egor Chunaev on 14.06.2021.
//

#include "game_logger.h"

#include <sstream>

TGameLogger::TGameLogger(bool logAllGameStates)
    : LogAllGamesStates_(logAllGameStates)
{}

void TGameLogger::LogGameState(const TGameState& gameState) {
    if (LogAllGamesStates_) {
        GameStates_.push_back(gameState);
    } else {
        // Empty record to count steps and turns
        GameStates_.push_back({});
    }
}

void TGameLogger::LogFinalState(const TGameState& gameState) {
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

const std::vector<TGameState>& TGameLogger::GetGameStates() const {
    return GameStates_;
}

const TGameState& TGameLogger::GetFinalGameState() const {
    return FinalGameState_;
}

const std::vector<std::string>& TGameLogger::GetErrors() const {
    return Errors_;
}
