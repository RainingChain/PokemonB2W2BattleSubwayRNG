#ifndef BATTLESUBWAYDATA_HPP
#define BATTLESUBWAYDATA_HPP

#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Global.hpp"
#include "json.hpp"

using json = nlohmann::json;

struct trainerPokemon_t {
  u32 id;
  std::string name;
  std::string item;
  u16 speed;
  std::vector<std::string> moves;
};

static std::map<std::pair<u32, u32>, std::string> PokeDescMap = []() {
  std::map<std::pair<u32, u32>, std::string> map;
  std::ifstream read("./b2w2_pokemons.json");
  if (!read.is_open()) return map;
  const json& trainerPokemons = json::parse(read, nullptr, false);
  for (const json& poke : trainerPokemons) {
    for (u8 abIdx = 0; abIdx < 2; abIdx++) {
      u32 id = poke["id"].get<u32>();
      std::string& Str = map[{id, abIdx}];
      Str += "Id:" + std::to_string(id) +
             ", Name:" + poke["name"].get<std::string>() +
             ", Item:" + poke["item"].get<std::string>() + ", Ability:";
      std::string ab0 = poke["ability"][0].get<std::string>();
      std::string abX = poke["ability"][abIdx].get<std::string>();
      Str += (abX == "") ? ab0 : abX;

      Str += ", Moves:";
      for (const auto& move : poke["moves"])
        Str += move.get<std::string>() + ",";
    }
  }
  return map;
}();

static std::map<u32, std::string> PokeShortDescMap = []() {
  std::map<u32, std::string> map;
  std::ifstream read("./b2w2_pokemons.json");
  if (!read.is_open()) return map;
  const json& trainerPokemons = json::parse(read, nullptr, false);
  for (const json& poke : trainerPokemons) {
    u32 id = poke["id"].get<u32>();
    std::string& Str = map[id];
    Str += poke["name"].get<std::string>() + " (" + std::to_string(id) + ")";
  }
  return map;
}();

static std::map<u32, std::string> TrainerShortDescMap = []() {
  std::map<u32, std::string> map;
  std::ifstream read("./b2w2_trainers.json");
  if (!read.is_open()) return map;
  const json& trainers = json::parse(read, nullptr, false);
  for (const json& trainer : trainers) {
    u32 id = trainer["id"].get<u32>();
    std::string msg = trainer["startMsg"].get<std::string>();
    std::string msgShort =
        msg.size() <= 20 ? msg
                         : trainer["startMsg"].get<std::string>().substr(0, 20);

    std::string& Str = map[id];
    Str += msgShort + " (" + std::to_string(id) + ")";
  }
  return map;
}();

static std::map<u32, std::string> TrainerDescMap = []() {
  std::map<u32, std::string> map;
  std::ifstream read("./b2w2_trainers.json");
  if (!read.is_open()) return map;
  const json& trainers = json::parse(read, nullptr, false);
  for (const json& trainer : trainers) {
    u32 id = trainer["id"].get<u32>();
    std::string& Str = map[id];
    Str += "TrainerId: " + std::to_string(id) +
           ", Name:" + trainer["name"].get<std::string>() + ", StartMsg:";
    std::string msg = trainer["startMsg"].get<std::string>();
    Str += msg.size() <= 30
               ? msg
               : (trainer["startMsg"].get<std::string>().substr(0, 30) + "...");
  }
  return map;
}();

class BattleSubwayData {
 public:
  static const char* pokeShortDesc(u32 id) {
    return PokeShortDescMap[id].c_str();
  }
  static const char* pokeDesc(u32 id, u32 abIdx) {
    return PokeDescMap[{id, abIdx}].c_str();
  }
  static const char* trainerDesc(u32 id, bool shortDesc) {
    return shortDesc ? TrainerShortDescMap[id].c_str()
                     : TrainerDescMap[id].c_str();
  }

  static const std::vector<trainerPokemon_t>& getAllTrainersPokemons() {
    static const auto allTrainersPokemons =
        ([]() -> std::vector<trainerPokemon_t> {
          std::vector<trainerPokemon_t> allTrainerPokemons;
          std::ifstream read("./b2w2_pokemons.json");
          if (!read.is_open()) {
            allTrainerPokemons.resize(987 + 1);
            return allTrainerPokemons;
          }
          const json& trainerPokemons = json::parse(read, nullptr, false);

          for (const auto& trainerPokemon1 : trainerPokemons) {
            trainerPokemon_t& tp = allTrainerPokemons.emplace_back();
            tp.id = trainerPokemon1["id"].get<u32>();
            tp.item = trainerPokemon1["item"].get<std::string>();
            tp.name = trainerPokemon1["name"].get<std::string>();
            tp.speed = trainerPokemon1["speed"].get<u16>();
            tp.moves = trainerPokemon1["moves"].get<std::vector<std::string>>();
          }
          return allTrainerPokemons;
        })();
    return allTrainersPokemons;
  }
};

#endif  // BATTLESUBWAYDATA_HPP
