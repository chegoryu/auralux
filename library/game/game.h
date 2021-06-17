//
// Created by Egor Chunaev on 14.06.2021.
//

#ifndef AURALUX_GAME_H
#define AURALUX_GAME_H

#include "game_state.h"
#include "game_logger.h"
#include "player.h"

#include <functional>
#include <memory>

class TGame {
public:
    struct TConfig {
        int MaxSteps_ = 10000;

        int StartShipsCount_ = 100;

        int ShipsToCapturePlanet_ = 100;
        std::vector<int> ShipsToUpgradePlanet_ = {0, 100, 200};
        std::vector<int> PerLevelPlanetArmor_ = {0, 100, 200, 300};
        int PlanetProductionMultiply_ = 1;

        int MaxShipGroupsInSpace_ = 1000;
        int MaxPlayerShipMovesPerStep_ = 1000;

        TGameMap GameMap_;
    };

    using TOnNewStepCallback = std::function<void(int)>;

private:
    struct TPlayerInfo {
        std::unique_ptr<IPlayer> PlayerEngine_;
        int PlayerId_;
        bool IsDead_;
        bool IsDisqualified_;
    };

public:
    explicit TGame(const TConfig& config);

    void AddPlayer(std::unique_ptr<IPlayer> player);
    void SetOnNewStepCallback(TOnNewStepCallback&& onNewStepCallback);
    void Process();

    [[nodiscard]] const TConfig& GetGameConfig() const;
    [[nodiscard]] const TGameLogger& GetGameLogger() const;

private:
    bool Init();
    bool Step();
    void Finish();
    void PrePlayerMove(int playerId);
    void PlayerMove(int playerId);

    void DisqualifyPlayer(int playerId, const std::string& reason);
    void MarkPlayerAsDead(int playerId);

    bool IsPlayerDead(int playerId) const;
    int GetShipGroupsInSpace(int playerId) const;
    bool IsValidPlanetId(int planetId) const ;

private:
    const TConfig Config_;
    TGameState GameState_;
    std::vector<TLastShipMoves> LastShipMovesByPlayer_;
    std::vector<TPlayerInfo> Players_;

    TGameLogger GameLogger_;
    TOnNewStepCallback OnNewStepCallback_ = [](int){};
};

#endif // AURALUX_GAME_H
