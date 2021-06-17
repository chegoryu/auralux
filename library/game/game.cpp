//
// Created by Egor Chunaev on 14.06.2021.
//

#include "game.h"

#include <cassert>
#include <set>

TGame::TGame(const TConfig& config)
    : Config_(config)
    , GameLogger_(true)
{
    assert(Config_.ShipsToUpgradePlanet_.size() + 1 == Config_.PerLevelPlanetArmor_.size());

    for (size_t i = 0; i < config.GameMap_.Dists_.size(); ++i) {
        GameState_.PlanetInfos_.push_back({
            .PlayerId_ = 0,
            .ShipCount_ = 0,
            .Level_ = 0,
            .Armor_ = 0,
        });
    }

    for (size_t i = 0; i < config.GameMap_.StartPlanets_.size(); ++i) {
        auto& planet = GameState_.PlanetInfos_.at(config.GameMap_.StartPlanets_.at(i) - 1);
        planet.PlayerId_ = i + 1;
        planet.ShipCount_ = Config_.StartShipsCount_;
        planet.Level_ = 1;
        planet.Armor_ = Config_.PerLevelPlanetArmor_[planet.Level_];

        GameState_.AlivePlayers_.insert(i + 1);
    }


    GameState_.IsFirstStep_ = true;
}

void TGame::AddPlayer(std::unique_ptr<IPlayer> player) {
    Players_.push_back({
        .PlayerEngine_ = std::move(player),
        .PlayerId_ = static_cast<int>(Players_.size()) + 1,
        .IsDead_ = false,
        .IsDisqualified_ = false,
    });
    LastShipMovesByPlayer_.push_back({});
}

void TGame::SetOnNewStepCallback(TOnNewStepCallback&& onNewStepCallback) {
    OnNewStepCallback_ = onNewStepCallback;
}

void TGame::Process() {
    assert(Players_.size() == Config_.GameMap_.StartPlanets_.size());

    if (Init()) {
        for (int stepId = 0; stepId < Config_.MaxSteps_; ++stepId) {
            OnNewStepCallback_(stepId);

            if (!Step()) {
                break;
            }
        }
    }

    Finish();

    GameLogger_.LogFinalGameState(GameState_);
}

const TGame::TConfig& TGame::GetGameConfig() const {
    return Config_;
}

const TGameLogger& TGame::GetGameLogger() const {
    return GameLogger_;
}

bool TGame::Init() {
    for (auto& player : Players_) {
        try {
            TGameInfo gameInfo;
            gameInfo.PlayerId_ = player.PlayerId_;
            gameInfo.PlayerCount_ = Players_.size();
            gameInfo.GameMap_ = Config_.GameMap_;

            player.PlayerEngine_->SendGameInfo(gameInfo);
        } catch (const std::exception& e) {
            DisqualifyPlayer(player.PlayerId_, "failed send game info to player: " + std::string(e.what()));
        }
    }

    return GameState_.AlivePlayers_.size() > 1;
}

bool TGame::Step() {
    for (size_t i = 1; i <= Players_.size(); ++i) {
        PrePlayerMove(static_cast<int>(i));
        try {
            PlayerMove(static_cast<int>(i));
        } catch (const std::exception& e) {
            DisqualifyPlayer(i, "failed to do player move: " + std::string(e.what()));
        }

        GameLogger_.LogGameTurn(GameState_, LastShipMovesByPlayer_.at(i - 1));

        if (GameState_.AlivePlayers_.size() <= 1) {
            return false;
        }
    }

    if (GameState_.IsFirstStep_) {
        GameState_.IsFirstStep_ = false;
    }

    return true;
}

void TGame::Finish() {
    for (size_t i = 1; i <= Players_.size(); ++i) {
        try {
            Players_[i - 1].PlayerEngine_->SendGameOver();
        } catch (const std::exception& e) {
            DisqualifyPlayer(i, "failed to do send game over: " + std::string(e.what()));
        }
    }
}

