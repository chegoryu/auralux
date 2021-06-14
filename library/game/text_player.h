//
// Created by Egor Chunaev on 14.06.2021.
//

#ifndef AURALUX_TEXT_PLAYER_H
#define AURALUX_TEXT_PLAYER_H

#include "player.h"

#include <string>

class TTextPlayer : public TPlayer {
public:
    using TPlayer::TPlayer;

    void SendGameInfo(const TGameInfo& gameInfo) override;
    TPlayerMove GetMove(const TGameState& gameInfo) override;

protected:
    virtual void WriteGameInfo(const TGameInfo& gameInfo) = 0;
    virtual void WriteGameState(const TGameState& gameState) = 0;
    virtual TPlayerMove ReadPlayerMove() = 0;
};

#endif // AURALUX_TEXT_PLAYER_H
