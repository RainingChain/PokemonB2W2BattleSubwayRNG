#include "pch.h"
#include <iostream>
#include <sstream>
#include <string>

#include "Global.hpp"
#include "Game.hpp"
#include "DSType.hpp"
#include "Language.hpp"
#include "Profile5.hpp"
#include "BattleSubwayGenerator.hpp"
#include "BattleSubwaySearcher.hpp"
#include "BattleSubwayFilter.hpp"
#include "Profile5.hpp"
#include "Utilities.hpp"
#include "SHA1.hpp"
#include "Keypresses.hpp"
#include "ProfileLoader.hpp"

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

class Options
{
public:
    Profile5 profile;
    DateTime dateTime = DateTime(2009, 1, 1, 0, 0, 0);
    u32 wins = 28;
    float filterMinRating = 0;
    bool printRngFramesInfo = false;
    std::string playerPokemonsFile = "./b2w2_playerPokemons.json";
    u32 macToCheckCount = 0;

    DateTime strToDateTime(std::string dt)
    {
        std::vector<u32> Ints;
        std::string temp;
        std::istringstream f(dt);
        while (std::getline(f, temp, '-'))
            Ints.push_back(std::stoi(temp));
        while (Ints.size() < 6)
            Ints.push_back(0);
        return DateTime(Ints[0], Ints[1], Ints[2], Ints[3], Ints[4], Ints[5]);
    }
    void print(std::ostream &os)
    {
        os << "\nInput:" << "\n";
        os << "  profile:" << ProfileLoader5::getJson(this->profile).dump() << "\n";
        os << "  dateTime:" << this->dateTime.toString() << "\n";
        os << "  wins:" << wins << "\n";
        os << "  filterMinRating:" << this->filterMinRating << "\n";
        if (this->filterMinRating != 0.0)
            os << "  playerPokemonsFile:" << this->playerPokemonsFile << "\n";
        os << "\n";
    }
    Options(int argc, char* argv[])
    {
        std::string profileFile = "./profile.json";
        for (int i = 1; i < argc - 1; i += 2)
        {
            std::string Key(argv[i]);
            std::string Value(argv[i + 1]);
            if (Key == "--profile")
                profileFile = Value;
            else if (Key == "--dateTime")
                this->dateTime = this->strToDateTime(Value);
            else if (Key == "--wins")
                this->wins = (std::stoi(Value) / 7) * 7; //round to 7
            else if (Key == "--macToCheckCount")
                this->macToCheckCount = std::stoi(Value);
            else if (Key == "--filterMinRating")
                this->filterMinRating = std::stof(Value);
            else if (Key == "--playerPokemonsFile")
                this->playerPokemonsFile = Value;
            else if (Key == "--printRngFramesInfo")
                this->printRngFramesInfo = Value != "false";
            else
                std::cout << "Invalid arguments: " << Key << "\n";
        }
        auto profile = ProfileLoader5::getProfile(profileFile);
        if (!profile.has_value())
            std::cout << "Invalid profile.";
        else
            this->profile = *profile;
    }
};

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
        std::cout << "--wins: number of wins so far in the Battle Subway. Must be a multiple of 7. Default: 28 wins\n";
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

    Options opts(argc, argv);
    opts.print(std::cout);

    for (u64 mac = opts.profile.getMac(); mac <= opts.profile.getMac() + opts.macToCheckCount; mac++)
    {
        if (opts.macToCheckCount != 0)
            std::cout << "mac: 0x" << std::hex << mac << std::dec << "\n" << std::flush;

        BattleSubwaySearcher searcher(opts.profile, opts.filterMinRating);
        if (opts.filterMinRating == 0)
        {
            BattleSubwayGenerator generator(nullptr /*filter*/, opts.wins /*wins*/, opts.printRngFramesInfo /*trace*/);
            searcher.searchOne(generator, opts.dateTime);
        }
        else
        {
            BattleSubwayFilter filter(true /*active*/, opts.playerPokemonsFile);
            BattleSubwayGenerator generator(&filter, opts.wins /*wins*/, false /*trace*/);
            searcher.startSearch(generator, 8 /*thread*/, DateTime(2000, 1, 1, 0, 0, 0), DateTime(2099, 12, 31, 23, 59, 59));
        }

    }
    return 0;
}