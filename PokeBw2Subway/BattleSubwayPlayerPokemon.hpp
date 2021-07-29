#pragma once

#include <vector>

#include "BattleSubwayState.hpp"
#include "Global.hpp"
#include "Options.hpp"
#include "SubwayType.hpp"

class BattleSubwayPlayerPokemon {
 public:
  BattleSubwayPlayerPokemon(
      u32 id, std::string name, u16 speed,
      const std::vector<std::tuple<u32, u8, float>>& ratings)
      : id(id), speed(speed), name(name) {
    for (const auto& rating : ratings)
      this->ratingsByPokemonAndAbility[std::get<0>(rating)]
                                      [std::get<1>(rating)] =
          std::get<2>(rating);
  }
  float getPokemonRating(const BattleSubwayPokemon& trainerPokemon) const {
    return this->ratingsByPokemonAndAbility[trainerPokemon.getId()]
                                           [trainerPokemon.getAbility()];
  }
  float getTrainerRating(const BattleSubwayTrainer& trainer) const {
    float rating = 0;
    trainer.forEachPokemon([&](const auto& trainerPokemon, size_t) {
      rating += this->getPokemonRating(trainerPokemon);
    });
    return rating;
  }

  std::string name;
  std::array<std::array<float, 2>, 987 + 1> ratingsByPokemonAndAbility = {};
  u32 id;
  u16 speed;
};