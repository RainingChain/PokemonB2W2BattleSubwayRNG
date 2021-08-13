#pragma once

#include <array>
#include <vector>

#include "Global.hpp"
#include "SubwayType.hpp"

class BattleSubwayPokemon;
class BattleSubwayTrainer;

class BattleSubwayPlayerPokemon {
 public:
  BattleSubwayPlayerPokemon(
      u32 id, std::string name, std::string item,
      std::string description, u16 speed,
      const std::vector<std::tuple<u32, u8, float>>& ratings)
      : id(id), item(item), description(description), speed(speed), name(name) {
    for (const auto& rating : ratings)
      this->ratingsByPokemonAndAbility[std::get<0>(rating)]
                                      [std::get<1>(rating)] =
          std::get<2>(rating);
  }
  float getPokemonRating(const BattleSubwayPokemon& trainerPokemon) const;
  float getTrainerRating(const BattleSubwayTrainer& trainer) const;

  std::string name;
  std::array<std::array<float, 2>, 987 + 1> ratingsByPokemonAndAbility = {};
  u32 id;
  u16 speed;
  std::string item;
  std::string description;
};