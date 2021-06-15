//
// Created by Egor Chunaev on 14.06.2021.
//

#include "game_map.h"

#include <cassert>
#include <cmath>

TGameMap LoadPlanarGraph(std::function<int()> readInt) {
    TGameMap gameMap;

    int planetCount = readInt();
    int playerCount = readInt();

    for (int i = 0; i < playerCount; ++i) {
        gameMap.StartPlanets_.push_back(readInt());
    }

    gameMap.Points_ = std::vector<TGameMap::TPoint>();
    gameMap.Points_->resize(planetCount);
    for (int i = 0; i < planetCount; ++i) {
        gameMap.Points_->at(i).x = readInt();
        gameMap.Points_->at(i).y = readInt();
    }

    gameMap.Dists_.resize(planetCount, std::vector<int>(planetCount, 0));

    for (int i = 0; i < planetCount; ++i) {
        for (int j = i + 1; j < planetCount; ++j) {
            long long int dx = gameMap.Points_->at(i).x - gameMap.Points_->at(j).x;
            long long int dy = gameMap.Points_->at(i).y - gameMap.Points_->at(j).y;
            long long int sqDist = dx * dx + dy * dy;
            int dist = std::round(sqrtl(sqDist));
            assert(dist > 0);
            assert(dist <= 100);
            gameMap.Dists_[i][j] = gameMap.Dists_[j][i] = dist;
        }
    }

    return gameMap;
}
