//-----------------------------------------------------------------------------------------------
// Time.cpp
//	A simple high-precision time utility function for Windows
//	based on code by Squirrel Eiserloh

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Time.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


//-----------------------------------------------------------------------------------------------
double InitializeTime( LARGE_INTEGER& out_initialTime )
{
	LARGE_INTEGER countsPerSecond;
	QueryPerformanceFrequency( &countsPerSecond );
	QueryPerformanceCounter( &out_initialTime );
	return( 1.0 / static_cast< double >( countsPerSecond.QuadPart ) );
}


//-----------------------------------------------------------------------------------------------
double GetCurrentTimeSeconds()
{
	static LARGE_INTEGER initialTime;
	static double secondsPerCount = InitializeTime( initialTime );
	LARGE_INTEGER currentCount;
	QueryPerformanceCounter( &currentCount );
	LONGLONG elapsedCountsSinceInitialTime = currentCount.QuadPart - initialTime.QuadPart;

	double currentSeconds = static_cast< double >( elapsedCountsSinceInitialTime ) * secondsPerCount;
	return currentSeconds;
}
//////////////////////////////////////////////////////////////////////////
#include <time.h>
#pragma  warning(disable : 4996) // Disable "'asctime': This function or variable may be unsafe. Consider using asctime_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details."
const char* GetLocalTime()
{
	time_t rawtime;
	struct tm* info;
	char* buffer;

	time(&rawtime);

	info = localtime(&rawtime);
	buffer = asctime(info);
	buffer[strlen(buffer) - 1] = 0;
	return buffer;
}

