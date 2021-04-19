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

class Options 
{
public:
    Profile5 profile = Profile5("Black2", Game::Black2, 58901 /*TID*/, 0 /*SID*/, 0x9bf123456 /*MAC*/,
                        { true /*None*/, false /*1*/, false /*2*/, false /*3*/ } /*keypresses*/,
                        0x72 /*vcount*/, 6 /*gxstat*/, 4 /*vframe*/, false /*skipLR*/, 
                        0xed2 /*timer0Min*/, 0xed2 /*timer0Max*/, false /*softReset*/,
                        true /*memoryLink*/, false /*shinyCharm*/, DSType::DSOriginal, Language::English);
    DateTime dateTime = DateTime(2009, 1, 1, 0, 0, 0);
    u32 wins = 28;
    u32 macToCheckCount = 0;
    float filterMinRating = 0;
    std::string playerPokemonsFile = "./b2w2_playerPokemons.json";

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
    Options(int argc, char* argv[])
    {
        for (int i = 1; i < argc - 1; i++)
        {
            std::string Key(argv[i]);
            std::string Value(argv[i + 1]);
            if (Key == "--profileFile")
                this->profile = ProfileLoader5::getProfile(Value);
            else if (Key == "--dateTime")
                this->dateTime = this->strToDateTime(Value);
            else if (Key == "--wins")
                this->wins = std::stoi(Value);
            else if (Key == "--macToCheckCount")
                this->macToCheckCount = std::stoi(Value);
            else if (Key == "--filterMinRating")
                this->filterMinRating = std::stof(Value);
            else if (Key == "--playerPokemonsFile")
                this->playerPokemonsFile = Value;            
        }
    }
};

int main(int argc, char* argv[])
{
    if (argc >= 2 && std::string(argv[1]) == "-h")
    {
        std::cout << "This tool calculates the trainers and pokemons that you will encounter in the Battle Subway in Pokemon Black2 or White2\n";
        std::cout << "based on your profile, the time you boot the DS and the number of current wins. \n";
        std::cout << "The tool uses the average of Timer0Min and Timer0Max to determine the initial seed.\n";
        std::cout << "Options:\n";
        std::cout << "--profileFile: Path to profile json file. Use other tool such as PokeFinder to find the right values. Check myProfile.json for an example. \n";
        std::cout << "--dateTime: Date when the game is started, in yyyy-m-d-h-m-s format. Default: 2009-1-1-0-0-0\n";
        std::cout << "--wins: number of wins so far in the Battle Subway. Default: 28 wins\n";
        std::cout << "Ex: --profileFile ./myProfile.json --dateTime 2021-4-18-23-54-9 --wins 0\n";
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
        std::cout << "--filterMinRating: The minimum rating score that a 21-pokemon series must have in order to be printed. Recommended value: 19.5\n";
        std::cout << "\n";
        std::cout << "Credits: This tool and the Battle Subway research was made by RainingChain. (RainingChain#3489 on Discord)\n";
        std::cout << "Special thanks to Admiral Fish for providing the generic pokemon RNG algorithms.\n";
        return 0;
    }

    Options opts(argc, argv);

    for (u64 mac = opts.profile.getMac(); mac <= opts.profile.getMac() + opts.macToCheckCount; mac++)
    {
        if (opts.macToCheckCount != 0)
            std::cout << "mac: 0x" << std::hex << mac << std::dec << "\n" << std::flush;

        BattleSubwaySearcher searcher(opts.profile, opts.filterMinRating);
        if (opts.filterMinRating == 0)
        {
            BattleSubwayGenerator generator(nullptr /*filter*/, opts.wins /*wins*/, false /*trace*/);
            searcher.searchOne(generator, opts.dateTime);
        }
        else
        {
            BattleSubwayFilter filter(true /*active*/, opts.playerPokemonsFile);
            BattleSubwayGenerator generator(&filter, opts.wins /*wins*/, false /*trace*/);
            searcher.startSearch(generator, 8 /*thread*/, DateTime(2000, 1,1, 0,0,0), DateTime(2099,12,31,23,59,59));
        }

    }
	return 0;
}