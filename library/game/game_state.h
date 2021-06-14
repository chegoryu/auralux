//
// Created by Egor Chunaev on 14.06.2021.
//

#ifndef AURALUX_GAME_STATE_H
#define AURALUX_GAME_STATE_H

#include <vector>

struct TGameMap {

};

struct TGameInfo {
    int PlayerId_;
    int PlayerCount_;
    TGameMap GameMap_;
};

struct TGameState {
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

#endif //AURALUX_GAME_STATE_H
