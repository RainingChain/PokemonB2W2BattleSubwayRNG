#ifndef BattleSubwaySearcher_HPP
#define BattleSubwaySearcher_HPP

#include "Profile5.hpp"
#include "BattleSubwayState.hpp"
#include "BattleSubwayGenerator.hpp"
#include "Options.hpp"
#include "Global.hpp"
#include <mutex>
#include <atomic>

class BattleSubwaySearcher
{
public:
    BattleSubwaySearcher() = default;
    explicit BattleSubwaySearcher(const Options& opts)
		: opts(opts)
    {
        this->minimumRatingForPrint = opts.filterMinRating;
    }
    void startSearch(const BattleSubwayGenerator &generator, int threads, DateTime start, const DateTime &end);
    void searchOne(const BattleSubwayGenerator& generator, DateTime start);
	void searchByPidRng(const BattleSubwayGenerator& generator, u64 pidRng);

	void searchCustom(const BattleSubwayGenerator &generator);

    u64 getProgress() const { return progress; }
    std::atomic<bool> found21Wins = false;

private:
    std::atomic<u64> progress = 0;
    u64 progressTodo = 1;
    float minimumRatingForPrint;
    const Options opts;
    template <typename ForEach>
    void forEachSeed(const DateTime &startDate, const DateTime &endDate, const ForEach &func) const;

    void search(const BattleSubwayGenerator &generator, const DateTime& start, const DateTime &end);
};

#endif // BattleSubwaySearcher_HPP
