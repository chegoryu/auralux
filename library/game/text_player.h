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

    TPlayerMove GetMove(const TGameInfo& gameInfo) override;

protected:
    virtual void WriteGameInfo(const std::string& gameInfoStr) = 0;
    virtual std::string ReadPlayerMove() = 0;
};

#endif //AURALUX_TEXT_PLAYER_H
