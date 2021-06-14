//
// Created by Egor Chunaev on 14.06.2021.
//

#ifndef AURALUX_TEXT_PLAYER_H
#define AURALUX_TEXT_PLAYER_H

#include "player.h"

#include <string>

class TTextPlayer : public IPlayer {
public:
    using IPlayer::IPlayer;

    void SendGameInfo(const TGameInfo& gameInfo) override;
    TPlayerMove GetMove(const TGameState& gameInfo, const TLastShipMoves& lastShipMoves) override;

protected:
    virtual std::string ReadLine() = 0;
    virtual void WriteLine(const std::string& line) = 0;

private:
    void WriteGameInfo(const TGameInfo& gameInfo);
    void WriteGameState(const TGameState& gameState, const TLastShipMoves& lastShipMoves);
    TPlayerMove ReadPlayerMove();
};

#endif // AURALUX_TEXT_PLAYER_H
