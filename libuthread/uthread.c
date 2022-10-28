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
	uthread_ctx_t* cpu_registers;
};

struct uthread_tcb idle_thread;
struct uthread_tcb* current_thread;

struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/4 */
}

void uthread_yield(void)
{
	queue_enqueue(ready_queue, current_thread);

	struct uthread_tcb* next;
	queue_dequeue(ready_queue, (void**)&next); //next will have the dequeued thread
	struct uthread_tcb* curr = current_thread;
	
	if (curr != next)
	{
		current_thread->state = ready; //state change for context switch
		next->state = running;

		current_thread = next;
		uthread_ctx_switch(curr, next);
	}
	
}

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
	uthread_ctx_switch(curr, next);

	assert(0); //should never return
}

int uthread_create(uthread_func_t func, void *arg)
{
	struct uthread_tcb* tcb = malloc(sizeof(struct uthread_tcb));

	tcb->stack_pointer = uthread_ctx_alloc_stack();
	tcb->state = ready;
	int init_return_value = uthread_ctx_init(tcb->cpu_registers,  tcb->stack_pointer, func, arg);

	if(init_return_value || !tcb->stack_pointer)
		return -1;

	queue_enqueue(ready_queue, tcb);//enqueue the tcb into the ready queue
	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	ready_queue = queue_create();
	exited_queue = queue_create();
	struct uthread_tcb* tcb;

	if(!ready_queue || !exited_queue)
		return -1;

	current_thread = &idle_thread;
	int thread_create_return_value = uthread_create(func, arg); //creating new thread

	if(thread_create_return_value) //anything other than 0
		return -1;

	while(1)
	{
		if (!preempt)
		{
			if(queue_length(ready_queue) == 0)
				break;
			uthread_yield();
		}
	}

	return 0;
}

void uthread_block(void)
{
	/* TODO Phase 4 */
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* TODO Phase 4 */
}

