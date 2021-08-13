#pragma once

#include <charconv>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

#include "DSType.hpp"
#include "Game.hpp"
#include "Global.hpp"
#include "Keypresses.hpp"
#include "Language.hpp"
#include "Profile5.hpp"
#include "ProfileLoader.hpp"
#include "SHA1.hpp"
#include "SubwayType.hpp"
#include "BattleSubwayPlayerPokemon.hpp"
#include "Utilities.hpp"
#include "pch.h"

class BattleSubwayPlayerTeam {
 public:
  BattleSubwayPlayerTeam() = default;

  std::string pokemon1 = "Volcarona";  // "Chandelure";
  std::string pokemon2 = "Garchomp";   // "Volcarona";
  std::string item1 = "Focus Sash";    //"Life Orb";
  std::string item2 = "Choice Band";   //"Choice Specs";
  int multiTrainer = 300;              // 300 Attack, 301 Defence, 302 Balanced

  std::string toString() const {
    std::string str;
    str += "Trainer:" + std::to_string(this->multiTrainer) + ", Poke1:" + this->pokemon1 + "@" +
           this->item1 + ", Poke2:" + this->pokemon2 + "@" + this->item2;
    return str;
  }
};

class Options {
 public:
  Profile5 profile;
  DateTime dateTime = DateTime(2009, 1, 1, 0, 0, 0);
  u32 wins = 28;
  float filterMinRating = 0;
  bool printRngFramesInfo = false;
  std::string playerPokemonsFile = "./b2w2_playerPokemons.json";
  std::vector<u32> playerPokemonsIdxFilter;
  u32 macToCheckCount = 0;
  Subway subway = SubwayType::superSingle;
  /** After initial advances */
  std::optional<u64> pidRng;
  u32 multiTeammateUnknownFrameAdvance = 12;
  BattleSubwayPlayerTeam playerTeamForMulti = {};
  std::vector<BattleSubwayPlayerPokemon> playerPokemons;

  DateTime strToDateTime(std::string dt) const;
  std::vector<u32> strToVector(std::string str) const;
  void print(std::ostream& os) const;
  Options(int argc, char* argv[]);
  void initPlayerPokemons();

  const BattleSubwayPlayerPokemon* getFirstPlayerPokemon() const {
    return this->playerPokemons.empty() ? nullptr : &this->playerPokemons[0];
  }
};
