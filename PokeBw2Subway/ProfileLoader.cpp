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

#include "ProfileLoader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

using json = nlohmann::json;

namespace ProfileLoader5
{
    Profile5 getProfile(const json &j)
    {
        std::string name = j["name"].get<std::string>();
        Game version = j["version"].get<Game>();
        u16 tid = j["tid"].get<u16>();
        u16 sid = j["sid"].get<u16>();
        u64 mac = std::stoull(j["mac"].get<std::string>(), nullptr, 16);
        std::vector<bool> keypresses = j["keypresses"].get<std::vector<bool>>();
        u8 vcount = j["vcount"].get<u8>();
        u8 gxstat = j["gxstat"].get<u8>();
        u8 vframe = j["vframe"].get<u8>();
        bool skipLR = j["skipLR"].get<bool>();
        u16 timer0Min = j["timer0Min"].get<u16>();
        u16 timer0Max = j["timer0Max"].get<u16>();
        bool softReset = j["softReset"].get<bool>();
        bool memoryLink = j["memoryLink"].get<bool>();
        bool shinyCharm = j["shinyCharm"].get<bool>();
        DSType dsType = j["dsType"].get<DSType>();
        Language language = j["language"].get<Language>();
        return Profile5(name, version, tid, sid, mac, keypresses, vcount, gxstat, vframe, skipLR, timer0Min, timer0Max, softReset,
                        memoryLink, shinyCharm, dsType, language);
    }

    std::optional<Profile5> getProfile(const std::string& Path)
    {
        std::ifstream read(Path);
        if (!read.is_open())
            return std::nullopt;
        json j = json::parse(read, nullptr, false);
        return ProfileLoader5::getProfile(j);
    }

    json getJson(const Profile5& profile)
    {
        json j;
        //j["name"] = profile.getName();
        j["version"] = profile.getVersion();
        //j["tid"] = profile.getTID();
        //j["sid"] = profile.getSID();
        std::stringstream stream;
        stream << std::hex << profile.getMac();
        j["mac"] = stream.str();
        //j["keypresses"] = profile.getKeypresses();
        j["vcount"] = profile.getVCount();
        j["gxstat"] = profile.getGxStat();
        j["vframe"] = profile.getVFrame();
        //j["skipLR"] = profile.getSkipLR();
        j["timer0Min"] = profile.getTimer0Min();
        j["timer0Max"] = profile.getTimer0Max();
        j["softReset"] = profile.getSoftReset();
        j["memoryLink"] = profile.getMemoryLink();
        //j["shinyCharm"] = profile.getShinyCharm();
        j["dsType"] = profile.getDSType();
        j["language"] = profile.getLanguage();
        return j;
    }
}
