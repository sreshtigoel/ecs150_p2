#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"
#include "uthread.h"

struct semaphore {
	size_t count;
	queue_t blocked_queue;
};
struct uthread_tcb* calling_thread;

sem_t sem_create(size_t count)
{
	struct semaphore* sem = malloc(sizeof(struct semaphore));
	sem->count = count;
	sem->blocked_queue = queue_create();

	if(!sem || !sem->blocked_queue)
		return NULL;

	return sem;
}

int sem_destroy(sem_t sem)
{
	if(!sem || queue_length(sem->blocked_queue) != 0)
		return -1;

	queue_destroy(sem->blocked_queue);
	free(sem);

	return 0;
}

int sem_down(sem_t sem)
{
	if (!sem)
		return -1;
	
	if (sem->count == 0)
	{
		calling_thread = uthread_current();
		queue_enqueue(sem->blocked_queue, calling_thread);
		uthread_block(); //change the state of the current thread to blocked
	}
	sem->count--;

	return 0;
}

int sem_up(sem_t sem)
{
	if (!sem)
		return -1;

	sem->count++;

	if(queue_length(sem->blocked_queue) != 0)
	{
		struct uthread_tcb* unblock_thread;
		queue_dequeue(sem->blocked_queue, (void**)&unblock_thread);
		uthread_unblock(unblock_thread); //change state of thread to ready and enqueue to ready queue
	}

	return 0;
}