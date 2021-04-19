#ifndef BATTLESUBWAYFILTER_HPP
#define BATTLESUBWAYFILTER_HPP

#include "Global.hpp"
#include "BattleSubwayState.hpp"
#include <vector>

class BattleSubwayState;


class BattleSubwayPlayerPokemon 
{
public:
    BattleSubwayPlayerPokemon(
            u32 id, 
            std::string name, 
            const std::vector<std::tuple<u32, u8, float>> &ratings) :
        id(id),
        name(name)
    {
        for (const auto& rating : ratings)
            this->ratingsByPokemonAndAbility[std::get<0>(rating)][std::get<1>(rating)] = std::get<2>(rating);
    }
    float getPokemonRating(const BattleSubwayPokemon &trainerPokemon) const
    {
        return this->ratingsByPokemonAndAbility[trainerPokemon.getId()][trainerPokemon.getAbility()];
    }
    float getTrainerRating(const BattleSubwayTrainer &trainer) const
    {
        float rating = 0;
        for (const auto &trainerPokemon : trainer.getPokemons())
        {
            rating += this->getPokemonRating(trainerPokemon);
        }
        return rating;
    }
    float getStateRating(const BattleSubwayState &state) const
    {
        float rating = 0;
        for (const auto &trainer : state.getTrainers())
            rating += this->getTrainerRating(trainer);
        return rating;
    }

    
    u32 id;
    std::string name;
    std::array<std::array<float, 2>, 987 + 1> ratingsByPokemonAndAbility = {};
};

class BattleSubwayFilter
{
public:
    BattleSubwayFilter() = default;
    BattleSubwayFilter(bool active, const std::string& playerPokemonsFile);
    bool doesTrainerRespectFilter(const BattleSubwayTrainer &trainer) const;
    std::pair<const BattleSubwayPlayerPokemon*, float> getStateRating(const BattleSubwayState &state) const;

private:
    bool active = true;
    std::vector<BattleSubwayPlayerPokemon> playerPokemons;
};



#endif // BATTLESUBWAYFILTER_HPP
