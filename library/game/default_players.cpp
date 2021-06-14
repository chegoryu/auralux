//
// Created by Egor Chunaev on 14.06.2021.
//

#include "default_players.h"

#include <iostream>

void TAFKPlayer::SendGameInfo(const TGameInfo& gameInfo) {
    (void)gameInfo;
}

TPlayerMove TAFKPlayer::GetMove(const TGameState& gameState, const TLastShipMoves& lastShipMoves) {
    (void)gameState;
    (void)lastShipMoves;

    TPlayerMove playerMove;
    return playerMove;
}

std::string TStdinStdoutPlayer::ReadLine() {
    std::string line;
    getline(std::cin, line);

    return line;
}

void TStdinStdoutPlayer::WriteLine(const std::string& line) {
    std::cout << line << std::endl;
}