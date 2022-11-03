#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

queue_t ready_queue;
queue_t exited_queue;

enum thread_state { 
	ready, 
	running,
	blocked,
	exited
};

struct uthread_tcb {
	enum thread_state state;
	void *stack_pointer;
	uthread_ctx_t* ctx;
};

struct uthread_tcb* idle_thread;
struct uthread_tcb* current_thread;

/*   ===========================================================================================  */


int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	if (preempt)
		preempt_start(true);

	ready_queue = queue_create();
	exited_queue = queue_create();

	idle_thread = malloc(sizeof(struct uthread_tcb));
	idle_thread->ctx = malloc(sizeof(uthread_ctx_t));

	if(!ready_queue || !exited_queue || !idle_thread || !idle_thread->ctx)
		return -1;
	
	idle_thread->state = running;
	
	current_thread = idle_thread;

	if(uthread_create(func, arg)) //anything other than 0
		return -1;

	while(1)
	{
		
		while (queue_length(exited_queue) > 0) //destroy threads that reach completion 
		{
			void* garbage;
			queue_dequeue(exited_queue, (void**) &garbage);
			free(garbage);
		}
		
		if(queue_length(ready_queue) == 0)
			break;
		uthread_yield();
	}

	if (preempt) // reset preempt
		preempt_stop();

	return 0;
}

/* ===========================================================================================   */

int uthread_create(uthread_func_t func, void *arg)
{
	struct uthread_tcb* tcb = malloc(sizeof(struct uthread_tcb));
	if (tcb == NULL)
		return -1;

	tcb->stack_pointer = uthread_ctx_alloc_stack();
	tcb->state = ready;
	tcb->ctx = malloc(sizeof(uthread_ctx_t));
	int init_return_value = uthread_ctx_init(tcb->ctx,  tcb->stack_pointer, func, arg);

	if(init_return_value || !tcb->stack_pointer)
		return -1;

	queue_enqueue(ready_queue, tcb);//enqueue the tcb into the ready queue
	return 0;
}


/* ===========================================================================================  */


void uthread_yield(void)
{
	preempt_disable(); // below is critical section

	queue_enqueue(ready_queue, current_thread); // add current thread to end of queue

	struct uthread_tcb* next;
	queue_dequeue(ready_queue, (void**)&next); //pop the next to-run thread

	struct uthread_tcb* curr = current_thread;
	
	if (curr != next)
	{
		current_thread->state = ready; //state change for context switch
		next->state = running;

		current_thread = next;
		uthread_ctx_switch(curr->ctx, next->ctx);
	}
	preempt_enable();
	
}


/*   =========================================================================================== */

void uthread_exit(void)
{
	current_thread->state = exited;
	uthread_ctx_destroy_stack(current_thread->stack_pointer);
	queue_enqueue(exited_queue, current_thread);

	struct uthread_tcb* next;
	queue_dequeue(ready_queue, (void**)&next); //next will have the dequeued thread
	struct uthread_tcb* curr = current_thread;

	current_thread = next;
	current_thread->state = running;
	uthread_ctx_switch(curr->ctx, next->ctx);

}


struct uthread_tcb *uthread_current(void)
{
	return current_thread;
}

void uthread_block(void)
{
	current_thread->state = blocked;

	struct uthread_tcb* next;
	queue_dequeue(ready_queue, (void**)&next); //pop the next to-run thread
	struct uthread_tcb* curr = current_thread;

	if (current_thread != next)
	{
		current_thread = next;
		uthread_ctx_switch(curr->ctx, next->ctx);
	}
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	uthread->state = ready;
	queue_enqueue(ready_queue, uthread);

	uthread_yield();
}
