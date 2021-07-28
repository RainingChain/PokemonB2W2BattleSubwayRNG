/*
 * This file is part of PokéFinder
 * Copyright (C) 2017-2020 by Admiral_Fish, bumba, and EzPzStreamz
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include "pch.h"
#include <iostream>
#include <sstream>
#include <string>
#include <charconv>
#include <optional>

#include "Global.hpp"
#include "Game.hpp"
#include "DSType.hpp"
#include "json.hpp"
#include "Language.hpp"
#include "Profile5.hpp"
#include "Utilities.hpp"
#include "SubwayType.hpp"
#include "SHA1.hpp"
#include "Keypresses.hpp"
#include "ProfileLoader.hpp"


class BattleSubwayPlayerTeam
{
    // TODO: Be able to initialize that from command line arguments.
public:
    BattleSubwayPlayerTeam() = default;
    BattleSubwayPlayerTeam(std::string pokemon1, std::string pokemon2, std::string item1, std::string item2, int multiTrainerId) :
        pokemon1(pokemon1),
        pokemon2(pokemon2),
        item1(item1),
        item2(item2),
        multiTrainer(multiTrainer) {}
    BattleSubwayPlayerTeam(int config)
    {
        if (config == 0)
        {
            pokemon1 = "Volcarona";
            pokemon2 = "Garchomp";
            item1 = "Focus Sash";
            item2 = "Choice Band";
            multiTrainer = 300;
        }
        if (config == 1)
        {
            pokemon1 = "Volcarona";
            pokemon2 = "Garchomp";
            item1 = "Focus Sash";
            item2 = "Choice Band";
            multiTrainer = 302;
        }
        std::cout << "Invalid config for BattleSubwayPlayerTeam (" << config << ")\n";
    }
    std::string pokemon1 = "Volcarona"; // "Chandelure";
    std::string pokemon2 = "Garchomp"; // "Volcarona";
    std::string item1 = "Focus Sash"; //"Life Orb";
    std::string item2 = "Choice Band";  //"Choice Specs";
    int multiTrainer = 300; // 300 Attack, 301 Defence, 302 Balanced
};

class Options
{
public:
    Profile5 profile;
    DateTime dateTime = DateTime(2009, 1, 1, 0, 0, 0);
    u32 wins = 28;
    float filterMinRating = 0;
    bool printRngFramesInfo = false;
    std::string playerPokemonsFile = "./b2w2_playerPokemons.json";
    std::vector<u32> playerPokemonsIdxFilter;
    u32 macToCheckCount = 0;
    Subway subway = SubwayType::superSingle;
    /** After initial advances */
    std::optional<u64> pidRng;
    u32 multiTeammateUnknownFrameAdvance = 12;
    int playerTeamConfig = 0;
    BattleSubwayPlayerTeam playerTeam = {};

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
    std::vector<u32> strToVector(std::string str)
    {
        std::string str2 = "[" + str + "]";
        nlohmann::json j = nlohmann::json::parse(str2);
        if (j.is_discarded())
        {
            std::cout << "Invalid --playerPokemonsIdxFilter\n";
            return {};
        }
        return j.get<std::vector<u32>>();
    }
    void print(std::ostream& os)
    {
        os << "\nInput:" << "\n";
        os << "  profile:" << ProfileLoader5::getJson(this->profile).dump() << "\n";
        os << "  wins:" << this->wins << "\n";
        os << "  subwayType:" << this->subway.toStr() << "\n";

        if (this->subway.isMulti())
            {
                os << "  playerTeamConfig:" << this->playerTeamConfig << "\n";
                os << "  multiTeammateUnknownFrameAdvance:" << this->multiTeammateUnknownFrameAdvance << "\n";
            }
        if (this->filterMinRating != 0.0)
        {
            os << "  filterMinRating:" << this->filterMinRating << "\n";
            os << "  playerPokemonsFile:" << this->playerPokemonsFile << "\n";
            if (!this->playerPokemonsIdxFilter.empty())
            {
                os << "  playerPokemonsIdxFilter:[";
                for (const u32 Idx : this->playerPokemonsIdxFilter)
                    os << Idx << ",";
                os << "]\n";
            }
        }
        else if (this->pidRng.has_value())
            os << "  pidRng: 0x" << std::hex << *this->pidRng << std::dec << "\n";
        else
            os << "  dateTime:" << this->dateTime.toString() << "\n";
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
            else if (Key == "--pidRng")
                this->pidRng = std::stoull(Value, nullptr, 16);
            else if (Key == "--multiTeammateUnknownFrameAdvance")
                this->multiTeammateUnknownFrameAdvance = std::stoi(Value);
            else if (Key == "--playerTeamConfig")
            {
                this->playerTeamConfig = std::stoi(Value);
                this->playerTeam = BattleSubwayPlayerTeam(this->playerTeamConfig);
            }
            else if (Key == "--playerPokemonsFile")
                this->playerPokemonsFile = Value;
            else if (Key == "--playerPokemonsIdxFilter")
                this->playerPokemonsIdxFilter = strToVector(Value);
            else if (Key == "--printRngFramesInfo")
                this->printRngFramesInfo = Value != "false";
            else if (Key == "--subwayType")
                this->subway = Value;
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

#endif // OPTIONS_HPP
