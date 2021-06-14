//
// Created by Egor Chunaev on 14.06.2021.
//

#ifndef AURALUX_GAME_H
#define AURALUX_GAME_H

#include "game_state.h"
#include "player.h"

class TGame {
public:
    struct TConfig {
        int MaxSteps_ = 0;
        TGameMap GameMap_;
    };

private:
    struct TPlayerInfo {
        std::unique_ptr<IPlayer> PlayerEngine_;
        bool IsDisqualified_;
    };

public:
    TGame(const TConfig& config);

    void AddPlayer(std::unique_ptr<IPlayer> player);

    void Process();

private:
    void Init();
    void Step();

private:
    const TConfig Config_;
    std::vector<TPlayerInfo> Players_;
};

#endif // AURALUX_GAME_H
