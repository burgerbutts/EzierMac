#include "config.h"

void Config::ReadSaveFile( )
{
	std::ifstream inFile( "skills.ini", std::ifstream::in );
	std::string buf;
	int extend = 0;

	if ( inFile.is_open( ) && inFile.good( ) )
	{
		do
		{
			buf.clear( );
			std::getline( inFile, buf );

			if ( buf.empty( ) || buf.at( 0 ) == ';' )
				continue;

			if ( buf.find( "Extended" ) != std::string::npos )
			{
				std::size_t pos = buf.find( '=' );

				if ( pos != std::string::npos )
				{
					std::string strValue = buf.substr( pos + 1 );
					strValue = UpperString( strValue );

					if ( strValue.find( "TRUE" ) != std::string::npos )
						useExtended = true;
					else
						useExtended = false;
				}

				continue;
			}

			if ( buf.find( "MaxVE" ) != std::string::npos )
			{
				std::size_t pos = buf.find( '=' );

				if ( pos != std::string::npos )
				{
					std::string strValue = buf.substr( pos + 1 );
					maxVE = std::stoi( strValue );
				}

				continue;
			}

			if ( buf.find( "MedTime" ) != std::string::npos )
			{
				std::size_t pos = buf.find( "=" );

				if ( pos != std::string::npos )
				{
					std::string strValue = buf.substr( pos + 1 );
					medTime = std::stod( strValue );
				}

				continue;
			}

			if ( buf.find( "MedVE" ) != std::string::npos )
			{
				std::size_t pos = buf.find( "=" );

				if ( pos != std::string::npos )
				{
					std::string strValue = buf.substr( pos + 1 );
					medVE = std::stoi( strValue );
				}

				continue;
			}

			int element = GetSkillElement( buf );

			if ( buf.find( "Enabled" ) != std::string::npos )
			{
				std::size_t pos = buf.find( '=' );

				if ( pos != std::string::npos )
				{
					std::string strValue = buf.substr( pos + 1 );
					strValue = UpperString( strValue );

					if ( strValue.find( "TRUE" ) != std::string::npos )
						g_Skills[ element ].bEnabled = true;
					else
						g_Skills[ element ].bEnabled = false;
				}
			}
			else if ( buf.find( "Cooldown" ) != std::string::npos )
			{
				std::size_t pos = buf.find( '=' );

				if ( pos != std::string::npos )
				{
					std::string strValue = buf.substr( pos + 1 );
					int value = std::stoi( strValue );

					g_Skills[ element ].cooldown = value;
				}
			}
			else if ( buf.find( "VeCost" ) != std::string::npos )
			{
				std::size_t pos = buf.find( '=' );

				if ( pos != std::string::npos )
				{
					std::string strValue = buf.substr( pos + 1 );
					int value = std::stoi( strValue );

					g_Skills[ element ].VECost = value;
				}
			}
			else if ( buf.find( "CastTime" ) != std::string::npos )
			{
				std::size_t pos = buf.find( '=' );

				if ( pos != std::string::npos )
				{
					std::string strValue = buf.substr( pos + 1 );
					double value = std::stod( strValue );

					g_Skills[ element ].castTime = value;
				}
			}
		} while ( !inFile.eof( ) );

		inFile.close( );
		return;
	}
	else
		CreateSaveFile( );
}

int Config::GetMaxVE( )
{
	return maxVE;
}

bool Config::GetExtended( )
{
	return useExtended;
}

double Config::GetMedTime( )
{
	return medTime;
}

int Config::GetMedVE( )
{
	return medVE;
}

auto Config::MyConfig( )
{
	maxVE = 2500;
	medTime = 50;
	medVE = 150;
	useExtended = false;
}

std::string Config::UpperString( std::string string )
{
	for ( std::string::iterator & it = string.begin( ); it < string.end( ); ++it )
		*it = toupper( *it );

	return string;
}

int Config::GetSkillElement( std::string buf )
{
	int skillNum = std::stoi( buf.substr( 0, 1 ) );

	if ( buf.at( 1 ) == 'E' )
		return skillNum + 10;
	else
		return skillNum;
}

void Config::CreateSaveFile( )
{
	std::ofstream outFile( "skills.ini", std::ofstream::out );

	if ( outFile.is_open( ) && outFile.good( ) )
	{
		outFile << "; Extended skill bar will not work with an inactive window" << std::endl;
		outFile << "Extended=true\n";
		outFile << "MaxVE=2700\n";
		outFile << "MedTime=50\n";
		outFile << "MedVE=150\n\n";
		outFile << "; [basic skill bar]\n";

		for ( int i = 0; i < _countof( g_Skills ) / 2; ++i )
		{
			outFile << i << "_Enabled=false\n";
			outFile << i << "_Cooldown=1\n";
			outFile << i << "_VeCost=20\n";
			outFile << i << "_CastTime=2.5\n";
		}

		outFile << "\n\n; [extended skill bar]\n";

		for ( int i = 0; i < _countof( g_Skills ) / 2; ++i )
		{
			outFile << i << "E" << "_Enabled=false\n";
			outFile << i << "E" << "_Cooldown=2\n";
			outFile << i << "E" << "_VeCost=30\n";
			outFile << i << "E" << "_CastTime=2.5\n";
		}

		outFile.close( );
		ReadSaveFile( );
	}
	else
		std::cout << "Failed to open the skills.txt file, fix your shit." << std::endl;
}
