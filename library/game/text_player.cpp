//
// Created by Egor Chunaev on 14.06.2021.
//

#include "text_player.h"

/*

 INIT INFO START
 PlanetCount PlayerCount
 <Matrix PlanetCount x PlanetCount>
 INIT INFO END
*/

/*
 PER MOVE INFO START

 <PlanetCount lines>
 Captured PlayerId ShipCount Level Armor

 <number of moves>
 PlayerId FromPlanetId ToPlanetId Count

 PER MOVE INFO END
*/

void TTextPlayer::SendGameInfo(const TGameInfo& gameInfo) {
    WriteGameInfo(gameInfo);
}

TPlayerMove TTextPlayer::GetMove(const TGameState& gameState, const TLastShipMoves& lastShipMoves) {
    WriteGameState(gameState, lastShipMoves);
    return ReadPlayerMove();
}

void TTextPlayer::WriteGameInfo(const TGameInfo& gameInfo) {
    (void)gameInfo;
    // TODO
}

void TTextPlayer::WriteGameState(const TGameState& gameState, const TLastShipMoves& lastShipMoves) {
    (void)gameState;
    (void)lastShipMoves;
    // TODO
}

TPlayerMove TTextPlayer::ReadPlayerMove() {
    // TODO
    return {};
}

