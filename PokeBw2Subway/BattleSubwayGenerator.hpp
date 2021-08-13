#pragma once 
#include < atomic>
#include <optional>
#include <vector>

#include "BattleSubwayFilter.hpp"
#include "BattleSubwayState.hpp"
#include "DateTime.hpp"
#include "LCRNG64.hpp"
#include "Options.hpp"
#include "Profile5.hpp"
#include "SubwayType.hpp"

class BattleSubwayGenerator {
 public:
  BattleSubwayGenerator(const Options& opts, const BattleSubwayFilter* filter)
      : opts(opts), filter(filter) {}
  const BattleSubwayFilter* filter = nullptr;
  std::optional<BattleSubwayState> generate(
      u64 seed, u32 multiTeammateUnknownFrameAdvance = 12) const;

 private:
  const Options opts;

  void advance(BWRNG& rng, u32 adv, const char* format, ...) const;
  void log(const char* format, ...) const;
  void addPokemonsToTrainer(
      BWRNG& rng, BattleSubwayTrainer& trainer, bool forTeammate = false,
      const BattleSubwayTrainer* prevTrainerForMulti = nullptr) const;

  const BattleSubwayPlayerTeam& getPlayerTeam() const {
    return this->opts.playerTeamForMulti;
  }
  bool isTraceActive() const { return this->opts.printRngFramesInfo; }
};
