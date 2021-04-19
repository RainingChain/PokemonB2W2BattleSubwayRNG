#ifndef BATTLESUBWAYSTATE_HPP
#define BATTLESUBWAYSTATE_HPP

#include "Global.hpp"
#include <array>
#include <iostream>

class BattleSubwayPlayerPokemon;

class BattleSubwayPokemon
{
public:
    BattleSubwayPokemon() = default;
    
    BattleSubwayPokemon(u16 id, u8 ability) : id(id), ability(ability)
    {
    }

    u16 getId() const
    {
        return id;
    }
    u8 getAbility() const
    {
        return ability;
    }
    void setId(u16 id)
    {
        this->id = id;
    }
    void setAbility(u8 ability)
    {
        this->ability = ability;
    }
private:
    u16 id;
    u8 ability;
};

class BattleSubwayTrainer
{
public:
    BattleSubwayTrainer() = default;
    
    u32 getTrainerId() const
    {
        return trainerId;
    }
    void setTrainerId(u32 trainerId)
    {
        this->trainerId = trainerId;
    }
    std::array<BattleSubwayPokemon, 3> &getPokemons()
    {
        return pokemons;
    }
    const std::array<BattleSubwayPokemon, 3> &getPokemons() const
    {
        return pokemons;
    }
private:
    std::array<BattleSubwayPokemon, 3> pokemons;
    u32 trainerId;
};


class BattleSubwayState
{
public:
    BattleSubwayState() = default;

    BattleSubwayState(u64 seed) : seed(seed)
    {
    }

    u64 getSeed() const
    {
        return seed;
    }
    std::array<BattleSubwayTrainer, 7> &getTrainers()
    {
        return trainers;
    }
    const std::array<BattleSubwayTrainer, 7> &getTrainers() const
    {
        return trainers;
    }
    void print(std::ostream& os, const BattleSubwayPlayerPokemon* selfPokemon) const;

private:
    std::array<BattleSubwayTrainer, 7> trainers;
    u64 seed;
};

#endif // BATTLESUBWAYSTATE_HPP
