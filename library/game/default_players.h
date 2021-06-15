//
// Created by Egor Chunaev on 14.06.2021.
//

#ifndef AURALUX_DEFAULT_PLAYERS_H
#define AURALUX_DEFAULT_PLAYERS_H

#include "player.h"
#include "text_player.h"

#include <memory>

class TAutoGameOverPlayer : public IPlayer {
public:
    using IPlayer::IPlayer;

    void SendGameOver() override;
};

class TAFKPlayer : public TAutoGameOverPlayer {
public:
    using TAutoGameOverPlayer::TAutoGameOverPlayer;

    void SendGameInfo(const TGameInfo& gameInfo) override;
    TPlayerMove GetMove(const TGameState& gameState, const TLastShipMoves& lastShipMoves) override;
};

class TDisqualifyPlayer : public TAutoGameOverPlayer  {
public:
    using TAutoGameOverPlayer::TAutoGameOverPlayer;

    void SendGameInfo(const TGameInfo& gameInfo) override;
    TPlayerMove GetMove(const TGameState& gameState, const TLastShipMoves& lastShipMoves) override;

private:
    int StepToDisqualify_ = 100;
};

class TUpgradeAndRepairMainPlayer : public TAutoGameOverPlayer {
public:
    using TAutoGameOverPlayer::TAutoGameOverPlayer;

    void SendGameInfo(const TGameInfo& gameInfo) override;
    TPlayerMove GetMove(const TGameState& gameState, const TLastShipMoves& lastShipMoves) override;

private:
    int PlayerId_;
};

class TAggressiveExpansionPlayer : public TAutoGameOverPlayer {
public:
    enum EGameStyle {
        RANDOM = 0,
        NEAREST = 1,
    };

public:
    TAggressiveExpansionPlayer(EGameStyle gameStyle = EGameStyle::RANDOM);

    void SendGameInfo(const TGameInfo& gameInfo) override;
    TPlayerMove GetMove(const TGameState& gameState, const TLastShipMoves& lastShipMoves) override;

private:
    int ChooseTargetPlanet(const TGameState& gameState);

private:
    static constexpr int MAX_PLANET_LEVEL = 3;
    static constexpr int MIN_SHIPS_TO_MOVE = 100;
    static constexpr int MIN_ARMOR_TO_REPAIR[] = {0, 75, 150, 225};

    EGameStyle GameStyle_;
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

std::unique_ptr<IPlayer> CreateDefaultPlayer(const std::string& playerType);
bool HasDefaultPlayer(const std::string& playerType);

#endif // AURALUX_DEFAULT_PLAYERS_H
