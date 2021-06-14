//
// Created by Egor Chunaev on 14.06.2021.
//

#include "game.h"

TGame::TGame(const TConfig& config)
    : Config_(config)
{
}

void TGame::AddPlayer(std::unique_ptr<TPlayer> player) {
    Players_.push_back({
        .PlayerEngine_ = std::move(player),
        .IsDisqualified_ = false,
    });
}

void TGame::Process() {
    Init();
    for (int stepId = 0; stepId < Config_.MaxSteps_; ++stepId) {
        Step();
    }

    // TODO some game result
}

void TGame::Init() {
    for (auto& player : Players_) {
        try {
            player.PlayerEngine_->SendGameInfo({} /* TODO gameInfo */);
        } catch (...) {
            player.IsDisqualified_ = true;
        }
    }
}

void TGame::Step() {
    // TODO
}