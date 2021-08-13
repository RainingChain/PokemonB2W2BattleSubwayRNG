#pragma once

#include "Global.hpp"
#include <iostream>

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
		else if (Str == "normalSingle")
			this->type = SubwayType::normalSingle;
		else if (Str == "normalDouble")
			this->type = SubwayType::normalDouble;
		else if (Str == "superDouble")
			this->type = SubwayType::superDouble;
		else if (Str == "normalMulti")
			this->type = SubwayType::normalMulti;
		else if (Str == "superMulti")
			this->type = SubwayType::superMulti;
		else if (Str == "wifi")
			this->type = SubwayType::wifi;
		else if (Str == "battleInstituteDouble")
			this->type = SubwayType::battleInstituteDouble;
		else if (Str == "battleInstituteSingle")
			this->type = SubwayType::battleInstituteSingle;
		else
			{
			std::cout << "Invalid --subwayType\n";
			this->type = SubwayType::superSingle;
			}
	};
	const char* toStr() const
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
