//
// Created by Egor Chunaev on 14.06.2021.
//

#include "default_players.h"

void TAFKPlayer::SendGameInfo(const TGameInfo& gameInfo) {
    (void)gameInfo;
}

TPlayerMove TAFKPlayer::GetMove(const TGameState& gameState, const TLastShipMoves& lastShipMoves) {
    (void)gameState;
    (void)lastShipMoves;

    TPlayerMove playerMove;
    return playerMove;
}