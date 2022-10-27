#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

struct node {
	struct node* next;
	void* data;
};

struct queue {
	struct node* head;
	struct node* tail;
	int length;
};

queue_t queue_create(void)
{
	queue_t queue = malloc(sizeof(struct queue));

	if(!queue) //if case of failure, return null
		return NULL;

	queue->head = NULL;
	queue->tail = NULL;
	queue->length = 0;
	return queue;
}

int queue_destroy(queue_t queue)
{
	if(!queue || queue->length != 0) //in case of error or if queue is not empty
		return -1;
	
	free(queue);
	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	struct node* new_node = malloc(sizeof(struct node));
	if(!queue || !data || !new_node) //if queue or data is null or memory allocation error
		return -1;
	
	new_node->data = data;
	new_node->next = NULL;

	if(queue->length == 0) //first element of the queue
	{
		queue->head = new_node;
		queue->tail = new_node;
	}
	else
	{
		queue->tail->next = new_node;
		queue->tail = new_node;
	}
	queue->length++;
	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	if(!queue || !data || queue->length == 0)
		return -1;
	
	*data = queue->head->data;
	queue->head = queue->head->next;
	queue->length--;
	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	if(!queue || !data)
		return -1;

	if (queue->head->data == data) //if the node to be deleted is the first element
	{
		queue->head = queue->head->next;
		queue->length--;
		return 0;
	}

	struct node* counter  = queue->head;
	struct node* previous = queue->head;

	while(counter != NULL)
	{

		if(counter->data == data)
		{
			previous->next = counter->next;
			queue->length--;
			return 0;
		}
		previous = counter;
		counter = counter->next;
	}

	return -1;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	if(!queue || !func)
		return -1;

	struct node* counter  = queue->head;

	while(counter != NULL)
	{
		func(queue, counter->data);
		counter = counter->next;
	}
	
	return 0;
}

int queue_length(queue_t queue)
{
	if(!queue)
		return -1;
	
	return queue->length;
}

