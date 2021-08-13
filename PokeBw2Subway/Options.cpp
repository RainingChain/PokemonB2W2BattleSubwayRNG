#pragma once

#include "Options.hpp"
#include <algorithm>
#include <fstream>

#include "Global.hpp"
#include "json.hpp"

using json = nlohmann::json;

Options::Options(int argc, char* argv[]) {
  std::string profileFile = "./profile.json";
  for (int i = 1; i < argc - 1; i += 2) {
    std::string Key(argv[i]);
    std::string Value(argv[i + 1]);
    if (Key == "--profile")
      profileFile = Value;
    else if (Key == "--dateTime")
      this->dateTime = this->strToDateTime(Value);
    else if (Key == "--wins")
      this->wins = (std::stoi(Value) / 7) * 7;  // round to 7
    else if (Key == "--macToCheckCount")
      this->macToCheckCount = std::stoi(Value);
    else if (Key == "--filterMinRating")
      this->filterMinRating = std::stof(Value);
    else if (Key == "--pidRng")
      this->pidRng = std::stoull(Value, nullptr, 16);
    else if (Key == "--multiTeammateUnknownFrameAdvance")
      this->multiTeammateUnknownFrameAdvance = std::stoi(Value);
    else if (Key == "--multiTrainer") {
      if (Value == "attack") this->playerTeamForMulti.multiTrainer = 300;
      else if (Value == "defense") this->playerTeamForMulti.multiTrainer = 301;
      else if (Value == "balanced") this->playerTeamForMulti.multiTrainer = 302;
      else
        std::cout << "Invalid --multiTrainer value: " << Value << "\n";
    } else if (Key == "--playerPokemonsFile")
      this->playerPokemonsFile = Value;
    else if (Key == "--playerPokemonsIdxFilter")
      this->playerPokemonsIdxFilter = strToVector(Value);
    else if (Key == "--printRngFramesInfo")
      this->printRngFramesInfo = Value != "false";
    else if (Key == "--subwayType")
      this->subway = Value;
    else
      std::cout << "Invalid arguments: " << Key << "\n";
  }
  auto profile = ProfileLoader5::getProfile(profileFile);
  if (!profile.has_value())
    std::cout << "Invalid profile.";
  else
    this->profile = *profile;

  this->initPlayerPokemons();
}

DateTime Options::strToDateTime(std::string dt) const {
  std::vector<u32> Ints;
  std::string temp;
  std::istringstream f(dt);
  while (std::getline(f, temp, '-')) Ints.push_back(std::stoi(temp));
  while (Ints.size() < 6) Ints.push_back(0);
  return DateTime(Ints[0], Ints[1], Ints[2], Ints[3], Ints[4], Ints[5]);
}
std::vector<u32> Options::strToVector(std::string str) const {
  std::string str2 = "[" + str + "]";
  nlohmann::json j = nlohmann::json::parse(str2);
  if (j.is_discarded()) {
    std::cout << "Invalid --playerPokemonsIdxFilter\n";
    return {};
  }
  return j.get<std::vector<u32>>();
}
void Options::print(std::ostream& os) const {
  os << "\nInput:"
     << "\n";
  os << "  profile:" << ProfileLoader5::getJson(this->profile).dump() << "\n";
  os << "  wins:" << this->wins << "\n";
  os << "  subwayType:" << this->subway.toStr() << "\n";

  if (this->subway.isMulti()) {
    os << "  playerTeam:" << this->playerTeamForMulti.toString() << "\n";
    os << "  multiTeammateUnknownFrameAdvance:" << this->multiTeammateUnknownFrameAdvance << "\n";
  }
  if (this->filterMinRating != 0.0) {
    os << "  filterMinRating:" << this->filterMinRating << "\n";
    os << "  playerPokemonsFile:" << this->playerPokemonsFile << "\n";
    if (!this->playerPokemonsIdxFilter.empty()) {
      os << "  playerPokemonsIdxFilter:[";
      for (const u32 Idx : this->playerPokemonsIdxFilter) 
        os << Idx << ",";
      os << "]\n";
    }
  } else if (this->pidRng.has_value())
    os << "  pidRng: 0x" << std::hex << *this->pidRng << std::dec << "\n";
  else
    os << "  dateTime:" << this->dateTime.toString() << "\n";
  os << "\n";
}

void Options::initPlayerPokemons() {
  std::ifstream read(this->playerPokemonsFile);
  if (!read.is_open())
    return;
  const json& jsonPokemons = json::parse(read, nullptr, false);
  u32 idx = -1;
  for (const auto& playerPokemon : jsonPokemons) {
    idx++;
    if (!this->playerPokemonsIdxFilter.empty() &&
        std::count(this->playerPokemonsIdxFilter.begin(),
                   this->playerPokemonsIdxFilter.end(), idx) == 0)
      continue;
    std::vector<std::tuple<u32, u8, float>> ratings;
    for (const auto& jsonRatings : playerPokemon["ratings"])
      ratings.emplace_back(jsonRatings[0].get<int>(), jsonRatings[1].get<int>(),
                           jsonRatings[2].get<float>());
    this->playerPokemons.emplace_back(
        this->playerPokemons.size(), playerPokemon["name"].get<std::string>(),
        playerPokemon["item"].get<std::string>(),
        playerPokemon["description"].get<std::string>(),
        playerPokemon["speed"].get<u16>(), ratings);
  }

  if (this->playerPokemons.size() >= 2) {
    this->playerTeamForMulti.item1 = this->playerPokemons[0].item;
    this->playerTeamForMulti.pokemon1 = this->playerPokemons[0].name;
    this->playerTeamForMulti.item2 = this->playerPokemons[1].item;
    this->playerTeamForMulti.pokemon2 = this->playerPokemons[1].name;
  }

}