//
// Created by Egor Chunaev on 14.06.2021.
//

#include "text_player.h"

void TTextPlayer::SendGameInfo(const TGameInfo& gameInfo) {
    WriteGameInfo(gameInfo);
}

TPlayerMove TTextPlayer::GetMove(const TGameState& gameState) {
    WriteGameState(gameState);
    return ReadPlayerMove();
}