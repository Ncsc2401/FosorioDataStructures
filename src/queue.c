#include <stdio.h>

#include "queue.h"

#define ERROR 1
#define SUCCESS 0

// (HEAD) Previous -> Node -> Next (TAIL)
typedef struct s_node {
    data value;
    struct s_node * next;
    struct s_node * previous;
} t_node;

typedef struct s_queue{
    size_t size;
    t_node * head;
    t_node * tail;
} t_queue;

// Prototypes
t_queue * queue_create(void);
int queue_delete(t_queue * queue, void (*destructor)(data));
int queue_reverse(t_queue * queue, t_queue * out);
int queue_copy(t_queue * queue, t_queue * out);
int queue_push(t_queue * queue, data value);
int queue_peek(t_queue * queue, data * out_data);
int queue_pop(t_queue * queue, data * out_data);
int queue_clear(t_queue * queue);
int queue_get_size(t_queue * queue, size_t * out_size);
int queue_is_empty(t_queue * queue);

// Helper prototypes
static int queue_foreach(t_queue * queue, void (*function)(data));
static t_node * node_create(data value, t_node * previous, t_node * next);

/* 
    Creates a new queue 
    Performance O(1)
*/
t_queue * queue_create(void){
    t_queue * queue = malloc(sizeof(*queue));

    if(queue == NULL){
        perror("Couldn't allocate queue");
        return NULL;
    }

    (queue -> size) = 0;
    (queue -> head) = NULL;
    (queue -> tail) = NULL;

    return queue;
}

/*
    Reverses a queue and appends it to out
    Performance O(n)
*/
int queue_reverse(t_queue * queue, t_queue * out){
    if(queue == NULL){
        return ERROR;
    }

    if(out == NULL){
        return ERROR;
    }

    t_node * node = (queue -> tail);
    while(node != NULL){
        if(queue_push(out, node -> value)){
            return ERROR;
        }

        node = (node -> previous);
    }

    return SUCCESS;
}

/*
    Copies a queue and appends it to out
    Performance O(n²)
*/
int queue_copy(t_queue * queue, t_queue * out){
    if(queue == NULL){
        return ERROR;
    }

    if(out == NULL){
        return ERROR;
    }

    t_node * node = (queue -> head);
    while(node != NULL){
        if(queue_push(queue, (node -> value))){
            return ERROR;
        }

        node = (node -> next);
    }

    return SUCCESS;
}

/* 
    Deletes a queue
    Performance O(n)
*/
int queue_delete(t_queue * queue, void (*destructor)(data)){
    if(queue == NULL){
        return ERROR;
    }

    if(destructor != NULL){
        if(queue_foreach(queue, destructor)){
            return ERROR;
        }
    }

    if(queue_clear(queue)){
        return ERROR;
    }

    free(queue);

    return SUCCESS;
}

/*
    Pushes a new value to a queue
    Performance O(1)
*/
int queue_push(t_queue * queue, data value){
    if(queue == NULL){
        return ERROR;
    }

    t_node * new_node = node_create(value, (queue -> tail), NULL);
    if(new_node == NULL){
        return ERROR;
    }

    if((queue -> head) == NULL){
        (queue -> head) = new_node;
        (queue -> tail) = new_node;
    }
    else{
        (queue -> tail -> next) = new_node; 
        (queue -> tail) = new_node;
    }
    
    (queue -> size)++;
    
    return SUCCESS;
}

/*
    Peeks the value at head of the queue
    Performance O(1)
*/
int queue_peek(t_queue * queue, data * out_data){
    if(queue == NULL){
        return ERROR;
    }

    if((queue -> head) == NULL){
        return ERROR;
    }

    if(out_data != NULL){
        *out_data = (queue -> head -> value);
    }

    return SUCCESS;
}

/*
    Gets a value and removes it from a queue
    Performance O(1)
*/
int queue_pop(t_queue * queue, data * out_data){
    if(queue == NULL){
        return ERROR;
    }

    t_node * head = (queue -> head);
    if((queue -> head) == NULL){
        return ERROR;
    }

    if((head -> next) == NULL){
        (queue -> tail) = NULL;
    }
    else{
        (head -> next -> previous) = NULL;
    }

    (queue -> head) = (head -> next);
    (queue -> size)--;

    if(out_data != NULL){
        *out_data = (head -> value);
    }
    free(head);

    return SUCCESS;
}

/*
    Clears all values in a queue
    Performance O(n)
*/
int queue_clear(t_queue * queue){
    if(queue == NULL){
        return ERROR;
    }

    t_node * node = (queue -> head);
    t_node * previous = NULL;

    while(node != NULL){
        previous = node;
        node = (node -> next);

        free(previous);
    }

    (queue -> head) = NULL;
    (queue -> tail) = NULL;
    (queue -> size) = 0;

    return SUCCESS;
}

/*
    Gets the size of a queue
    Performance O(1)
*/
int queue_get_size(t_queue * queue, size_t * out_size){
    if(queue == NULL){
        return ERROR;
    }

    if(out_size != NULL){
        *out_size = (queue -> size);
    }

    return SUCCESS;
}

/*
    Returns if a queue is empty
    Performance O(1)
*/
int queue_is_empty(t_queue * queue){
    if(queue == NULL){
        return ERROR;
    }

    return (queue -> size) == 0;
}

// HELPER FUNCTIONS

static int queue_foreach(t_queue * queue, void (*function)(data)){
    if(queue == NULL){
        return ERROR;
    }

    if(function == NULL){
        return ERROR;
    }

    t_node * node = (queue -> head);
    while(node != NULL){
        function(node -> value);

        node = (node -> next);
    }

    return SUCCESS;
}


static t_node * node_create(data value, t_node * previous, t_node * next){
    t_node * node = malloc(sizeof(*node));

    if(node == NULL){
        perror("Couldn't allocate node");
        return NULL;
    }

    (node -> value) = value;
    (node -> previous) = previous;
    (node -> next) = next;

    return node;
}

