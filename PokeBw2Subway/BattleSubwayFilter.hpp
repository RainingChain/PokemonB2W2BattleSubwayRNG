#pragma once

#include <vector>

#include "BattleSubwayPlayerPokemon.hpp"
#include "BattleSubwayState.hpp"
#include "Global.hpp"
#include "Options.hpp"
#include "SubwayType.hpp"

class BattleSubwayState;

class BattleSubwayFilter {
 public:
  BattleSubwayFilter() = default;
  BattleSubwayFilter(const Options opts, bool active)
      : opts(opts), active(active) {
    if (!active) return;
    init();
  }
  void init();
  bool doesTrainerRespectFilter(const BattleSubwayTrainer& trainer) const;
  bool doesMultiTeammateRespectFilter(const BattleSubwayTrainer& trainer) const;
  std::pair<const BattleSubwayPlayerPokemon*, float> getStateRating(
      const BattleSubwayState& state) const;
  float getStateRating(const BattleSubwayState& state,
                       const BattleSubwayPlayerPokemon& selfPokemon) const;

  const BattleSubwayPlayerPokemon* firstPlayerPokemon() const {
    return playerPokemons.empty() ? nullptr : &playerPokemons[0];
  }

 private:
  const Options opts;
  bool active = true;
  std::vector<BattleSubwayPlayerPokemon> playerPokemons;
};
