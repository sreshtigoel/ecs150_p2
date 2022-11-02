#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100
struct sigaction old_action; // to save old action of SIGVTALRM
struct itimerval old_itimerval;

void alarm_handler(int signum) {
	(void) signum; // non-harmful code. to suppress compiler warning
	uthread_yield();
}

void preempt_disable(void)
{
	sigset_t ss;
	sigemptyset(&ss);
	sigaddset(&ss, SIGVTALRM);
	sigprocmask(SIG_BLOCK, &ss, NULL);
}

void preempt_enable(void)
{
	sigset_t ss;
	sigemptyset(&ss);
	sigaddset(&ss, SIGVTALRM);
	sigprocmask(SIG_UNBLOCK, &ss, NULL);
}

void preempt_start(bool preempt)
{
	if (preempt)
	{
		struct sigaction sa;
		sa.sa_handler = alarm_handler;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = 0;

		// change action of signal, and save old action
		sigaction(SIGVTALRM, &sa, &old_action); 
		
		// alarm config
		struct itimerval new_itimerval;
		new_itimerval.it_interval.tv_sec = 0;
		new_itimerval.it_interval.tv_usec = 100 * HZ; // 100 * HZ == 10 msec
		new_itimerval.it_value.tv_sec = 0;
		new_itimerval.it_value.tv_usec = 100 * HZ;
		setitimer(ITIMER_VIRTUAL, &new_itimerval, &old_itimerval);

	}
}

void preempt_stop(void)
{
	//restore configuration
	sigaction(SIGVTALRM, &old_action, NULL);
	setitimer(ITIMER_VIRTUAL, &old_itimerval, NULL);

}

