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
    std::string DisqualifyReason_;
    std::vector<TShipMove> ShipMoves_;
};

struct TLastShipMoves {
    struct TShipMove {
        int PlayerId_;
        int FromPlanetId_;
        int ToPlanetId_;
        int Count_;
    };

    std::vector<TShipMove> ShipMoves_;
};

class IPlayer {
public:
    IPlayer() = default;
    virtual ~IPlayer() = default;

    virtual void SendGameInfo(const TGameInfo& gameInfo) = 0;
    virtual TPlayerMove GetMove(const TGameState& gameState, const TLastShipMoves& lastShipMoves) = 0;
};

#endif // AURALUX_PLAYER_H
