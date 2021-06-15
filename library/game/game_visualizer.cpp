//
// Created by Egor Chunaev on 15.06.2021.
//

#include "game_visualizer.h"

void PrintGameLogInVisualizerFormat(std::ostream& stream, const TGameMap& gameMap, const TGameLogger& gameLogger) {
    assert(gameMap.Points_);

    stream << gameMap.Points_->size() << std::endl;
    for (const auto& point : *gameMap.Points_) {
        stream << point.x << " " << point.y << std::endl;
    }

    for (const auto& gameTurn : gameLogger.GetGameTurns()) {
        stream << gameTurn.LastShipMoves_.ShipMoves_.size() << std::endl;
        for (const auto& shipMove : gameTurn.LastShipMoves_.ShipMoves_) {
            stream
                << shipMove.PlayerId_ << " "
                << shipMove.FromPlanetId_ << " "
                << shipMove.ToPlanetId_ << " "
                << shipMove.Count_ << std::endl;
        }

        for (const auto& planetInfo : gameTurn.GameState_.PlanetInfos_) {
            stream
                << planetInfo.PlayerId_ << " "
                << planetInfo.ShipCount_ << " "
                << planetInfo.Level_ << " "
                << planetInfo.Armor_ << std::endl;
        }
    }

    stream << -1 << std::endl;
}