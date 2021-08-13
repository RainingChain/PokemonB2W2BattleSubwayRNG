#include "BattleSubwaySearcher.hpp"

#include <fstream>
#include <future>
#include <mutex>
#include <sstream>

#include "BattleSubwayFilter.hpp"
#include "BattleSubwayGenerator.hpp"
#include "Game.hpp"
#include "SHA1.hpp"
#include "Utilities.hpp"

// public
void BattleSubwaySearcher::startSearch(const BattleSubwayGenerator &generator,
                                       int threads, DateTime start,
                                       const DateTime &end) {
  long long seconds = start.secondsTo(end) + 1;
  this->progressTodo = seconds;
  if (seconds < threads) threads = (int)this->progressTodo;

  std::vector<std::future<void>> threadContainer;

  long long secondsPerSplit = seconds / threads;
  for (int i = 0; i < threads; i++) {
    if (i == threads - 1) {
      threadContainer.emplace_back(
          std::async([=] { search(generator, start, end); }));
    } else {
      DateTime mid = start;
      mid.addSeconds(secondsPerSplit - 1);
      threadContainer.emplace_back(
          std::async([=] { search(generator, start, mid); }));
    }
    start.addSeconds(secondsPerSplit);
  }

  for (size_t i = 0; i < threadContainer.size(); i++) threadContainer[i].wait();
}

void BattleSubwaySearcher::searchOne(const BattleSubwayGenerator &generator,
                                     DateTime start) {
  this->forEachSeed(start, start, [&](u64 seed, const DateTime &date) {
    u32 frameAdvance = Utilities::initialAdvancesBW2(seed, false);
    BWRNG rng(seed);
    rng.advance(frameAdvance);
    std::cout << "Date: " << date.toInputString() << "\n";
    std::cout << "Seed: 0x" << std::hex << seed
              << ", InitialAdvances:" << std::dec << frameAdvance
              << ", PIDRNG: 0x" << std::hex << rng.getSeed() << std::dec
              << "\n";
    this->searchByPidRng(generator, rng.getSeed());
    return true;
  });
}

void BattleSubwaySearcher::searchByPidRng(
    const BattleSubwayGenerator &generator, u64 pidRng) {
  for (u32 i = this->opts.multiTeammateUnknownFrameAdvance;
       i <= this->opts.multiTeammateUnknownFrameAdvance; i++) {
    std::cout << "multiTeammateUnknownFrameAdvance: " << i << "\n\n"
              << std::flush;
    BattleSubwayFilter filter(this->opts, true);
    const auto &state = generator.generate(pidRng, i);
    if (state.has_value()) {
      const auto* PlayerPoke = this->opts.getFirstPlayerPokemon();
      if (PlayerPoke != nullptr)
        std::cout << "Rating with " << PlayerPoke->description << ": " << filter.getStateRating(*state, *PlayerPoke) << "\n";
      state->print(std::cout, PlayerPoke);
      std::cout << "\n\n" << std::flush;
    }
  }
}

void BattleSubwaySearcher::searchCustom(
    const BattleSubwayGenerator &generator) {
  const DateTime start(2000, 1, 1, 0, 0, 0);
  const DateTime end(2099, 1, 1, 0, 0, 0);
  this->forEachSeed(start, end, [&](u64 seed, const DateTime &date) {
    u32 frameAdvance = Utilities::initialAdvancesBW2(seed, false);
    BWRNG rng(seed);
    rng.advance(frameAdvance);

    const auto &state = generator.generate(
        rng.getSeed(), this->opts.multiTeammateUnknownFrameAdvance);
    if (!state.has_value()) return true;

    bool isGood = true;
    state->getMultiTeammate().forEachPokemon([&](const auto &Poke, size_t i) {
      if (i == 0 and Poke.getId() != 896)  // Haxorus
        isGood = false;
      if (i == 1 and Poke.getId() != 725)  // Archeops
        isGood = false;
    });

    if (isGood) {
      std::cout << date.toInputString() << "|0x" << std::hex << seed << "|"
                << std::dec << frameAdvance << "\n";
      std::cout << "Before saving seed: 0x" << std::hex << rng.getSeed() << "|"
                << std::dec << "\n";
      state->print(std::cout, this->opts.getFirstPlayerPokemon());
      std::cout
          << "\n\n########################################################\n"
          << std::flush;
    }
    return true;
  });
}

// private
std::mutex mutex;
void BattleSubwaySearcher::search(const BattleSubwayGenerator &generator,
                                  const DateTime &start, const DateTime &end) {
  if (generator.filter == nullptr) return;
  this->forEachSeed(start, end, [&](u64 seed, const DateTime &date) {
    u32 frameAdvance = Utilities::initialAdvancesBW2(seed, false);
    BWRNG rng(seed);
    rng.advance(frameAdvance);
    u64 seedAfterInitAdvance = rng.getSeed();

    u64 pct1 = this->progressTodo / 100;
    if (pct1 != 0 && ++progress % pct1 == 0) {
      std::lock_guard<std::mutex> guard(mutex);
      std::cout << "progress: " << (progress / pct1) << "%\n" << std::flush;
    }

    const auto &state = generator.generate(rng.getSeed());
    if (!state.has_value()) 
      return true;

    std::pair<const BattleSubwayPlayerPokemon *, float> rating = 
        generator.filter->getStateRating(*state);

    if (rating.second >= this->minimumRatingForPrint) {
      //this->minimumRatingForPrint = rating.second + 0.01;
      std::vector<BattleSubwayState> states;
      states.push_back(*state);

      for (size_t i = 0; i < states.size(); i++) {
        const auto &state = states[i];
        std::lock_guard<std::mutex> guard(mutex);
        std::cout << "Rating:" << rating.second
                  << ". PlayerPokemon:" << rating.first->description << "\n";
        std::cout << date.toInputString() << "|Seed: 0x" << std::hex << seed
                  << "|InitialAdv: " << std::dec << frameAdvance
                  << "|PIDNRG: 0x" << std::hex << seedAfterInitAdvance
                  << std::dec << "|MultiFrame: " << i << "\n";
        state.print(std::cout, rating.first);
        std::cout
            << "\n\n########################################################\n"
            << std::flush;
      }
    }
    return true;
  });
}

template <typename ForEach>
void BattleSubwaySearcher::forEachSeed(const DateTime &startDate,
                                       const DateTime &endDate,
                                       const ForEach &func) const {
  SHA1 sha(this->opts.profile);
  u16 timerMin =
      (this->opts.profile.getTimer0Min() + this->opts.profile.getTimer0Max()) /
      2;
  sha.setTimer0(timerMin, this->opts.profile.getVCount());
  sha.setButton(4281270272);

  bool changedDay = true;
  for (DateTime date = startDate; date <= endDate;
       changedDay = date.addSeconds(1) > 0) {
    if (changedDay) {
      auto parts = date.getDate().getParts();
      sha.setDate(parts[0] - 2000, parts[1], parts[2],
                  date.getDate().dayOfWeek());
      sha.precompute();
      changedDay = false;
    }
    sha.setTime(date.getTime(), DSType::DSOriginal);
    u64 seed = sha.hashSeed();
    if (!func(seed, date)) return;
  }
}
