#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"
#include "queue.h"
#include "uthread.h"

struct semaphore {
	size_t count;
	queue_t blocked;
	struct uthread_tcb* thread_tcb;
};

sem_t sem_create(size_t count)
{
	struct semaphore* sem = malloc(sizeof(struct semaphore));
	sem->count = count;
	sem->blocked = queue_create();

	if(!sem || !sem->blocked)
		return NULL;

	return sem;
}

int sem_destroy(sem_t sem)
{
	if(!sem || queue_length(sem->blocked) != 0)
		return -1;

	free(sem);
	
	return 0;
}

int sem_down(sem_t sem)
{
	if (!sem)
		return NULL;
	
	while (sem->count == 0)
	{
		//sem->thread_tcb->state = blocked;
		queue_enqueue(sem->blocked, &sem->thread_tcb);
	}
	sem->count--;

	return 0;
}

int sem_up(sem_t sem)
{
	if (!sem)
		return -1;

	sem->count++;

	if(queue_length(sem->blocked) != 0)
	{
		uthread_tcb* run_thread;
		queue_dequeue(sem->blocked, &run_thread);
		run_thread->state = ready;
		queue_enqueue(ready_queue, run_thread);
	}

	return 0;
}