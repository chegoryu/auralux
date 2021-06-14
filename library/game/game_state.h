//
// Created by Egor Chunaev on 14.06.2021.
//

#ifndef AURALUX_GAME_STATE_H
#define AURALUX_GAME_STATE_H

#include "game_map.h"

#include <set>
#include <vector>

struct TGameInfo {
    int PlayerId_;
    int PlayerCount_;
    TGameMap GameMap_;
};

struct TGameState {
    struct TShipGroup {
        int PlayerId_;
        int FromPlanetId_;
        int ToPlanetId_;
        int Count_;
        int TimeToDestination_;
    };

    struct TPlanetInfo {
        int PlayerId_;
        int ShipCount_;
        int Level_;
        int Armor_;
    };

    std::vector<TPlanetInfo> PlanetInfos_;
    std::vector<TShipGroup> ShipGropus_;
    std::set<int> AlivePlayers_;
    std::vector<int> DisqualifiedPlayers_;
    std::vector<int> DeadPlayers_;
    bool IsFirstStep_ = true;
};

#endif // AURALUX_GAME_STATE_H
