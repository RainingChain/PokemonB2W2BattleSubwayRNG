#include "BattleSubwayFilter.hpp"

#include <algorithm>
#include <fstream>

#include "BattleSubwayData.hpp"
#include "BattleSubwayState.hpp"
#include "Global.hpp"

bool BattleSubwayFilter::doesMultiTeammateRespectFilter(
    const BattleSubwayTrainer& trainer) const {
  if (!this->active) return true;
  if (!this->opts.subway.isMulti()) return true;
  bool good = false;
  int i = 0;
  trainer.forEachPokemon([&](const auto& Poke, size_t) {
    if (i == 0 and Poke.getId() == 546)  // 546,896 Haxorus, 150 Amoonguss
      good = true;
    i++;
  });
  return good;
}

bool BattleSubwayFilter::doesTrainerRespectFilter(
    const BattleSubwayTrainer& trainer) const {
  if (!this->active) return true;
  if (this->opts.subway.isMulti())
    return true;  // filter is based on 2
                  // trainers

  u32 minScore = 2;
  if (this->opts.subway.isDouble()) minScore = 3;

  return std::any_of(this->opts.playerPokemons.begin(),
                     this->opts.playerPokemons.end(),
                     [&](const BattleSubwayPlayerPokemon& selfPokemon) {
                       return selfPokemon.getTrainerRating(trainer) >= minScore;
                     });
}

std::pair<const BattleSubwayPlayerPokemon*, float>
BattleSubwayFilter::getStateRating(const BattleSubwayState& state) const {
  std::pair<const BattleSubwayPlayerPokemon*, float> best;
  for (const auto& selfPokemon : this->opts.playerPokemons) {
    float rating = this->getStateRating(state, selfPokemon);
    if (rating > best.second) best = {&selfPokemon, rating};
  }
  return best;
}

float BattleSubwayFilter::getStateRating(
    const BattleSubwayState& state,
    const BattleSubwayPlayerPokemon& selfPokemon) const {
  //single, double
  float rating = 0;
  if (!this->opts.subway.isMulti()) {
    state.forEachTrainer([&](const auto& trainer) {
      rating += selfPokemon.getTrainerRating(trainer);
    });
    return rating;
  }

  // multi
  state.forEachTrainerPair([&](const auto& trainer1, const auto& trainer2) {
    float rating1 = selfPokemon.getTrainerRating(trainer1);
    float rating2 = selfPokemon.getTrainerRating(trainer2);
    if (rating1 == 2 && rating2 == 2)
      rating += 2.2f;
    else
      rating += std::max(rating1, rating2);
  });

  /*
  static std::vector<bool> HasBlizzardVector = []() {
    const auto& Pokes = BattleSubwayData::getAllTrainersPokemons();
    std::vector<bool> HasBlizzardVector(Pokes.size());
    for (const auto& pokemon : Pokes)
      HasBlizzardVector[pokemon.id] =
          std::find(pokemon.moves.begin(), pokemon.moves.end(), "Blizzard") !=
          pokemon.moves.end();
    return HasBlizzardVector;
  }();  // IIFE

  bool HasBlizzard = false;
  state.forEachTrainerPair([&](const BattleSubwayTrainer& trainer1,
                               const BattleSubwayTrainer& trainer2) {
    for (const auto& trainer : {trainer1, trainer2})
      trainer.forEachPokemon([&](const BattleSubwayPokemon& pokemon, size_t i) {
        const auto& trainerPokemon =
            BattleSubwayData::getAllTrainersPokemons()[pokemon.getId()];
        if (HasBlizzardVector[trainerPokemon.id] &&
            trainerPokemon.speed >= 154)  // Garchomp adamant
          HasBlizzard = true;
      });
  });
  if (HasBlizzard)
      return -1000.0f;
  */

  bool seriesHasTooFast = false;
  state.forEachTrainerPair([&](const BattleSubwayTrainer& trainer1,
                               const BattleSubwayTrainer& trainer2) {
    int pairIsTooFastCount = 0;
    for (const auto* trainer : {&trainer1, &trainer2}) {
      trainer->forEachPokemon(
        [&](const BattleSubwayPokemon& pokemon, size_t i) {
          const auto& trainerPokemon = BattleSubwayData::getAllTrainersPokemons()[pokemon.getId()];
          if (i == 0 && trainerPokemon.speed >= selfPokemon.speed)  // Volcarona timid
            pairIsTooFastCount++;
        });
    }
    if (pairIsTooFastCount >= 2)  // both too fast can bypass focus sash
      seriesHasTooFast = true;
  });
  if (seriesHasTooFast) 
    return -1000.0f;

  return rating;
}