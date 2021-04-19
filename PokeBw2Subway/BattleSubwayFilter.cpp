#include "Global.hpp"
#include "BattleSubwayFilter.hpp"
#include "BattleSubwayState.hpp"
#include <algorithm>

#include "json.hpp"
#include <fstream>
using json = nlohmann::json;

BattleSubwayFilter::BattleSubwayFilter(bool active, const std::string& playerPokemonsFile)
    : active(active)
{
    if (!active)
        return;
    std::ifstream read(playerPokemonsFile);
    if (!read.is_open())
        return;
    const json& jsonPokemons = json::parse(read, nullptr, false);
    for (const auto& playerPokemon : jsonPokemons)
    {
        std::vector<std::tuple<u32, u8, float>> ratings;
        for (const auto& jsonRatings : playerPokemon["ratings"])
            ratings.emplace_back(jsonRatings[0].get<int>(), jsonRatings[1].get<int>(), jsonRatings[2].get<float>());
        this->playerPokemons.emplace_back(this->playerPokemons.size(), playerPokemon["name"].get<std::string>(), ratings);
    }
}

bool BattleSubwayFilter::doesTrainerRespectFilter(const BattleSubwayTrainer &trainer) const
{
    if (!this->active)
        return true;
    return std::any_of(this->playerPokemons.begin(), this->playerPokemons.end(), [&](const BattleSubwayPlayerPokemon &selfPokemon)
    {
        return selfPokemon.getTrainerRating(trainer) >= 2;
    });
}

std::pair<const BattleSubwayPlayerPokemon*, float> BattleSubwayFilter::getStateRating(const BattleSubwayState &state) const
{
    std::pair<const BattleSubwayPlayerPokemon*, float> best = { nullptr, 0 };
    for (const auto &selfPokemon : this->playerPokemons)
        {
            float rating = selfPokemon.getStateRating(state);
            if (rating > best.second)
                best = {&selfPokemon, rating};
        }
    return best;
}
