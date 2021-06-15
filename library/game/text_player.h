//
// Created by Egor Chunaev on 14.06.2021.
//

#ifndef AURALUX_TEXT_PLAYER_H
#define AURALUX_TEXT_PLAYER_H

#include "player.h"

#include <string>

class TTextPlayer : public IPlayer {
public:
    TTextPlayer(int maxPlayerMovesPerReadMove);

    void SendGameInfo(const TGameInfo& gameInfo) override final;
    TPlayerMove GetMove(const TGameState& gameInfo, const TLastShipMoves& lastShipMoves) override final;
    void SendGameOver() override final;

protected:
    virtual std::string ReadLine() = 0;
    virtual void WriteLine(const std::string& line) = 0;

private:
    TPlayerMove DisqualifyMe(const std::string& reason) const;

    void WriteGameInfo(const TGameInfo& gameInfo);
    void WriteGameState(const TGameState& gameState, const TLastShipMoves& lastShipMoves);
    void WriteGameOver();
    TPlayerMove ReadPlayerMove();

private:
    int MaxPlayerMovesPerReadMove_;
};

#endif // AURALUX_TEXT_PLAYER_H
