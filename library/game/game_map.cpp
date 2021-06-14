//
// Created by Egor Chunaev on 14.06.2021.
//

#include "game_map.h"

TGameMap LoadPlanarGraph(std::function<int()> readInt) {
    TGameMap gameMap;

    int planetCount = readInt();
    int playerCount = readInt();

    for (int i = 0; i < playerCount; ++i) {
        gameMap.StartPlanets_.push_back(readInt());
    }

    std::vector<std::pair<int, int>> coords;
    for (int i = 0; i < planetCount; ++i) {
        std::pair<int, int> coord;
        coord.first = readInt();
        coord.second = readInt();
        coords.push_back(coord);
    }

    gameMap.Dists_.resize(planetCount, std::vector<int>(planetCount, 0));

    for (int i = 0; i < planetCount; ++i) {
        for (int j = i + 1; j < planetCount; ++j) {
            long long int dx = coords[i].first - coords[j].first;
            long long int dy = coords[i].second - coords[j].second;
            long long int sqDist = dx * dx + dy * dy;
            int dist = static_cast<int>(sqrtl(sqDist));
            gameMap.Dists_[i][j] = gameMap.Dists_[j][i] = dist;
        }
    }

    return gameMap;
}
