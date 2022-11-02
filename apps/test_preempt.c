#include <stdio.h>
#include <stdlib.h>
#include<signal.h>

#include "uthread.h"
#include "private.h"


void boo(void* arg)
{
    (void) arg;
    printf("context switch happened\n");
}

void voo(void* arg)
{
    (void) arg;
    printf("You switched to me as well\n");
    raise(SIGINT);
}

void foo(void* arg)
{
    (void) arg;
    uthread_create(boo, NULL);
    uthread_create(voo, NULL);
    while (1) {}  // if program doesn't terminate due to SIGINT, then it means there is error in our implmentation
}



int main(void)
{
	uthread_run(true, foo, NULL);
	return 0;
}
