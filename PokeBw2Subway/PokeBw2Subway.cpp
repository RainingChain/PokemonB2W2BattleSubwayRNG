#include "pch.h"
#include <iostream>
#include <sstream>
#include <string>
#include <charconv>

#include "Global.hpp"
#include "Game.hpp"
#include "DSType.hpp"
#include "json.hpp"
#include "Language.hpp"
#include "Options.hpp"
#include "Profile5.hpp"
#include "BattleSubwayGenerator.hpp"
#include "BattleSubwaySearcher.hpp"
#include "BattleSubwayFilter.hpp"
#include "Profile5.hpp"
#include "Utilities.hpp"
#include "SubwayType.hpp"
#include "SHA1.hpp"
#include "Keypresses.hpp"
#include "ProfileLoader.hpp"

// cd C:\Users\samue\source\repos\PokeBw2Subway\x64\Release
// PokemonB2W2BattleSubwayRNG.exe --wins 21 --subwayType superMulti --filterMinRating 14 --printRngFramesInfo false --profile C:\Users\samue\Game\DS\Softwares\RC_PokeSubway\profile.json --playerPokemonsIdxFilter 18
// PokemonB2W2BattleSubwayRNG.exe --wins 7 --subwayType superMulti --pidRng dc85af581cc79a54  --printRngFramesInfo false --playerPokemonsFile "C:\Users\samue\Game\DS\Softwares\RC_PokeSubway\b2w2_playerPokemons.json" --profile C:\Users\samue\Game\DS\Softwares\RC_PokeSubway\profile.json
// PokemonB2W2BattleSubwayRNG.exe --wins 7 --subwayType superMulti --multiTeammateUnknownFrameAdvance 12 --dateTime 2087-07-05-01-01-36 --printRngFramesInfo true --playerPokemonsFile "C:\Users\samue\Game\DS\Softwares\RC_PokeSubway\b2w2_playerPokemons.json" --profile C:\Users\samue\Game\DS\Softwares\RC_PokeSubway\profile.json

void printSeedInfo(const Profile5& profile, const DateTime& dateTime)
{
    SHA1 sha(profile);
    sha.setTimer0(profile.getTimer0Min(), profile.getVCount());

    auto parts = dateTime.getDate().getParts();
    sha.setDate(parts[0] - 2000, parts[1], parts[2], dateTime.getDate().dayOfWeek());
    sha.precompute();

    auto buttons = Keypresses::getKeyPresses(profile.getKeypresses(), profile.getSkipLR());
    auto values = Keypresses::getValues(buttons);
    sha.setButton(values[0]); //4281270272

    sha.setTime(dateTime.getTime().hour(), dateTime.getTime().minute(), dateTime.getTime().second(), profile.getDSType());
    u64 seed = sha.hashSeed();
    u32 initialAdvances = Utilities::initialAdvancesBW2(seed, false);
    std::cout << "seed = 0x" << std::hex << seed;
    std::cout << ", initialAdvances = " << std::dec << initialAdvances;
    BWRNG rng(seed);
    rng.advance(initialAdvances);
    std::cout << ", PIDRNG after initialAdvances = 0x" << std::hex << rng.getSeed() << "\n";
}

void testInitialSeed()
{
Profile5 profile("Black2", Game::Black2, 58901 /*TID*/, 0 /*SID*/, 0x9bf123484 /*MAC*/,
    { true /*None*/, false /*1*/, false /*2*/, false /*3*/ } /*keypresses*/,
    0x72 /*vcount*/, 6 /*gxstat*/, 4 /*vframe*/, false /*skipLR*/, 0xed2 /*timer0Min*/, 0xed2 /*timer0Max*/, false /*softReset*/,
    false /*memoryLink*/, false /*shinyCharm*/, DSType::DSOriginal, Language::English);

printSeedInfo(profile, DateTime(2009, 1, 1, 0, 0, 0));
}

