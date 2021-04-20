#include "BattleSubwaySearcher.hpp"
#include "Game.hpp"
#include "SHA1.hpp"
#include "Utilities.hpp"
#include <future>
#include <fstream>
#include <sstream>

#include "BattleSubwayGenerator.hpp"
#include "BattleSubwayFilter.hpp"
#include <mutex>

void BattleSubwaySearcher::startSearch(const BattleSubwayGenerator &generator, int threads, DateTime start, const DateTime &end)
{
    long long seconds = start.secondsTo(end) + 1;
    this->progressTodo = seconds;
    if (seconds < threads)
        threads = this->progressTodo;

    std::vector<std::future<void>> threadContainer;

    std::cout << "Searching best pokemon series with rating of at least " << this->minimumRatingForPrint << "...\n" << std::flush;
    long long secondsPerSplit = seconds / threads;
    for (int i = 0; i < threads; i++)
    {
        if (i == threads - 1)
        {
            threadContainer.emplace_back(std::async([=] { search(generator, start, end); }));
        }
        else
        {
            DateTime mid = start;
            mid.addSeconds(secondsPerSplit - 1);
            threadContainer.emplace_back(std::async([=] { search(generator, start, mid); }));
        }
        start.addSeconds(secondsPerSplit);
    }

    for (size_t i = 0; i < threadContainer.size(); i++)
        threadContainer[i].wait();
}


void BattleSubwaySearcher::searchOne(const BattleSubwayGenerator& generator, DateTime start)
{
this->forEachSeed(start, start, [&](u64 seed, const DateTime& date)
    {
    u32 frameAdvance = Utilities::initialAdvancesBW2(seed, false);
    BWRNG rng(seed);
    rng.advance(frameAdvance);
    std::cout << "Date: " << date.toString() << "\n";
    std::cout << "Seed: 0x" << std::hex << seed << ", InitialAdvances:" << std::dec << frameAdvance << "\n";
    const auto& state = generator.generate(rng.getSeed());
    if (state.has_value())
    {
        state->print(std::cout, nullptr /*playerPokemon*/);
        std::cout << "\n\n" << std::flush;
    }
    return true;
    });
}

std::mutex mutex;
void BattleSubwaySearcher::search(const BattleSubwayGenerator &generator, const DateTime &start, const DateTime &end)
{
    if (generator.filter == nullptr)
        return;
    this->forEachSeed(start, end, [&](u64 seed, const DateTime &date)
    {
        u32 frameAdvance = Utilities::initialAdvancesBW2(seed, false);
        BWRNG rng(seed);
        rng.advance(frameAdvance);
            
        u64 pct1 = this->progressTodo / 100;
        if (pct1 != 0 && ++progress % pct1 == 0)
        {
            std::lock_guard<std::mutex> guard(mutex);
            std::cout << "progress: " << (progress / pct1) << "%\n" << std::flush;
        }

        const auto& state = generator.generate(rng.getSeed());
        if (!state.has_value())
            return true;
                        
        std::pair<const BattleSubwayPlayerPokemon*, float> rating = generator.filter->getStateRating(*state);
        if (rating.second >= this->minimumRatingForPrint)
        {
            std::lock_guard<std::mutex> guard(mutex);
            std::cout << "Rating:" << rating.second << ". PlayerPokemon:" << rating.first->name << "\n";
            std::cout << date.toString() << "|0x" << std::hex << seed << "|" << std::dec << frameAdvance << "\n";
            state->print(std::cout, rating.first); 
            std::cout << "\n\n########################################################\n" << std::flush;
        }
        return true;
    });
}

template <typename ForEach>
void BattleSubwaySearcher::forEachSeed(const DateTime &startDate, const DateTime &endDate, const ForEach &func) const
{
    SHA1 sha(this->profile);
    u16 timerMin = (this->profile.getTimer0Min() + this->profile.getTimer0Max()) / 2;
    sha.setTimer0(timerMin, this->profile.getVCount());
    sha.setButton(4281270272);

    bool changedDay = true;
    for (DateTime date = startDate;
        date <= endDate; 
        changedDay = date.addSeconds(1) > 0)
    {
        if (changedDay)
            {
            auto parts = date.getDate().getParts();
            sha.setDate(parts[0] - 2000, parts[1], parts[2], date.getDate().dayOfWeek());
            sha.precompute();
            changedDay = false;
            }
        sha.setTime(date.getTime(), DSType::DSOriginal);
        u64 seed = sha.hashSeed();
        if (!func(seed, date))
            return;
    }
}
