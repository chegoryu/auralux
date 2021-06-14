//
// Created by Egor Chunaev on 14.06.2021.
//

#ifndef AURALUX_PLAYER_H
#define AURALUX_PLAYER_H

#include "game_state.h"

#include <vector>

struct TPlayerMove {
    struct TShipMove {
        int FromPlanetId_;
        int ToPlanetId_;
        int Count_;
    };

    bool DisqualifyMe_ = false;
    std::vector<TShipMove> ShipMoves_;
};

class IPlayer {
public:
    IPlayer() = default;
    virtual ~IPlayer() = default;

    virtual void SendGameInfo(const TGameInfo& gameInfo) = 0;
    virtual TPlayerMove GetMove(const TGameState& gameState) = 0;
};

#endif // AURALUX_PLAYER_H
