#ifndef DEQUE_H
#define DEQUE_H

#include <stdlib.h>

typedef float data;

typedef struct s_deque t_deque;

// Prototypes
t_deque * deque_create(void);
int deque_delete(t_deque * deque, void (*destructor)(data));
int deque_reverse(t_deque * deque, t_deque * out);
int deque_copy(t_deque * deque, t_deque * out);
int deque_push_front(t_deque * deque, data value);
int deque_peek_front(t_deque * deque, data * out_data);
int deque_pop_front(t_deque * deque, data * out_data);
int deque_push_back(t_deque * deque, data value);
int deque_peek_back(t_deque * deque, data * out_data);
int deque_pop_back(t_deque * deque, data * out_data);
int deque_clear(t_deque * deque);
int deque_get_size(t_deque * deque, size_t * out_size);
int deque_is_empty(t_deque * deque);

#endif