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
    struct TGameTurn {
        TGameState GameState_;
        TLastShipMoves LastShipMoves_;
    };

public:
    TGameLogger(bool logPlanetInfoOnly);

    void LogGameTurn(const TGameState& gameState, const TLastShipMoves& lastShipMoves);
    void LogFinalGameState(const TGameState& gameState);
    void LogError(const std::string& error);

    void LogInvalidMove(int playerId, const TPlayerMove::TShipMove& shipMove, const std::string& reason);
    void LogDisqualifyPlayer(int playerId, const std::string& reason);

    const std::vector<TGameTurn>& GetGameTurns() const;
    const TGameState& GetFinalGameState() const;
    const std::vector<std::string>& GetErrors() const;

private:
    const bool LogPlanetInfoOnly_;

    std::vector<TGameTurn> GameTurns_;
    TGameState FinalGameState_;
    std::vector<std::string> Errors_;
};

#endif // AURALUX_GAME_LOGGER_H
