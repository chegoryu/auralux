//
// Created by Egor Chunaev on 14.06.2021.
//

#ifndef AURALUX_GAME_LOGGER_H
#define AURALUX_GAME_LOGGER_H

#include "game_state.h"
#include "player.h"

#include <string>
#include <vector>

class TGameLogger {
public:
    TGameLogger();

    void LogGameState(const TGameState& gameState);
    void LogFinalState(const TGameState& gameState);
    void LogError(const std::string& error);

    void LogInvalidMove(int playerId, const TPlayerMove::TShipMove& shipMove, const std::string& reason);
    void LogDisqualifyPlayer(int playerId, const std::string& reason);

    const std::vector<TGameState> GetGameStates() const;
    const TGameState& GetFinalGameState() const;
    const std::vector<std::string> GetErrors() const;

private:
    std::vector<TGameState> GameStates_;
    TGameState FinalGameState_;
    std::vector<std::string> Errors_;
};

#endif // AURALUX_GAME_LOGGER_H
