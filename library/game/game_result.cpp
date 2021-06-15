//
// Created by Egor Chunaev on 15.06.2021.
//

#include "game_result.h"

namespace {

std::string GameResultEResultToString(TGameResult::EResult result) {
    switch (result) {
        case TGameResult::EResult::ONE_WINNER: {
            return "one winner";
        }
        case TGameResult::EResult::NO_WINNER: {
            return "no winner";
        }
        case TGameResult::EResult::DRAW: {
            return "draw";
        }
    }
}

template<typename T>
void PrintContainer(std::ostream& stream, const T& container) {
    if (container.empty()) {
        stream << "empty" << std::endl;
    } else {
        bool isFirst = true;
        for (const auto id : container) {
            stream << (isFirst ? "" : " ") << id;
            isFirst = false;
        }
        stream << std::endl;
    }
}

} // namespace

TGameResult GetGameResult(const TGameLogger& gameLogger) {
    const TGameState& finalGameState = gameLogger.GetFinalGameState();

    TGameResult gameResult;

    gameResult.WinnerId_ = -1;
    if (finalGameState.AlivePlayers_.size() == 1u) {
        gameResult.Result_ = TGameResult::EResult::ONE_WINNER;
        gameResult.WinnerId_ = *finalGameState.AlivePlayers_.begin();
    } else if (finalGameState.AlivePlayers_.empty()) {
        gameResult.Result_ = TGameResult::EResult::NO_WINNER;
    } else {
        gameResult.Result_ = TGameResult::EResult::DRAW;
    }

    gameResult.PlayerCount_ = static_cast<int>(
        finalGameState.AlivePlayers_.size() +
        finalGameState.DisqualifiedPlayers_.size() +
        finalGameState.DeadPlayers_.size()
    );
    gameResult.PlanetCount_ = static_cast<int>(finalGameState.PlanetInfos_.size());

    gameResult.FullSteps_ = static_cast<int>(gameLogger.GetGameStates().size() / gameResult.PlayerCount_);
    gameResult.TurnsInLastStep_ = static_cast<int>(gameLogger.GetGameStates().size() % gameResult.PlayerCount_);
    if (gameResult.TurnsInLastStep_ == 0) {
        gameResult.TurnsInLastStep_ = gameResult.PlayerCount_;
    }

    gameResult.ShipCount_ = 0;
    gameResult.NonDisqualifiedPlayersShipCount_ = 0;
    gameResult.PlayerPlanetCounts_.resize(gameResult.PlayerCount_, 0);
    gameResult.PlayerShipCounts_.resize(gameResult.PlayerCount_, 0);
    for (const auto& planetInfo : finalGameState.PlanetInfos_) {
        if (planetInfo.PlayerId_ == 0) {
            continue;
        }

        gameResult.ShipCount_ += planetInfo.ShipCount_;
        if (std::find(finalGameState.DisqualifiedPlayers_.begin(), finalGameState.DisqualifiedPlayers_.end(), planetInfo.PlayerId_) ==
            finalGameState.DisqualifiedPlayers_.end()
        ) {
            gameResult.NonDisqualifiedPlayersShipCount_ += planetInfo.ShipCount_;
        }

        ++gameResult.PlayerPlanetCounts_[planetInfo.PlayerId_ - 1];
        gameResult.PlayerShipCounts_[planetInfo.PlayerId_ - 1] += planetInfo.ShipCount_;
    }

    for (const auto& shipGroup : finalGameState.ShipGropus_) {
        gameResult.ShipCount_ += shipGroup.Count_;
        if (std::find(finalGameState.DisqualifiedPlayers_.begin(), finalGameState.DisqualifiedPlayers_.end(), shipGroup.PlayerId_) ==
            finalGameState.DisqualifiedPlayers_.end()
        ) {
            gameResult.NonDisqualifiedPlayersShipCount_ += shipGroup.Count_;
        }

        gameResult.PlayerShipCounts_[shipGroup.PlayerId_ - 1] += shipGroup.Count_;
    }

    gameResult.PlayerScores_.resize(gameResult.PlayerCount_, 0.0);
    if (gameResult.Result_ == TGameResult::EResult::ONE_WINNER) {
        gameResult.PlayerScores_[gameResult.WinnerId_ - 1] = 1.0;
    } else {
        for (size_t i = 0; i < gameResult.PlayerScores_.size(); ++i) {
            if (std::find(finalGameState.DisqualifiedPlayers_.begin(), finalGameState.DisqualifiedPlayers_.end(), static_cast<int>(i + 1)) ==
                finalGameState.DisqualifiedPlayers_.end()
            ) {
                gameResult.PlayerScores_[i] =
                    0.5 * (static_cast<double>(gameResult.PlayerShipCounts_[i]) / gameResult.NonDisqualifiedPlayersShipCount_) +
                    0.5 * (static_cast<double>(gameResult.PlayerPlanetCounts_[i]) / gameResult.PlanetCount_);
            } else {
                gameResult.PlayerScores_[i] = 0.0;
            }
        }
    }

    gameResult.AlivePlayers_ = std::vector<int>(finalGameState.AlivePlayers_.begin(), finalGameState.AlivePlayers_.end());

    gameResult.DisqualifiedPlayers_ = finalGameState.DisqualifiedPlayers_;
    std::sort(gameResult.DisqualifiedPlayers_.begin(), gameResult.DisqualifiedPlayers_.end());

    gameResult.DeadPlayers_ = finalGameState.DeadPlayers_;
    std::sort(gameResult.DeadPlayers_.begin(), gameResult.DeadPlayers_.end());

    return gameResult;
}

void PrintHumanReadableGameResult(std::ostream& stream, const TGameResult& gameResult) {
    stream << "Game result: " << GameResultEResultToString(gameResult.Result_) << std::endl;
    stream << "Winner id: " << gameResult.WinnerId_ << std::endl;

    stream << "Player count: " << gameResult.PlayerCount_ << std::endl;
    stream << "Planet count: " << gameResult.PlanetCount_ << std::endl;

    stream << "Full steps: " << gameResult.FullSteps_ << std::endl;
    stream << "Turns in last step: " << gameResult.TurnsInLastStep_ << std::endl;

    stream << "Ship count: " << gameResult.ShipCount_ << std::endl;
    stream << "Non disqualified players ship count: " << gameResult.NonDisqualifiedPlayersShipCount_ << std::endl;

    stream << "Player planet counts list: ";
    PrintContainer(stream, gameResult.PlayerPlanetCounts_);

    stream << "Player ship counts list: ";
    PrintContainer(stream, gameResult.PlayerShipCounts_);

    stream << "Player scores list: ";
    PrintContainer(stream, gameResult.PlayerScores_);

    stream << "Alive players list: ";
    PrintContainer(stream, gameResult.AlivePlayers_);

    stream << "Disqualified players list: ";
    PrintContainer(stream, gameResult.DisqualifiedPlayers_);

    stream << "Dead players list: ";
    PrintContainer(stream, gameResult.DeadPlayers_);
}