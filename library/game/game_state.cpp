//
// Created by Egor Chunaev on 14.06.2021.
//

#include "game_state.h"

#include <sstream>

namespace {

template<typename T>
void PrintContainer(std::ostream& stream, const T& container) {
    stream << container.size() << '\n';
    bool isFirst = true;
    for (const auto id : container) {
        stream << (isFirst ? "" : " ") << id;
        isFirst = false;
    }
    stream << '\n';
}

} // namespace

void PrintGameState(std::ostream& stream, const TGameState& gameState) {
    {
        for (const auto& planetInfo : gameState.PlanetInfos_) {
            stream
                << planetInfo.PlayerId_ << " "
                << planetInfo.ShipCount_ << " "
                << planetInfo.Level_ << " "
                << planetInfo.Armor_ << '\n';
        }
    }

    {
        stream << gameState.ShipGropus_.size() << '\n';
        for (const auto& shipGroup : gameState.ShipGropus_) {
            stream
                << shipGroup.PlayerId_ << " "
                << shipGroup.FromPlanetId_ << " "
                << shipGroup.ToPlanetId_ << " "
                << shipGroup.Count_ << " "
                << shipGroup.TimeToDestination_ << '\n';
        }
    }

    PrintContainer(stream, gameState.AlivePlayers_);
    PrintContainer(stream, gameState.DisqualifiedPlayers_);
    PrintContainer(stream, gameState.DeadPlayers_);
}