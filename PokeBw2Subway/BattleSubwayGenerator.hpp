#ifndef BATTLESUBWAYGENERATOR_HPP
#define BATTLESUBWAYGENERATOR_HPP

#include "BattleSubwayFilter.hpp"
#include "BattleSubwayState.hpp"
#include "DateTime.hpp"
#include "Profile5.hpp"
#include "SubwayType.hpp"
#include "Options.hpp"
#include <vector>
#include <optional>
#include <atomic>
#include "LCRNG64.hpp"

class BattleSubwayGenerator
{
public:
	BattleSubwayGenerator(const Options& opts, const BattleSubwayFilter* filter) :
		opts(opts),
		filter(filter){}
    const BattleSubwayFilter* filter = nullptr;
    std::optional<BattleSubwayState> generate(u64 seed, u32 multiTeammateUnknownFrameAdvance = 12) const;

private:
	const Options opts;

    void advance(BWRNG& rng, u32 adv, const char *format, ...) const;
	void log(const char *format, ...) const;
    void addPokemonsToTrainer(
		BWRNG& rng, BattleSubwayTrainer& trainer, 
		bool forTeammate = false, const BattleSubwayTrainer* prevTrainerForMulti = nullptr) const;

	const BattleSubwayPlayerTeam& getPlayerTeam() const
	{
		return this->opts.playerTeam;
	}
	bool isTraceActive() const
	{
		return this->opts.printRngFramesInfo;
	}
};

#endif // BATTLESUBWAYGENERATOR_HPP
