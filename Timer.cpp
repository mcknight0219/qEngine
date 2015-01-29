#include "Timer.h"
#include "qEngine.h"

extern qEngine * engine;

void Timer::Pause()
{
	paused = true;
	timediff = 0;
}

void Timer::Resume()
{
	if( !paused )
		return;

	paused = false;
	lastTime = GetSysMilliseconds();
	currentTime = lastTime;
}

void Timer::Tick()
{
	if( paused )
		return;

	lastTime = currentTime;
	currentTime = GetSysMilliseconds();

	extraPrecision += 0.6666667f;
	timediff = 16 + (int)extraPrecision;
	extraPrecision -= timediff - 16;

	simulationTime += timediff;
}

void Timer::Reset()
{
	Tick();
	simulationTime = 0;
	extraPrecision = 0;
	timediff = 0;
}


#ifdef _WIN32
#include <Windows.h>
#include <MMSystem.h>
int Timer::GetSysMilliseconds()
{
	return (int)timeGetTime();
}

#elif __linux__
#include <sys/time.h>
int Timer::GetSysMilliseconds()
{
	struct timeval tp;
	static int secbase;

	gettimeofday(&tp, 0);

	if( !secbase )
	{
		secbase = tp.tv_sec;
		return tp.tv_usec / 1000;
	}

	return (tp.tv_sec - secbase) * 1000 + tp.tv_usec / 1000;
}

#endif