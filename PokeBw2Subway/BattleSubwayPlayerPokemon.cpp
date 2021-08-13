#pragma once
#include "BattleSubwayPlayerPokemon.hpp";

#include "BattleSubwayState.hpp"

float BattleSubwayPlayerPokemon::getPokemonRating(
    const BattleSubwayPokemon& trainerPokemon) const {
  return this->ratingsByPokemonAndAbility[trainerPokemon.getId()]
                                         [trainerPokemon.getAbility()];
}
float BattleSubwayPlayerPokemon::getTrainerRating(
    const BattleSubwayTrainer& trainer) const {
  float rating = 0;
  trainer.forEachPokemon([&](const auto& trainerPokemon, size_t) {
    rating += this->getPokemonRating(trainerPokemon);
  });
  return rating;
}