#ifndef BattleSubwaySearcher_HPP
#define BattleSubwaySearcher_HPP

#include "Profile5.hpp"
#include "BattleSubwayState.hpp"
#include "BattleSubwayGenerator.hpp"
#include "Global.hpp"
#include <mutex>
#include <atomic>

class BattleSubwaySearcher
{
public:
    BattleSubwaySearcher() = default;
    explicit BattleSubwaySearcher(const Profile5 &profile, float minimumRatingForPrint) : profile(profile), minimumRatingForPrint(minimumRatingForPrint) {}
    void startSearch(const BattleSubwayGenerator &generator, int threads, DateTime start, const DateTime &end);
    void searchOne(const BattleSubwayGenerator& generator, DateTime start);
    u64 getProgress() const { return progress; }
    std::atomic<bool> found21Wins = false;

private:
    Profile5 profile;

    std::atomic<u64> progress = 0;
    u64 progressTodo = 1;
    float minimumRatingForPrint;

    template <typename ForEach>
    void forEachSeed(const DateTime &startDate, const DateTime &endDate, const ForEach &func) const;

    void search(const BattleSubwayGenerator &generator, const DateTime& start, const DateTime &end);
};

#endif // BattleSubwaySearcher_HPP
