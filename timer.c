#include <SDL/SDL_timer.h>
#include "timer.h"
//Note. Timers go beef up if program runs for more than 49 days. WONTFIX
void timer_init(Timer * timer)
{
	timer->nowtime = SDL_GetTicks();
	timer->started = timer->nowtime / 1000.0;
	timer_update(timer);
	return;
}

void timer_update(Timer * timer)
{
	timer->thentime = timer->nowtime;
	timer->nowtime = (double) SDL_GetTicks();
	timer->deltatime = (timer->nowtime - timer->thentime) / 1000.0;
}

double timer_delta(const Timer * timer)
{
	return (double) SDL_GetTicks() / 1000.0 - timer->nowtime;
}

double timer_since_started(const Timer * timer)
{
	return SDL_GetTicks() / 1000.0 - timer->started;
}

double timer_last_delta(const Timer * timer)
{
	return timer->deltatime;
}

//Made by Nils O. Selåsdal
