//
// Created by Egor Chunaev on 14.06.2021.
//

#include "text_player.h"

TPlayerMove TTextPlayer::GetMove(const TGameInfo& gameInfo) {
    (void)gameInfo;

    // TODO Create gameInfoStr
    // WriteGameInfo(gameInfoStr);

    TPlayerMove playerMove;
    try {
        playerMove = ReadPlayerMove();
    } catch (...) {
        playerMove.DisqualifyMe_ = true;
        playerMove.ShipMoves_.clear();
    }

    return playerMove;
}