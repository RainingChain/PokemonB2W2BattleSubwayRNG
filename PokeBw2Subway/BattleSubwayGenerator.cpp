#include "Global.hpp"
#include "BattleSubwayGenerator.hpp"
#include "LCRNG64.hpp"
#include "Utilities.hpp"
#include "SHA1.hpp"
#include "BattleSubwayState.hpp"

#include "json.hpp"
#include <fstream>
#include <sstream>
#include <optional>
#include <iomanip>

using json = nlohmann::json;

void BattleSubwayGenerator::advance(BWRNG& rng, u32 adv, const char* Info) const
{
    rng.advance(adv);
    if (this->trace)
        std::cout << "+" << adv << " : " << "0x" << std::setfill('0') << std::setw(16) << std::hex << rng.getSeed() << std::resetiosflags(std::ios_base::basefield) << " : " << Info << "\n";
}

bool areCompatiblePokemons(u32 pokemon1, u32 pokemon2)
{
static const auto compatibleMatrix = []() -> std::array<std::array<bool, 987 + 1>, 987 + 1>
    {
    std::array<std::array<bool, 987 + 1>, 987 + 1> matrix = {};
    std::ifstream read("./b2w2_pokemons.json");
    if (!read.is_open())
        return matrix;
    const json &trainerPokemons = json::parse(read, nullptr, false);

    struct trainerPokemon_t { 
        u32 id; 
        std::string name;
        std::string item;
    };
    std::vector<trainerPokemon_t> allTrainerPokemons;
    for (const auto& trainerPokemon1 : trainerPokemons)
    {
        trainerPokemon_t& tp = allTrainerPokemons.emplace_back();
        tp.id = trainerPokemon1["id"].get<u32>();
        tp.name = trainerPokemon1["item"].get<std::string>();
        tp.item = trainerPokemon1["name"].get<std::string>();
    }

    for (const auto &tp1 : allTrainerPokemons)
    {
        for (const auto &tp2 : allTrainerPokemons)
        {
            matrix[tp1.id][tp2.id] = tp1.item != tp2.item && tp1.name != tp2.name;
        }
    }
    return matrix;
    }();
return compatibleMatrix[pokemon1][pokemon2];
}

const std::vector<u32> &getPossiblePokemonsForTrainer(u32 trainerId)
{
static const auto pokemonsByTrainerMatrix = []() -> std::array<std::vector<u32>, 313 + 1>
    {
    std::array<std::vector<u32>, 313 + 1> matrix;
    std::ifstream read("./b2w2_trainers.json");
    if (!read.is_open())
        return matrix;
    const json &trainers = json::parse(read, nullptr, false);
    for (const auto &trainer : trainers)
    {
        const u32 id = trainer["id"].get<u32>();
        matrix[id] = trainer["pokemons"].get<std::vector<u32>>();
    }
    return matrix;
    }();
return pokemonsByTrainerMatrix[trainerId];
}

std::vector<u32> createList(u32 start, u32 end)
{
    std::vector<u32> vector;
    vector.reserve(end - start + 1);
    for (u32 i = start; i <= end; ++i)
        vector.push_back(i);
    return vector;
}

const std::vector<u32>& getPossibleTrainersByWins(u32 wins)
{
    if (wins == 48)
        {
        static const auto& list = createList(307, 307);
        return list;
        }
    if(wins < 7 - 1)
        {
        static const auto &list = createList(110, 159);
        return list;
        }
    if(wins < 14 - 1)
        {
        static const auto &list = createList(140, 179);
        return list;
        }
    if(wins < 21 - 1)
        {
        static const auto &list = createList(160, 199);
        return list;
        }
    if(wins < 28 - 1)
        {
        static const auto &list = createList(180, 219);
        return list;
        }
    if (wins >= 28 - 1)
        {
        static const auto& list = createList(200, 299);
        return list;
        }
    static std::vector<u32> Error;
    return Error;
}

template <typename Container>
const auto &getElement(u64 seed, const Container& Vector){
    u32 v1 = seed >> 32;
    u32 v2 = ((v1 >> 16) + v1) >> 16;
    u32 v3 = v2 % Vector.size();
    return Vector[v3];
}

void BattleSubwayGenerator::addPokemonsToTrainer(BWRNG& rng, BattleSubwayTrainer &trainer) const
{
    const auto &possiblePokemons = getPossiblePokemonsForTrainer(trainer.getTrainerId());
    auto &trainerPokemons = trainer.getPokemons();
    for (u32 i = 0; i < 3;)
    {
        advance(rng, 1, "selectPokemonId");
        u32 pokeIdToAdd = getElement(rng.getSeed(), possiblePokemons);
        
        if (i == 1 && !areCompatiblePokemons(trainerPokemons[0].getId(), pokeIdToAdd))
            continue;
        if (i == 2 && (!areCompatiblePokemons(trainerPokemons[0].getId(), pokeIdToAdd) || !areCompatiblePokemons(trainerPokemons[1].getId(), pokeIdToAdd)))
            continue;
        trainerPokemons[i].setId(pokeIdToAdd);
        i++;
    }
    advance(rng, 2, "unknown advances");
    
    for (u32 i = 0; i < 3; i++)
    {
        advance(rng, 1, "selectPokemonAbilityAndGender");
        u32 ability = (rng.getSeed() >> 32) % 2;
        trainerPokemons[i].setAbility(ability);
    }
}

std::optional<BattleSubwayState> BattleSubwayGenerator::generate(u64 seed) const
{
    BWRNG rng(seed);
    advance(rng, 0, "save loaded");
    BattleSubwayState state(rng.getSeed());
    advance(rng, 1, "saving to start subway");

    // determine all 7 trainer ids
    std::array<BattleSubwayTrainer, 7> &trainers = state.getTrainers();
    for (u32 i = 0; i < 7;)
    {
        const auto& possibleTrainers = getPossibleTrainersByWins(this->wins + i);
        u32 adv = possibleTrainers.size() == 1 ? 0 : 1;
        advance(rng, adv, "selectTrainerId");
        const auto& trainerId = getElement(rng.getSeed(), possibleTrainers);
        auto end = trainers.begin() + i;
        auto it = std::find_if(trainers.begin(), end, [&](const auto &trainer)
            { return trainer.getTrainerId() == trainerId; });
        if (it != end)
            continue; // We can't have the same trainer twice in the series.
        trainers[i].setTrainerId(trainerId);
        i++;
    }

    //determine the pokemons
    for (auto &trainer : trainers)
    {
        addPokemonsToTrainer(rng, trainer);
        if (this->filter != nullptr && !this->filter->doesTrainerRespectFilter(trainer))
            return std::nullopt;
        advance(rng, 6, "unknown advances when starting battle");
    }
    return state;
}
