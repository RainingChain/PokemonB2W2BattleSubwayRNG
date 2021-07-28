#include "Global.hpp"
#include "BattleSubwayFilter.hpp"
#include "BattleSubwayState.hpp"
#include "BattleSubwayData.hpp"
#include <algorithm>

#include "json.hpp"
#include <fstream>
using json = nlohmann::json;

void BattleSubwayFilter::init()
{
    std::ifstream read(this->opts.playerPokemonsFile);
    if (!read.is_open())
        return;
    const json& jsonPokemons = json::parse(read, nullptr, false);
    u32 idx = -1;
    for (const auto& playerPokemon : jsonPokemons)
    {
        idx++;
        if (!this->opts.playerPokemonsIdxFilter.empty()
            && std::count(this->opts.playerPokemonsIdxFilter.begin(), this->opts.playerPokemonsIdxFilter.end(), idx) == 0)
            continue;
        std::vector<std::tuple<u32, u8, float>> ratings;
        for (const auto& jsonRatings : playerPokemon["ratings"])
            ratings.emplace_back(jsonRatings[0].get<int>(), jsonRatings[1].get<int>(), jsonRatings[2].get<float>());
        this->playerPokemons.emplace_back(this->playerPokemons.size(),
            playerPokemon["name"].get<std::string>(),
            playerPokemon["speed"].get<u16>(),
            ratings);
    }
}

bool BattleSubwayFilter::doesMultiTeammateRespectFilter(const BattleSubwayTrainer &trainer) const
{
	if (!this->active)
		return true;
	if (!this->opts.subway.isMulti())
		return true;
	bool good = false;
	int i = 0;
	trainer.forEachPokemon([&](const auto& Poke, size_t)
		{
			if (i == 0 and Poke.getId() == 150) // 896 Haxorus, 150 Amoonguss
				good = true;
			i++;
		});
	return good;
}

bool BattleSubwayFilter::doesTrainerRespectFilter(const BattleSubwayTrainer &trainer) const
{
    if (!this->active)
        return true; 
	if (this->opts.subway.isMulti())
		return true;  //filter is based on 2 trainers

	u32 minScore = 2;
	if (this->opts.subway.isDouble())
		minScore = 3;

    return std::any_of(this->playerPokemons.begin(), this->playerPokemons.end(), [&](const BattleSubwayPlayerPokemon &selfPokemon)
    {
        return selfPokemon.getTrainerRating(trainer) >= minScore;
    });
}

std::pair<const BattleSubwayPlayerPokemon*, float> BattleSubwayFilter::getStateRating(const BattleSubwayState &state) const
{
    std::pair<const BattleSubwayPlayerPokemon*, float> best;
    for (const auto &selfPokemon : this->playerPokemons)
        {
            float rating = selfPokemon.getStateRating(state, this->opts.subway);
            if (rating > best.second)
                best = {&selfPokemon, rating};
        }
    return best;
}

float BattleSubwayPlayerPokemon::getStateRating(const BattleSubwayState &state, Subway subway) const
{
float rating = 0;
if (!subway.isMulti())
	{
		state.forEachTrainer([&](const auto& trainer)
			{ rating += this->getTrainerRating(trainer); });
		return rating;
	}

// multi
state.forEachTrainerPair([&](const auto& trainer1, const auto& trainer2)
	{
		float rating1 = this->getTrainerRating(trainer1);
		float rating2 = this->getTrainerRating(trainer2);
		if (rating1 == 2 && rating2 == 2)
			rating += 2.2f;
		else
			rating += std::max(rating1, rating2);
	});

static std::vector<bool> HasBlizzardVector = []()
{
    const auto& Pokes = BattleSubwayData::getAllTrainersPokemons();
    std::vector<bool> HasBlizzardVector(Pokes.size());
    for (const auto& pokemon : Pokes)
        HasBlizzardVector[pokemon.id] = std::find(pokemon.moves.begin(), pokemon.moves.end(), "Blizzard") != pokemon.moves.end();
    return HasBlizzardVector;
}(); // IIFE

bool HasBlizzard = false;
state.forEachTrainerPair([&](const BattleSubwayTrainer& trainer1, const BattleSubwayTrainer& trainer2)
    {
        for (const auto& trainer : { trainer1 , trainer2 })
            trainer.forEachPokemon([&](const BattleSubwayPokemon& pokemon, size_t i)
                {
                    const auto& trainerPokemon = BattleSubwayData::getAllTrainersPokemons()[pokemon.getId()];
                    if (HasBlizzardVector[trainerPokemon.id] && trainerPokemon.speed >= 154) //Garchomp adamant
                        HasBlizzard = true;
                });
    });
//if (HasBlizzard) //NO_PROD
//    return -1000.0f;


bool isTooFast = false;
state.forEachTrainerPair([&](const BattleSubwayTrainer& trainer1, const BattleSubwayTrainer& trainer2)
    {
        bool isTooFast1 = false;
        bool isTooFast2 = false;
        trainer1.forEachPokemon([&](const BattleSubwayPokemon& pokemon, size_t i)
            {
                const auto& trainerPokemon = BattleSubwayData::getAllTrainersPokemons()[pokemon.getId()];
                if (i == 0 && trainerPokemon.speed >= 167) //Volcarona timid
                    isTooFast1 = true;
            });
        trainer2.forEachPokemon([&](const BattleSubwayPokemon& pokemon, size_t i)
            {
                const auto& trainerPokemon = BattleSubwayData::getAllTrainersPokemons()[pokemon.getId()];
                if (i == 0 && trainerPokemon.speed >= 167) //Volcarona timid
                    isTooFast2 = true;
            });
        if (isTooFast1 && isTooFast2) //both too fast can bypass focus sash
            isTooFast = true;
    });
if (isTooFast)
    return -1000.0f;

return rating;
}