## Phase 1: Implementing a Queue
The queue is FIFO (First-In-First-Out) order data structure. To implmenet the queue, we used two structs, one struct was used to refer each individual items of the queue, and another struct called queue to keep track of length, head, and the tail of the queue. queue_create() function initializes the queue by allocating dynamic memory. new element can be added to the queue by calling queue_enqueue() function. elements can be removed either by calling the dequeue function, which removes the oldest item in the queue, or by calling queue_delete() function and supplying it with the item to be deleted. When done with queue, we can call the destroy function which takes care of freeing all dynamic memory that was allocated.
we created a tester file (queue_tester.c) which included multiple test cases. The code passed all test cases.


## Phase 2: Implmenting Uthread API (ie User-level threads)
The thread control block (TCB)  struct helps us to keep of each thread. it has 3 elements; one to keep track of the state of the thread, another used as pointer to the stack of thread, and the last element used to store context of the thread. uthread_run() function checks whether preemption is set to true. if it’s, then it enables preemtion by calling preempt_start(true). then we created two queues (utilizing the functions from phase 1). one queue (ready_queue) used to keep track of all non-finished threads, and another queue (exited_queue) to keep track of all finished ones (ie zombie threads). Next we create a new thread and add it read_queue. Then thread that called uthread_run() function then becomes an idle thread, always yielding to other threads, unless if the are no more threads left in ready_queue then it we return and finish utilizing the uthread library. We also destroy take of destroying all finished threads in this function. Uthread_yeild function is responsible of initiating context switch, but before doing so, we disable preemption (ensuring that no other thread will take the processor from this thread), since context switching operation is considered a critical section. after disabling preemption, we add the current thread to the end of ready_queue, then pop the next thread and change it’s state to running, after that we do context switch by calling uthread_ctx_switch. when done with all these, we re-enable preemption. uthread_exit takes care of destroying the finished threads stacks, then adding those threads to exited_queue, then switching context to the next thread in ready_queue.