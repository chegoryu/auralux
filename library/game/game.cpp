//
// Created by Egor Chunaev on 14.06.2021.
//

#include "game.h"

TGame::TGame(const TConfig& config)
    : Config_(config)
{
}

void TGame::AddPlayer(std::unique_ptr<TPlayer> player) {
    Players_.push_back(std::move(player));
}

void TGame::LoadConfig(const TConfig& config) {
    (void)config;
    // TODO
}

void TGame::Step() {
    // TODO
}