void TGame::PrePlayerMove(int playerId) {
    if (GameState_.IsFirstStep_) {
        return;
    }

    LastShipMovesByPlayer_.at(playerId - 1).ShipMoves_.clear();

    // Production
    for (auto& planetInfo : GameState_.PlanetInfos_) {
        if (planetInfo.Level_ > 0 && planetInfo.PlayerId_ == playerId) {
            planetInfo.ShipCount_ += Config_.PlanetProductionMultiply_ * planetInfo.Level_;
        }
    }

    // Ship moves
    std::vector<TGameState::TShipGroup> newShipGroups;
    for (auto& shipGroup : GameState_.ShipGropus_) {
        if (shipGroup.PlayerId_ == playerId) {
            if (--shipGroup.TimeToDestination_ == 0) {
                assert(shipGroup.FromPlanetId_ != shipGroup.ToPlanetId_);
                auto& currentPlanet = GameState_.PlanetInfos_.at(shipGroup.ToPlanetId_ - 1);

                if (currentPlanet.PlayerId_ == playerId) {
                    currentPlanet.ShipCount_ += shipGroup.Count_;
                } else {
                    if (currentPlanet.ShipCount_ >= shipGroup.Count_) {
                        currentPlanet.ShipCount_ -= shipGroup.Count_;
                    } else {
                        shipGroup.Count_ -= currentPlanet.ShipCount_;
                        currentPlanet.ShipCount_ = 0;

                        if (currentPlanet.Level_ == 0) {
                            currentPlanet.ShipCount_ = shipGroup.Count_;
                            currentPlanet.Armor_ = 0;
                            currentPlanet.Level_ = 0;
                            currentPlanet.PlayerId_ = playerId;
                        } else if (currentPlanet.Armor_ > shipGroup.Count_) {
                            currentPlanet.Armor_ -= shipGroup.Count_;
                        } else {
                            currentPlanet.ShipCount_ = shipGroup.Count_ - currentPlanet.Armor_;
                            currentPlanet.Armor_ = 0;
                            currentPlanet.Level_ = 0;
                            currentPlanet.PlayerId_ = playerId;
                        }
                    }
                }

                if (currentPlanet.ShipCount_ == 0 && currentPlanet.Level_ == 0) {
                    currentPlanet.PlayerId_ = 0;
                }
            } else {
                newShipGroups.push_back(std::move(shipGroup));
            }
        } else {
            newShipGroups.push_back(std::move(shipGroup));
        }
    }

    GameState_.ShipGropus_.swap(newShipGroups);
}

