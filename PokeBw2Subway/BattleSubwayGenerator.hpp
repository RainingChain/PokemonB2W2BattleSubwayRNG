#ifndef BATTLESUBWAYGENERATOR_HPP
#define BATTLESUBWAYGENERATOR_HPP

#include "BattleSubwayFilter.hpp"
#include "BattleSubwayState.hpp"
#include "DateTime.hpp"
#include "Profile5.hpp"
#include <vector>
#include <optional>
#include <atomic>
#include "LCRNG64.hpp"

class BattleSubwayGenerator
{
public:
    BattleSubwayGenerator(const BattleSubwayFilter* filter, u32 wins, bool trace) : filter(filter), wins(wins), trace(trace){}
    const BattleSubwayFilter* filter = nullptr;
    std::optional<BattleSubwayState> generate(u64 seed) const;

private:
    void advance(BWRNG& rng, u32 adv, const char* Info) const;
    void addPokemonsToTrainer(BWRNG& rng, BattleSubwayTrainer& trainer) const;
    u32 wins = 0;
    bool trace = false;
};

#endif // BATTLESUBWAYGENERATOR_HPP
