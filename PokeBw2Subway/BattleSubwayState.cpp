#include "BattleSubwayState.hpp"

#include <fstream>
#include <sstream>
#include <string>

#include "BattleSubwayData.hpp"
#include "BattleSubwayFilter.hpp"

void BattleSubwayState::print(
    std::ostream& os, const BattleSubwayPlayerPokemon* selfPokemon) const {
  auto printTrainerShort = [&](const auto& trainer) {
    os << "(t" << (u32)trainer.getTrainerId() << ",";
    trainer.forEachPokemon([&](const auto& trainerPokemon, size_t) {
      os << "p" << trainerPokemon.getId() << ",a"
         << (u32)trainerPokemon.getAbility();
      if (selfPokemon != nullptr)
        os << ",r" << selfPokemon->getPokemonRating(trainerPokemon);
      os << "|";
    });
    os << ") ";
  };

  auto printTrainerLong = [&](const auto& trainer) {
    os << BattleSubwayData::trainerDesc(trainer.getTrainerId(), false) << "\n";
    trainer.forEachPokemon([&](const auto& trainerPokemon, size_t) {
      os << "  "
         << BattleSubwayData::pokeDesc(trainerPokemon.getId(),
                                       trainerPokemon.getAbility());
      if (selfPokemon != nullptr)
        os << " Rating:" << selfPokemon->getPokemonRating(trainerPokemon);
      os << "\n";
    });
    os << "\n";
  };

  this->forEachTrainer(printTrainerShort);
  os << "\n\n";

  if (this->multiTeammate.getTrainerId() != idNULL) {
    os << "Teammate: ";
    printTrainerLong(this->multiTeammate);
  }

  this->forEachTrainer(printTrainerLong);
}