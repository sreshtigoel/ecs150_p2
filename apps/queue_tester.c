#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

struct node {
	struct node* next;
	void* data;
};

struct queue {
	struct node* head;
	struct node* tail;
	int length;
};

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)


static void iterator_inc(queue_t q, void *data)
{
    int *a = (int*)data;

    if (*a == 42)
        queue_delete(q, data);
    else
        *a += 1;
}

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* ==================================================================   */
/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

/* ==================================================================   */

void test_queue_destroy(void) 
{
	fprintf(stderr, "*** TEST queue_destroy empty ***\n");
	queue_t q;
	q = queue_create();
	TEST_ASSERT(queue_destroy(q) == 0);


    fprintf(stderr, "*** TEST queue_destroy unempty ***\n");
	queue_t q2;
	int data = 17; // some random data
	q2 = queue_create();
	queue_enqueue(q2, &data);
	TEST_ASSERT(queue_destroy(q2) == -1);
}

/* ==================================================================   */

void enqueue_and_delete_test(void)
{
    int data = 2;
	queue_t q;

	fprintf(stderr, "*** TEST queue_enqueue ***\n");

	q = queue_create();
	TEST_ASSERT(queue_enqueue(q, &data) == 0);
    TEST_ASSERT(queue_enqueue(q, NULL) == -1);
    TEST_ASSERT(queue_enqueue(NULL, &data) == -1);

    int another_data = 4;
    queue_enqueue( q, &another_data);
    int data_3 = 8;
    queue_enqueue( q, &data_3);

    TEST_ASSERT(q->head->data == &data);
    TEST_ASSERT(q->head->next->data == &another_data);
    TEST_ASSERT(q->tail->data == &data_3);
    TEST_ASSERT(q->length == 3);

    fprintf(stderr, "*** TEST queue_delete ***\n");
    int non_exist = 16; 
    TEST_ASSERT(queue_delete(q, &non_exist) == -1);
    TEST_ASSERT(q->head->data == &data);
    TEST_ASSERT(queue_delete(q, &data) == 0);
    TEST_ASSERT(q->head->data == &another_data);
    TEST_ASSERT(queue_delete(q, &another_data) == 0);
    TEST_ASSERT(q->head->data == &data_3);
    TEST_ASSERT(queue_delete(q, NULL) == -1);
    TEST_ASSERT(q->head->data == &data_3);
    TEST_ASSERT(queue_delete(q, &data_3) == 0);
    TEST_ASSERT(q->length == 0);
    TEST_ASSERT(queue_destroy(q) == 0);
}

/* ==================================================================   */

void test_queue_iterate(void)
{
    fprintf(stderr, "*** TEST queue_iterate ***\n");
    queue_t q;
    int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
    size_t i;

    /* Initialize the queue and enqueue items */
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    /* Increment every item of the queue, delete item '42' */
    queue_iterate(q, iterator_inc);
    TEST_ASSERT(data[0] == 2);
    TEST_ASSERT(queue_length(q) == 9);
}


/* ==== MAIN  Funciton ===============================================   */


int main(void)
{
	test_create();
	test_queue_simple();
    test_queue_destroy();
    enqueue_and_delete_test();
    test_queue_iterate();

	return 0;
}
