#ifndef  __TIMER_H__
#define  __TIMER_H__

typedef struct Timer Timer;
struct Timer {
	double started;
	double nowtime;
	double thentime;
	double deltatime;

};

void timer_init(Timer * timer);

void timer_update(Timer * timer);

double timer_delta(const Timer * timer);
double timer_since_started(const Timer * timer);
double timer_last_delta(const Timer * timer);
#endif
