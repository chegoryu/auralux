//
// Created by Egor Chunaev on 14.06.2021.
//

#ifndef AURALUX_GAME_H
#define AURALUX_GAME_H

#include "player.h"

class TGame {
public:
    struct TConfig {
        int SomeVal_ = 0;
    };

public:
    TGame(const TConfig& config);

    void AddPlayer(std::unique_ptr<TPlayer> player);

    void LoadConfig(const TConfig& config);
    void Step();

private:
    const TConfig Config_;
    std::vector<std::unique_ptr<TPlayer>> Players_;
};

#endif //AURALUX_GAME_H