int main(int argc, char* argv[])
{
    //testInitialSeed();

    if (argc >= 2 && (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help"))
    {
        std::cout << "This tool calculates the trainers and pokemons that you will encounter in the Battle Subway in Pokemon Black 2 or White 2\n";
        std::cout << "based on your profile, the time you boot the DS and the number of current wins. \n";
        std::cout << "The tool uses the average of Timer0Min and Timer0Max to determine the initial seed.\n";
        std::cout << "The tool assumes no buttons is held when booting the game.\n";
        std::cout << "\n";
        std::cout << "Important note: After changing the DS clock, loading the game and changing map will cause daily events to reset\n";
        std::cout << "which will increase the PID RNG frame, causing the pokemons to be different than what the tool says.\n";
        std::cout << "To avoid this issue, change the DS clock, load the game, leave the subway entirely, save, come back inside\n";
        std::cout << "and save again next to the operator.\n";
        std::cout << "\n";
        std::cout << "Options:\n";
        std::cout << "--profile: Path to profile json file. Use other tool such as PokeFinder to find the right values. Default: ./profile.json\n";
        std::cout << "--dateTime: Date when the game is started, in yyyy-m-d-h-m-s format. Default: 2009-1-1-0-0-0\n";
        std::cout << "--wins: number of wins so far in the Battle Subway. Must be a multiple of 7. For Battle Institute, must be 0. Default: 28 wins\n";
        std::cout << "--subwayType: The type of subway. Possible values: normalSingle, superSingle, normalDouble, superDouble, battleInstituteSingle, battleInstituteDouble. Default: superSingle\n";
        std::cout << "--printRngFramesInfo: Advanced. Display RNG frame info for RNG searchers. Default: false\n";
        std::cout << "Ex: PokemonB2W2BattleSubwayRNG.exe --profile ./profile.json --dateTime 2021-4-18-23-54-9 --wins 0\n";
        std::cout << "\n";
        std::cout << "Search usage:\n";
        std::cout << "The tool can also be used to find the boot time that will give you the most chance to win.\n";
        std::cout << "For example, it can determine that if using Volcarona, the best time to boot the game is at 2066-9-10-5-27-57, because\n";
        std::cout << "if booted at that time, 19 out of the 21 pokemons in the series is outsped by Volcarona and Volcarona is guaranteed to OHKO them.\n";
        std::cout << "Search options: \n";
        std::cout << "--playerPokemonsFile: Path to the file containing pokemon ratings. Check ./b2w2_playerPokemons.json for an example. \n";
        std::cout << "                      \"ratings\" is an array of [PokemonBattleSubwayId,AbilityIdx,Rating]\n";
        std::cout << "                      To determine the PokemonBattleSubwayId of a pokemon, check ./b2w2_pokemons.json\n";
        std::cout << "                      For example, [408,0,1] means that encountering a Skarmory with ability Keen Eye has a +1 rating.\n";
        std::cout << "                      Default: ./b2w2_playerPokemons.json\n";
        std::cout << "--filterMinRating: The minimum rating score that a 21-pokemon series must have in order to be printed. Recommended value: 19\n";
        std::cout << "\n";
        std::cout << "Credits: This tool and the Battle Subway RNG research was done by RainingChain. (RainingChain#3489 on Discord)\n";
        std::cout << "Special thanks to Admiral Fish for providing the generic pokemon RNG algorithms.\n";
        std::cout << "Version 1.0.0\n";
        return 0;
    }
    //TODO_FUTUR: Support normalMulti, superMulti, wifi. Update bulbagarden info.

    Options opts(argc, argv);
    opts.print(std::cout);

    for (u64 mac = opts.profile.getMac(); mac <= opts.profile.getMac() + opts.macToCheckCount; mac++)
    {
        if (opts.macToCheckCount != 0)
            std::cout << "mac: 0x" << std::hex << mac << std::dec << "\n" << std::flush;

		auto PrintStartMsg = [](const std::string& Text)
		{
			std::cout << "--------------------------------------------------------------------------------------------\n";
			std::cout << Text << "\n" << std::flush;
			std::cout << "--------------------------------------------------------------------------------------------\n";
		};

        BattleSubwaySearcher searcher(opts);
		if (opts.filterMinRating == 0)
		{
			BattleSubwayGenerator generator(opts, nullptr /*filter*/);
			//searcher.searchCustom(generator);

			if (opts.pidRng.has_value())
			{
				PrintStartMsg("Searching a single result from input PID RNG (after initial frames)");
				searcher.searchByPidRng(generator, *opts.pidRng);
			}
			else
			{
				PrintStartMsg("Searching a single result from input date time and profile");
				searcher.searchOne(generator, opts.dateTime);
			}
        }
        else
        {
			PrintStartMsg("Searching for date times that will result in pokemon series with rating of at least " + std::to_string(opts.filterMinRating));

            BattleSubwayFilter filter(opts, true /*active*/);
            BattleSubwayGenerator generator(opts, &filter);
            searcher.startSearch(generator, 8 /*thread*/, DateTime(2000, 1, 1, 0, 0, 0), DateTime(2099, 12, 31, 23, 59, 59));
        }

    }
    return 0;
}