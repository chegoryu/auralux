//
// Created by Egor Chunaev on 14.06.2021.
//

#ifndef AURALUX_GAME_STATE_H
#define AURALUX_GAME_STATE_H

#include <vector>

struct TGameMap {
    std::vector<int> StartPlanets_;
    std::vector<std::vector<int>> Dists_;
};

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
};

#endif // AURALUX_GAME_STATE_H
