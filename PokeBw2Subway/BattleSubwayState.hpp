#ifndef BATTLESUBWAYSTATE_HPP
#define BATTLESUBWAYSTATE_HPP

#include <array>
#include <iostream>

#include "Global.hpp"

class BattleSubwayPlayerPokemon;

class BattleSubwayPokemon {
 public:
  BattleSubwayPokemon() = default;

  BattleSubwayPokemon(u16 id, u8 ability, u16 speed)
      : id(id), ability(ability) {}

  u16 getId() const { return id; }
  u8 getAbility() const { return ability; }
  void setId(u16 id) { this->id = id; }
  void setAbility(u8 ability) { this->ability = ability; }
  u16 getSpeed() const { return id; }

 private:
  u16 id = idNULL;
  u8 ability = 0;
};

class BattleSubwayTrainer {
 public:
  BattleSubwayTrainer() = default;

  u32 getTrainerId() const { return trainerId; }
  void setTrainerId(u32 trainerId) { this->trainerId = trainerId; }
  std::array<BattleSubwayPokemon, 4>& getPokemonsForInitialization() {
    return pokemons;
  }
  template <typename T>
  void forEachPokemon(const T& Func) const {
    for (size_t i = 0; i < pokemons.size(); i++) {
      const auto& pokemon = pokemons[i];
      if (pokemon.getId() == idNULL) break;
      Func(pokemon, i);
    }
  }

 private:
  std::array<BattleSubwayPokemon, 4> pokemons;
  u32 trainerId = idNULL;
};

class BattleSubwayState {
 public:
  BattleSubwayState() = default;

  BattleSubwayState(u64 seed) : seed(seed) {}

  u64 getSeed() const { return seed; }
  std::array<BattleSubwayTrainer, 14>& getTrainersForInitialization() {
    return trainers;
  }
  template <typename T>
  void forEachTrainer(const T& Func) const {
    for (const auto& trainer : this->trainers) {
      if (trainer.getTrainerId() == idNULL) break;
      Func(trainer);
    }
  }
  template <typename T>
  void forEachTrainerPair(const T& Func) const {
    for (size_t i = 0; i < this->trainers.size(); i += 2) {
      if (this->trainers[i].getTrainerId() == idNULL ||
          this->trainers[i + 1].getTrainerId() == idNULL)
        break;
      Func(this->trainers[i], this->trainers[i + 1]);
    }
  }
  void print(std::ostream& os,
             const BattleSubwayPlayerPokemon* selfPokemon) const;

  const BattleSubwayTrainer& getMultiTeammate() const { return multiTeammate; }
  BattleSubwayTrainer& getMultiTeammate() { return multiTeammate; }

 private:
  std::array<BattleSubwayTrainer, 14> trainers;
  u64 seed = 0;
  BattleSubwayTrainer multiTeammate;
};

#endif  // BATTLESUBWAYSTATE_HPP
