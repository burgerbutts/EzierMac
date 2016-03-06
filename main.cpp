#include "config.h"
#include "global.h"
#include <Windows.h>
#include <ctime>
#include <bitset>

#define BAR_COUNT 2

// icon by Freepik @ http://www.flaticon.com/free-icon/tap-with-two-fingers_109676

DWORD g_dwSendInput = ( DWORD ) GetProcAddress( LoadLibrary( "User32.dll" ), "SendInput" ) + 0x5;
DWORD g_dwSendMessageA = ( DWORD ) GetProcAddress( LoadLibrary( "User32.dll" ), "SendMessageA" ) + 0x05;
SkillData g_Skills[ 20 ];
HWND hWnd;

__declspec( naked ) UINT CALLBACK MySendInput( UINT cInputs, LPINPUT pInputs, int cbSize )
{
	_asm mov eax, 0x1084
	_asm jmp g_dwSendInput
}

__declspec( naked ) LRESULT CALLBACK MySendMessageA( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	_asm mov edi, edi
	_asm push ebp
	_asm mov ebp, esp
	_asm jmp g_dwSendMessageA
}

void SendKey( int index, bool useExtended )
{
	INPUT in;
	char theKey;

	ZeroMemory( &in, sizeof( in ) );
	ZeroMemory( &in.ki, sizeof( in.ki ) );

	in.type = INPUT_KEYBOARD;

	auto keyDown = [ ] ( INPUT in, char key, bool useExtended )
	{
		//keyState |= (scan code << 16); // scan code starts at 16

		if ( useExtended )
		{
			in.ki.wVk = key;
			in.ki.dwFlags = 0;
			MySendInput( 1, &in, sizeof( in ) );
		}
		else
			MySendMessageA( hWnd, WM_KEYDOWN, VkKeyScan( key ), 0 );
	};

	auto keyUp = [ ] ( INPUT in, char key, bool useExtended )
	{
		//keyState |= (scan code << 16); // scan code starts at 16
		if ( useExtended )
		{
			in.ki.wVk = key;
			in.ki.dwFlags = KEYEVENTF_KEYUP;
			MySendInput( 1, &in, sizeof( in ) );
		}
		else
			MySendMessageA( hWnd, WM_KEYUP, VkKeyScan( key ), 1 );
	};

	if ( index == -1 )
	{
		Sleep( 2500 ); // wait in case of animation

		for ( int i = 0; i < 10; ++i )	// push med 10 times in case of lag or key doesnt register
		{
			keyDown( in, 'p', useExtended );
			keyUp( in, 'p', useExtended );
			Sleep( 100 );
		}

		return;
	}
	else if ( index >= ( _countof( g_Skills ) / BAR_COUNT ) )
		theKey = '0' + ( index - ( _countof( g_Skills ) / BAR_COUNT ) );
	else
		theKey = '0' + index;

	//select yourself
	keyDown( in, 'u', useExtended );
	keyUp( in, 'u', useExtended );

	if ( useExtended && ( index >= ( _countof( g_Skills ) / BAR_COUNT ) ) )
	{
		std::cout << "Pressing CTRL + " << ( char ) theKey << std::endl;

		keyDown( in, VK_CONTROL, useExtended );
		Sleep( 100 );
		keyDown( in, theKey, useExtended );

		//Sleep( 200 );

		keyUp( in, theKey, useExtended );
		Sleep( 100 );
		keyUp( in, VK_CONTROL, useExtended );
	}
	else
	{
		std::cout << "Pressing " << ( char ) theKey << std::endl;

		keyDown( in, theKey, useExtended );
		keyUp( in, theKey, useExtended );
	}
}

int main( )
{
	time_t curTime;
	Config myConfig;
	bool bPause = false, needMed = false;

	myConfig.ReadSaveFile( );

	std::cout << "Waiting for Nine Dragons...." << std::endl;

	do
	{
		hWnd = FindWindow( "X3DKernel", NULL );
	} while ( hWnd == NULL || hWnd == INVALID_HANDLE_VALUE );

	std::cout << "Found Nine Dragons at 0x" << hWnd << std::endl;

	// init skills cooldown time
	for ( int i = 0; i < _countof( g_Skills ); ++i )
		ZeroMemory( &g_Skills[ i ].lastUsed, sizeof( time_t ) );

	int curVE = myConfig.GetMaxVE( );

	std::cout << "Set your cast time with the animation speed in mind!" << std::endl;
	std::cout << "When your skills level up, they may require more VE." << std::endl;
	std::cout << "If you have your MED & STEPS enabled you will need to lower your max VE." << std::endl;
	std::cout << "\nWaiting 5 seconds to start..." << std::endl;
	Sleep( 5000 );

	do
	{
		if ( GetAsyncKeyState( VK_END ) )
		{
			if ( bPause )
			{
				bPause = false;
				std::cout << "Unpaused" << std::endl;
			}
			else
			{
				bPause = true;
				std::cout << "Paused" << std::endl;
			}

			Sleep( 100 );
		}

		if ( bPause )
		{
			Sleep( 100 );
			continue;
		}

		if ( needMed )
		{
			std::cout << "Meditating...." << std::endl;

			SendKey( -1, myConfig.GetExtended( ) );
			needMed = false;

			Sleep( ( DWORD ) myConfig.GetMedTime( ) * 1000 );

			curVE = myConfig.GetMaxVE( );

			std::cout << "Resuming...." << std::endl;
		}

		const int maxLoop = myConfig.GetExtended( ) ? _countof( g_Skills ) : _countof( g_Skills ) / BAR_COUNT;

		for ( int i = 0; i < maxLoop; ++i )
		{
			if ( GetAsyncKeyState( VK_END ) )
			{
				std::cout << "Paused" << std::endl;
				bPause = true;
				break;
			}

			if ( g_Skills[ i ].bEnabled == false )
				continue;

			time( &curTime );

			if ( ( difftime( curTime, g_Skills[ i ].lastUsed ) - g_Skills[ i ].cooldown ) < 0 )
				continue;

			SendKey( i, myConfig.GetExtended( ) );

			Sleep( ( DWORD ) ( g_Skills[ i ].castTime * 1000 ) + 1250 );

			time( &g_Skills[ i ].lastUsed );
			curVE -= g_Skills[ i ].VECost;

			std::cout << "VE = " << curVE << std::endl;

			if ( curVE <= myConfig.GetMedVE( ) )
			{
				needMed = true;
				break;
			}
		}

		Sleep( 250 );
	} while ( true );

	std::getchar( );
}