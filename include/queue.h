#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>

typedef float t_data;

typedef struct s_queue t_queue;

// Prototypes
t_queue * queue_create(void);
int queue_delete(t_queue * queue, void (*destructor)(t_data));
int queue_reverse(t_queue * queue, t_queue * out);
int queue_copy(t_queue * queue, t_queue * out);
int queue_push(t_queue * queue, t_data value);
int queue_peek(t_queue * queue, t_data * out_t_data);
int queue_pop(t_queue * queue, t_data * out_t_data);
int queue_clear(t_queue * queue);
int queue_get_size(t_queue * queue, size_t * out_size);
int queue_is_empty(t_queue * queue);

#endif