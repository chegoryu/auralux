//
// Created by Egor Chunaev on 14.06.2021.
//

#ifndef AURALUX_GAME_MAP_H
#define AURALUX_GAME_MAP_H

#include <functional>
#include <optional>
#include <vector>

struct TGameMap {
    struct TPoint {
        int x;
        int y;
    };

    std::vector<int> StartPlanets_;
    std::vector<std::vector<int>> Dists_;

    // May be missing
    // Helper for planar graphs
    std::optional<std::vector<TPoint>> Points_ = {};
};

TGameMap LoadPlanarGraph(int maxDistBetweenPlanets, std::function<int()> readInt);

#endif // AURALUX_GAME_MAP_H
