//
// Created by Egor Chunaev on 14.06.2021.
//

#ifndef AURALUX_PLAYER_H
#define AURALUX_PLAYER_H

#include <vector>

struct TGameInfo {
    struct TShipMove {
        int PlayerId_;
        int FromPlanetId_;
        int ToPlanetId_;
        int Count_;
        int TimeToDestination_;
    };

    struct TPlanetInfo {
        bool Captured_ = false;
        int PlayerId_ = 0;
        int ShipCount_ = 0;
        int Level_ = 0;
    };

    std::vector<TPlanetInfo> PlanetInfos_;
    std::vector<TShipMove> ShipMoves_;
};

struct TPlayerMove {
    struct TShipMove {
        int FromPlanetId_;
        int ToPlanetId_;
        int Count_;
    };

    bool DisqualifyMe_ = false;
    std::vector<TShipMove> ShipMoves_;
};

class TPlayer {
public:
    TPlayer() = default;
    virtual ~TPlayer() = default;

    virtual TPlayerMove GetMove(const TGameInfo& gameInfo) = 0;
};

#endif //AURALUX_PLAYER_H
