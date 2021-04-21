#include "BattleSubwayState.hpp"
#include "BattleSubwayFilter.hpp"
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <string>
#include "json.hpp"
using json = nlohmann::json;

std::map<std::pair<u32,u32>, std::string> PokeDescMap = []()
{
    std::map<std::pair<u32, u32>, std::string> map;
    std::ifstream read("./b2w2_pokemons.json");
    if (!read.is_open())
        return map;
    const json& trainerPokemons = json::parse(read, nullptr, false);
    for (const json& poke : trainerPokemons)
    {
        for (u8 abIdx = 0; abIdx < 2; abIdx++)
        {
            u32 id = poke["id"].get<u32>();
            std::string& Str = map[{id, abIdx}];
            Str += "Id:" + std::to_string(id) + ", Name:" + poke["name"].get<std::string>() + ", Item:" + poke["item"].get<std::string>() + ", Ability:"; 
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

std::map<u32, std::string> TrainerDescMap = []()
{
    std::map<u32, std::string> map;
    std::ifstream read("./b2w2_trainers.json");
    if (!read.is_open())
        return map;
    const json& trainers = json::parse(read, nullptr, false);
    for (const json& trainer : trainers)
    {
        u32 id = trainer["id"].get<u32>();
        std::string& Str = map[id];
        Str += "TrainerId: " + std::to_string(id) + ", Name:" + trainer["name"].get<std::string>() + ", StartMsg:";
        std::string msg = trainer["startMsg"].get<std::string>();
        Str += msg.size() <= 30 ? msg : (trainer["startMsg"].get<std::string>().substr(0, 30) + "...");
    }
    return map;
}();

void BattleSubwayState::print(std::ostream& os, const BattleSubwayPlayerPokemon* selfPokemon) const
{
    for (const auto& trainer : trainers)
    {
        os << "(t" << (u32)trainer.getTrainerId() << ",";
        for (const auto& trainerPokemon : trainer.getPokemons())
        {
            os << "p" << trainerPokemon.getId() << ",a" << (u32)trainerPokemon.getAbility();
            if (selfPokemon != nullptr)
                os << ",r" << selfPokemon->getPokemonRating(trainerPokemon);
            os << "|";
        }
        os << ") ";
    }
    os << "\n\n";

    for (const auto& trainer : trainers)
    {
        os << TrainerDescMap[trainer.getTrainerId()] + "\n";
        for (const auto& trainerPokemon : trainer.getPokemons())
        {
            os << "  " << PokeDescMap[{trainerPokemon.getId(), trainerPokemon.getAbility()}];
            if (selfPokemon != nullptr)
                os << " Rating:" << selfPokemon->getPokemonRating(trainerPokemon);
            os << "\n";
        }
        os << "\n";
    }
}