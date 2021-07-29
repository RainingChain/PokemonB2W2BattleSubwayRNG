#pragma once
#include <atomic>
#include <mutex>

#include "BattleSubwayGenerator.hpp"
#include "BattleSubwayState.hpp"
#include "Global.hpp"
#include "Options.hpp"
#include "Profile5.hpp"

class BattleSubwaySearcher {
 public:
  BattleSubwaySearcher() = default;
  explicit BattleSubwaySearcher(const Options &opts) : opts(opts) {
    this->minimumRatingForPrint = opts.filterMinRating;
  }
  void startSearch(const BattleSubwayGenerator &generator, int threads,
                   DateTime start, const DateTime &end);
  void searchOne(const BattleSubwayGenerator &generator, DateTime start);
  void searchByPidRng(const BattleSubwayGenerator &generator, u64 pidRng);

  void searchCustom(const BattleSubwayGenerator &generator);

  u64 getProgress() const { return progress; }
  std::atomic<bool> found21Wins = false;

 private:
  std::atomic<u64> progress = 0;
  u64 progressTodo = 1;
  float minimumRatingForPrint;
  const Options opts;
  template <typename ForEach>
  void forEachSeed(const DateTime &startDate, const DateTime &endDate,
                   const ForEach &func) const;

  void search(const BattleSubwayGenerator &generator, const DateTime &start,
              const DateTime &end);
  const BattleSubwayPlayerPokemon *firstPlayerPokemon() const;
};