void TGame::PlayerMove(int playerId) {
    auto& playerInfo = Players_.at(playerId - 1);
    if (playerInfo.IsDisqualified_ || playerInfo.IsDead_) {
        return;
    }

    if (IsPlayerDead(playerId)) {
        MarkPlayerAsDead(playerId);
        return;
    }

    TLastShipMoves lastShipMoves;
    for (const auto& lastPlayerShipMoves : LastShipMovesByPlayer_) {
        lastShipMoves.ShipMoves_.reserve(lastShipMoves.ShipMoves_.size() + lastPlayerShipMoves.ShipMoves_.size());
        for (const auto& playerShipMove : lastPlayerShipMoves.ShipMoves_) {
            lastShipMoves.ShipMoves_.push_back(playerShipMove);
        }
    }

    TPlayerMove playerMove = playerInfo.PlayerEngine_->GetMove(GameState_, lastShipMoves);
    if (playerMove.DisqualifyMe_) {
        DisqualifyPlayer(playerId, playerMove.DisqualifyReason_);
        return;
    }

    if (playerMove.ShipMoves_.size() > Config_.MaxPlayerShipMovesPerStep_) {
        DisqualifyPlayer(playerId, "too many moves " + std::to_string(playerMove.ShipMoves_.size()));
        return;
    }

    int shipGroupsInSpace = GetShipGroupsInSpace(playerId);
    std::set<int> alreadyUpgraded_;
    for (const auto& shipMove : playerMove.ShipMoves_) {
        if (!IsValidPlanetId(shipMove.FromPlanetId_) || !IsValidPlanetId(shipMove.ToPlanetId_) || shipMove.Count_ <= 0) {
            GameLogger_.LogInvalidMove(playerId, shipMove, "Planet ids or ship count invalid");
            continue;
        }

        auto& fromPlanet = GameState_.PlanetInfos_.at(shipMove.FromPlanetId_ - 1);
        if (fromPlanet.PlayerId_ != playerId) {
            GameLogger_.LogInvalidMove(
                playerId,
                shipMove,
                "planet " + std::to_string(shipMove.FromPlanetId_) + " belongs to another player"
            );
            continue;
        }

        if (fromPlanet.ShipCount_ < shipMove.Count_) {
            GameLogger_.LogInvalidMove(
                playerId,
                shipMove,
                "planet " + std::to_string(shipMove.FromPlanetId_) + " belongs to another player"
            );
            continue;
        }

        if (shipMove.FromPlanetId_ == shipMove.ToPlanetId_) {
            if (alreadyUpgraded_.find(shipMove.FromPlanetId_) != alreadyUpgraded_.end()) {
                GameLogger_.LogInvalidMove(
                    playerId,
                    shipMove,
                    "planet " + std::to_string(shipMove.FromPlanetId_) + " already upgraded"
                );
                continue;
            }

            if (fromPlanet.Level_ == 0) {
                if (shipMove.Count_ >= Config_.ShipsToCapturePlanet_) {
                    fromPlanet.ShipCount_ -= Config_.ShipsToCapturePlanet_;
                    fromPlanet.Level_ = 1;
                    fromPlanet.Armor_ = Config_.PerLevelPlanetArmor_.at(fromPlanet.Level_);
                    alreadyUpgraded_.insert(shipMove.FromPlanetId_);
                    continue;
                }
            } else {
                int ArmorToRestore = Config_.PerLevelPlanetArmor_.at(fromPlanet.Level_) - fromPlanet.Armor_;

                if (fromPlanet.Level_ < Config_.ShipsToUpgradePlanet_.size()) {
                    int ShipsToUpgrade = Config_.ShipsToUpgradePlanet_.at(fromPlanet.Level_) + ArmorToRestore;

                    if (shipMove.Count_ >= ShipsToUpgrade) {
                        fromPlanet.ShipCount_ -= ShipsToUpgrade;
                        ++fromPlanet.Level_;
                        fromPlanet.Armor_ = Config_.PerLevelPlanetArmor_.at(fromPlanet.Level_);
                        alreadyUpgraded_.insert(shipMove.FromPlanetId_);
                        continue;
                    }
                }

                fromPlanet.ShipCount_ -= std::min(ArmorToRestore, shipMove.Count_);
                fromPlanet.Armor_ += std::min(ArmorToRestore, shipMove.Count_);
            }
        } else {
            if (shipGroupsInSpace >= Config_.MaxShipGroupsInSpace_) {
                GameLogger_.LogInvalidMove(
                    playerId,
                    shipMove,
                    "too many ship groups in space"
                );
                continue;
            }
            ++shipGroupsInSpace;

            fromPlanet.ShipCount_ -= shipMove.Count_;
            LastShipMovesByPlayer_.at(playerId - 1).ShipMoves_.push_back({
                .PlayerId_ = playerId,
                .FromPlanetId_ = shipMove.FromPlanetId_,
                .ToPlanetId_ = shipMove.ToPlanetId_,
                .Count_ = shipMove.Count_,
            });
            GameState_.ShipGropus_.push_back({
                .PlayerId_ = playerId,
                .FromPlanetId_ = shipMove.FromPlanetId_,
                .ToPlanetId_ = shipMove.ToPlanetId_,
                .Count_ = shipMove.Count_,
                .TimeToDestination_ = Config_.GameMap_.Dists_.at(shipMove.FromPlanetId_ - 1).at(shipMove.ToPlanetId_ - 1),
            });

            if (fromPlanet.ShipCount_ == 0 && fromPlanet.Level_ == 0) {
                fromPlanet.PlayerId_ = 0;
            }
        }
    }
}

void TGame::DisqualifyPlayer(int playerId, const std::string& reason) {
    GameLogger_.LogDisqualifyPlayer(playerId, reason);

    Players_.at(playerId - 1).IsDisqualified_ = true;
    GameState_.AlivePlayers_.erase(playerId);
    GameState_.DisqualifiedPlayers_.push_back(playerId);
}

void TGame::MarkPlayerAsDead(int playerId) {
    Players_.at(playerId - 1).IsDead_ = true;
    GameState_.AlivePlayers_.erase(playerId);
    GameState_.DeadPlayers_.push_back(playerId);
}

bool TGame::IsPlayerDead(int playerId) const {
    for (const auto& planetInfo : GameState_.PlanetInfos_) {
        if (planetInfo.PlayerId_ == playerId) {
            assert(planetInfo.Level_ > 0 || planetInfo.ShipCount_ > 0);
            return false;
        }
    }

    if (GetShipGroupsInSpace(playerId) > 0) {
        return false;
    }

    return true;
}

int TGame::GetShipGroupsInSpace(int playerId) const {
    int ret = 0;
    for (const auto& shipMove : GameState_.ShipGropus_) {
        if (shipMove.PlayerId_ == playerId) {
            ++ret;
        }
    }
    return ret;
}

bool TGame::IsValidPlanetId(int planetId) const {
    return 1 <= planetId && planetId <= GameState_.PlanetInfos_.size();
}
