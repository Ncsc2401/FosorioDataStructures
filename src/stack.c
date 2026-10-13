#include <stdio.h>

#include "stack.h"

#define ERROR 1
#define SUCCESS 0

typedef struct s_node {
    t_data value;
    struct s_node * next;
} t_node;

typedef struct s_stack{
    size_t size;
    t_node * top;
} t_stack;

// Prototypes
t_stack * stack_create(void);
int stack_delete(t_stack * stack, void (*destructor)(t_data));
int stack_reverse(t_stack * stack, t_stack * out);
int stack_copy(t_stack * stack, t_stack * out);
int stack_push(t_stack * stack, t_data value);
int stack_peek(t_stack * stack, t_data * out_t_data);
int stack_pop(t_stack * stack, t_data * out_t_data);
int stack_clear(t_stack * stack);
int stack_get_size(t_stack * stack, size_t * out_size);
int stack_is_empty(t_stack * stack);

// Helper prototypes
static int stack_foreach(t_stack * stack, void (*function)(t_data));
static t_node * node_create(t_data value, t_node * next);

/* 
    Creates a new stack 
    Performance O(1)
*/
t_stack * stack_create(void){
    t_stack * stack = malloc(sizeof(*stack));

    if(stack == NULL){
        perror("Couldn't allocate stack");
        return NULL;
    }

    (stack -> size) = 0;
    (stack -> top) = NULL;

    return stack;
}

/*
    Reverses a stack and appends it to out
    Performance O(n)
*/
int stack_reverse(t_stack * stack, t_stack * out){
    if(stack == NULL){
        return ERROR;
    }

    if(out == NULL){
        return ERROR;
    }

    t_node * node = (stack -> top);
    while(node != NULL){
        if(stack_push(out, node -> value)){
            return ERROR;
        }

        node = (node -> next);
    }

    return SUCCESS;
}

/*
    Copies a stack and appends it to out
    Performance O(n²)
*/
int stack_copy(t_stack * stack, t_stack * out){
    if(stack == NULL){
        return ERROR;
    }

    if(out == NULL){
        return ERROR;
    }

    t_stack * temp = stack_create();
    if(temp == NULL){
        return ERROR;
    }

    if(stack_reverse(stack, temp)){
        stack_delete(temp, NULL);
        return ERROR;
    }

    if(stack_reverse(temp, out)){
        stack_delete(temp, NULL);
        return ERROR;
    }

    stack_delete(temp, NULL);

    return SUCCESS;
}

/* 
    Deletes a stack
    Performance O(n)
*/
int stack_delete(t_stack * stack, void (*destructor)(t_data)){
    if(stack == NULL){
        return ERROR;
    }

    if(destructor != NULL){
        if(stack_foreach(stack, destructor)){
            return ERROR;
        }
    }

    if(stack_clear(stack)){
        return ERROR;
    }
    

    free(stack);

    return SUCCESS;
}

/*
    Pushes a new value to a stack
    Performance O(1)
*/
int stack_push(t_stack * stack, t_data value){
    if(stack == NULL){
        return ERROR;
    }

    t_node * new_node = node_create(value, (stack -> top));
    if(new_node == NULL){
        return ERROR;
    }

    (stack -> top) = new_node;
    (stack -> size)++;
    
    return SUCCESS;
}

/*
    Gets a value from the top of the tacl
    Performance O(1)
*/
int stack_peek(t_stack * stack, t_data * out_t_data){
    if(stack == NULL){
        return ERROR;
    }

    if((stack -> top) == NULL){
        return ERROR;
    }

    if(out_t_data != NULL){
        *out_t_data = (stack -> top -> value);
    }

    return SUCCESS;
}

/*
    Gets a value and removes it from a stack
    Performance O(1)
*/
int stack_pop(t_stack * stack, t_data * out_t_data){
    if(stack == NULL){
        return ERROR;
    }

    t_node * top = (stack -> top);
    if((stack -> top) == NULL){
        return ERROR;
    }

    (stack -> top) = (top -> next);
    (stack -> size)--;

    if(out_t_data != NULL){
        *out_t_data = (top -> value);
    }

    free(top);

    return SUCCESS;
}

/*
    Clears all values in a stack
    Performance O(n)
*/
int stack_clear(t_stack * stack){
    if(stack == NULL){
        return ERROR;
    }

    t_node * node = (stack -> top);
    t_node * next = NULL;

    while(node != NULL){
        next = node;
        node = (node -> next);

        free(next);
    }

    (stack -> top) = NULL;
    (stack -> size) = 0;

    return SUCCESS;int stack_is_empty(t_stack * stack);
}

/*
    Gets the size of a stack
    Performance O(1)
*/
int stack_get_size(t_stack * stack, size_t * out_size){
    if(stack == NULL){
        return ERROR;
    }

    if(out_size != NULL){
        *out_size = (stack -> size);
    }

    return SUCCESS;
}

/*
    Returns if a stack is empty
    Performance O(1)
*/
int stack_is_empty(t_stack * stack){
    if(stack == NULL){
        return ERROR;
    }

    return (stack -> size) == 0;
}

// HELPER FUNCTIONS

static int stack_foreach(t_stack * stack, void (*function)(t_data)){
    if(stack == NULL){
        return ERROR;
    }

    if(function == NULL){
        return ERROR;
    }

    t_node * node = (stack -> top);
    while(node != NULL){
        function(node -> value);

        node = (node -> next);
    }

    return SUCCESS;
}

static t_node * node_create(t_data value, t_node * next){
    t_node * node = malloc(sizeof(*node));

    if(node == NULL){
        perror("Couldn't allocate node");
        return NULL;
    }

    (node -> value) = value;
    (node -> next) = next;

    return node;
}

