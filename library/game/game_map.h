//
// Created by Egor Chunaev on 14.06.2021.
//

#ifndef AURALUX_GAME_MAP_H
#define AURALUX_GAME_MAP_H

#include <functional>
#include <vector>

struct TGameMap {
    std::vector<int> StartPlanets_;
    std::vector<std::vector<int>> Dists_;
};

TGameMap LoadPlanarGraph(std::function<int()> readInt);

#endif // AURALUX_GAME_MAP_H
