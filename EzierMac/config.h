#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include "global.h"

extern struct SkillData g_Skills[ 20 ];

class Config
{
public:
	void ReadSaveFile( );
	int GetMaxVE( );
	bool GetExtended( );
	double GetMedTime( );
	int GetMedVE( );

private:
	auto MyConfig( );
	std::string UpperString( std::string string );
	int GetSkillElement( std::string buf );
	void CreateSaveFile( );

	int maxVE;
	double medTime;
	int medVE;
	bool useExtended;
};
