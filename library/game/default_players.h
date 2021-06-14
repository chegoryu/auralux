//
// Created by Egor Chunaev on 14.06.2021.
//

#ifndef AURALUX_DEFAULT_PLAYERS_H
#define AURALUX_DEFAULT_PLAYERS_H

#include "player.h"
#include "text_player.h"

class TAFKPlayer : public IPlayer {
public:
    using IPlayer::IPlayer;

    void SendGameInfo(const TGameInfo& gameInfo) override;
    TPlayerMove GetMove(const TGameState& gameState, const TLastShipMoves& lastShipMoves) override;
};

class TDisqualifyPlayer : public IPlayer {
public:
    using IPlayer::IPlayer;

    void SendGameInfo(const TGameInfo& gameInfo) override;
    TPlayerMove GetMove(const TGameState& gameState, const TLastShipMoves& lastShipMoves) override;

private:
    int StepToDisqualify_ = 100;
};

class TUpgradeAndRepairMainPlayer : public IPlayer {
public:
    using IPlayer::IPlayer;

    void SendGameInfo(const TGameInfo& gameInfo) override;
    TPlayerMove GetMove(const TGameState& gameState, const TLastShipMoves& lastShipMoves) override;

private:
    int PlayerId_;
};

class TAggressiveExpansionPlayer : public IPlayer {
public:
    using IPlayer::IPlayer;

    void SendGameInfo(const TGameInfo& gameInfo) override;
    TPlayerMove GetMove(const TGameState& gameState, const TLastShipMoves& lastShipMoves) override;

private:
    int ChooseTargetPlanet(const TGameState& gameState);

private:
    static constexpr int MAX_PLANET_LEVEL = 3;
    static constexpr int MIN_SHIPS_TO_MOVE = 100;

    std::vector<int> PlanetOrder_;
    int PlayerId_;
};

class TStdinStdoutPlayer : public TTextPlayer {
public:
    using TTextPlayer::TTextPlayer;

protected:
    std::string ReadLine() override;
    void WriteLine(const std::string& line) override;
};

#endif // AURALUX_DEFAULT_PLAYERS_H
