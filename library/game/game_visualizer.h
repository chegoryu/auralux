//
// Created by Egor Chunaev on 15.06.2021.
//

#ifndef AURALUX_GAME_VISUALIZER_H
#define AURALUX_GAME_VISUALIZER_H

#include "game_logger.h"
#include "game_state.h"

#include <ostream>

void PrintGameLogInVisualizerFormat(std::ostream& stream, const TGameMap& gameMap, const TGameLogger& gameLogger);

#endif // AURALUX_GAME_VISUALIZER_H
