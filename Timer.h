#ifndef _TIMER_H
#define _TIMER_H

/* Timer, timer, timer! Full of tricks and subtles. So
   for the time being, I just copied the implementation from
   Shmup which has a lucid and simple timer
*/
class Timer
{
public:
			Timer();
	void	Pause();
	void	Resume();
	void	Tick();
	int		GetOneTick() const { return timediff; }
	void	Reset();

private:
	int		GetSysMilliseconds();

private:
	int		timediff;
	int		simulationTime;
	int		currentTime;
	int		lastTime;
	int		fps;

	float	extraPrecision;

	bool	paused;
};

inline Timer::Timer() : timediff(0), simulationTime(0), currentTime(0),
lastTime(0), fps(0), extraPrecision(0), paused(false)
{

}


#endif /* !_TIMER_H */