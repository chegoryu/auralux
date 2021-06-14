//
// Created by Egor Chunaev on 14.06.2021.
//

#ifndef AURALUX_DEFAULT_PLAYERS_H
#define AURALUX_DEFAULT_PLAYERS_H

#include "player.h"

class TAFKPlayer : public IPlayer {
public:
    using IPlayer::IPlayer;

    void SendGameInfo(const TGameInfo& gameInfo) override;
    TPlayerMove GetMove(const TGameState& gameState) override;
};

#endif //AURALUX_DEFAULT_PLAYERS_H
