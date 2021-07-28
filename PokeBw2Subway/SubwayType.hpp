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

#ifndef SubwayType_HPP
#define SubwayType_HPP

#include "Global.hpp"

enum SubwayType : u8
{
    normalSingle,
    superSingle,
    wifi,
    normalDouble,
    superDouble,
    normalMulti,
    superMulti,
    battleInstituteSingle,
    battleInstituteDouble
};

class Subway
{
public:
	Subway(SubwayType type) : type(type) {};
	SubwayType type;

	bool operator==(SubwayType type) const {
		return this->type == type;
	};

	Subway(const std::string& Str) 
	{
		if (Str == "superSingle")
			this->type = SubwayType::superSingle;
		if (Str == "normalSingle")
			this->type = SubwayType::normalSingle;
		if (Str == "normalDouble")
			this->type = SubwayType::normalDouble;
		if (Str == "superDouble")
			this->type = SubwayType::superDouble;
		if (Str == "normalMulti")
			this->type = SubwayType::normalMulti;
		if (Str == "superMulti")
			this->type = SubwayType::superMulti;
		if (Str == "wifi")
			this->type = SubwayType::wifi;
		if (Str == "battleInstituteDouble")
			this->type = SubwayType::battleInstituteDouble;
		if (Str == "battleInstituteSingle")
			this->type = SubwayType::battleInstituteSingle;
		std::cout << "Invalid --subwayType\n";
		this->type = SubwayType::superSingle;
	};
	const char* toStr()
	{
		switch (type)
		{
		case normalSingle: return "normalSingle";
		case superSingle: return "superSingle";
		case wifi: return "wifi";
		case superDouble: return "superDouble";
		case normalMulti: return "normalMulti";
		case superMulti: return "superMulti";
		case battleInstituteSingle: return "battleInstituteSingle";
		case battleInstituteDouble: return "battleInstituteDouble";
		default: return "INVALID";
		}
	}
	bool isSubway() const
	{
		return !this->isBattleInstitute();
	}
	bool isBattleInstitute() const
	{
		return this->type == SubwayType::battleInstituteSingle || this->type == SubwayType::battleInstituteDouble;
	}
	bool isMulti() const
	{
		return this->type == SubwayType::normalMulti || this->type == SubwayType::superMulti;
	}
	bool isDouble() const
	{
		return this->type == SubwayType::battleInstituteDouble || this->type == SubwayType::superDouble
			|| this->type == SubwayType::normalDouble;
	}
	bool isSingle() const
	{
		return this->type == SubwayType::battleInstituteSingle || this->type == SubwayType::superSingle
			|| this->type == SubwayType::normalSingle || this->type == SubwayType::wifi;
	}
	u8 getPokemonCountByTrainer() const
	{
		if (this->isMulti())
			return 2;
		if (this->isSingle())
			return 3;
		return 4;
	}
	u8 getTrainerCountByBattle() const
	{
		if (this->isMulti())
			return 2;
		return 1;
	}
	u8 getBattleCount() const
	{
		if (this->isBattleInstitute())
			return 5;
		return 7;
	}
};

#endif // SubwayType_HPP
