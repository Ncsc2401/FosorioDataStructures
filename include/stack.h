#ifndef STACK_H
#define STACK_H

#include <stdlib.h>

typedef float data;

typedef struct s_stack t_stack;

// Prototypes
t_stack * stack_create(void);
int stack_delete(t_stack * stack, void (*destructor)(data));
int stack_reverse(t_stack * stack, t_stack * out);
int stack_copy(t_stack * stack, t_stack * out);
int stack_push(t_stack * stack, data value);
int stack_peek(t_stack * stack, data * out_data);
int stack_pop(t_stack * stack, data * out_data);
int stack_clear(t_stack * stack);
int stack_get_size(t_stack * stack, size_t * out_size);
int stack_is_empty(t_stack * stack);

#endif