#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <errno.h>

#define CHECK(sts, msg) if (-1 == (sts) && errno != 0) 	\
	{	perror(msg); exit(EXIT_FAILURE); }			\
	else   printf("%s : %d\n", msg, sts)

int main (void)
{
    struct sched_param param;
    int my_pid = 0;
    int old_policy, old_priority;
    int status;
    int low_priority, high_priority;

    system ("clear;ps -l");
    //status = nice (+10);
    //CHECK(status, "nice(+10)");
    //system ("ps -l");

    high_priority = sched_get_priority_max(SCHED_FIFO);
    CHECK(high_priority,"sched_get_priority_max FIFO");

    low_priority = sched_get_priority_min(SCHED_FIFO);
    CHECK(low_priority,"sched_get_priority_min FIFO");

    old_policy = sched_getscheduler(my_pid);
    CHECK(old_policy,"sched_getscheduler");

    status = sched_getparam(my_pid, &param);
    CHECK(status,"sched_getparam");

    old_priority = param.sched_priority;

    param.sched_priority = high_priority;
    status = sched_setscheduler(my_pid, SCHED_FIFO, &param);
    CHECK(status,"sched_setscheduler");

    param.sched_priority = low_priority;
    status = sched_setparam(my_pid, &param);
    CHECK(status,"sched_setparam");

    param.sched_priority = old_priority;
    status = sched_setscheduler(my_pid, old_policy, &param);
    CHECK(status,"sched_setscheduler 2");

    status = nice (-11);
    CHECK(status, "nice(-11)");
    system ("ps -l");

    exit(EXIT_SUCCESS);
}

