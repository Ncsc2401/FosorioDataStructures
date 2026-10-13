#include <stdio.h>
#include "deque.h"

#define ERROR 1
#define SUCCESS 0

typedef struct s_node {
    t_data value;
    struct s_node * next;
    struct s_node * previous;
} t_node;

typedef struct s_deque{
    size_t size;
    t_node * front;
    t_node * back;
} t_deque;

// Prototypes
t_deque * deque_create(void);
int deque_delete(t_deque * deque, void (*destructor)(t_data));
int deque_reverse(t_deque * deque, t_deque * out);
int deque_copy(t_deque * deque, t_deque * out);
int deque_push_front(t_deque * deque, t_data value);
int deque_peek_front(t_deque * deque, t_data * out_t_data);
int deque_pop_front(t_deque * deque, t_data * out_t_data);
int deque_push_back(t_deque * deque, t_data value);
int deque_peek_back(t_deque * deque, t_data * out_t_data);
int deque_pop_back(t_deque * deque, t_data * out_t_data);
int deque_clear(t_deque * deque);
int deque_get_size(t_deque * deque, size_t * out_size);
int deque_is_empty(t_deque * deque);

// Helper prototypes
static int deque_foreach(t_deque * deque, void (*function)(t_data));
static t_node * node_create(t_data value, t_node * previous, t_node * next);

/* 
    Creates a new deque 
    Performance O(1)
*/
t_deque * deque_create(void){
    t_deque * deque = malloc(sizeof(*deque));

    if(deque == NULL){
        perror("Couldn't allocate deque");
        return NULL;
    }

    (deque -> size) = 0;
    (deque -> front) = NULL;
    (deque -> back) = NULL;

    return deque;
}

/*
    Reverses a deque and appends it to out
    Performance O(n)
*/
int deque_reverse(t_deque * deque, t_deque * out){
    if(deque == NULL){
        return ERROR;
    }

    if(out == NULL){
        return ERROR;
    }

    t_node * node = (deque -> back);
    while(node != NULL){
        if(deque_push_front(out, (node -> value))){
            return ERROR;
        }

        node = (node -> previous);
    }

    return SUCCESS;
}

/*
    Copies a deque and appends it to out
    Performance O(n)
*/
int deque_copy(t_deque * deque, t_deque * out){
    if(deque == NULL){
        return ERROR;
    }

    if(out == NULL){
        return ERROR;
    }

    t_node * node = (deque -> front);
    while(node != NULL){
        if(deque_push_front(deque, (node -> value))){
            return ERROR;
        }

        node = (node -> next);
    }

    return SUCCESS;
}

/* 
    Deletes a deque
    Performance O(n)
*/
int deque_delete(t_deque * deque, void (*destructor)(t_data)){
    if(deque == NULL){
        return ERROR;
    }

    if(destructor != NULL){
        if(deque_foreach(deque, destructor)){
            return ERROR;
        }
    }

    if(deque_clear(deque)){
        return ERROR;
    }

    free(deque);

    return SUCCESS;
}

/*
    Pushes a new value to the front of a deque
    Performance O(1)
*/
int deque_push_front(t_deque * deque, t_data value){
    if(deque == NULL){
        return ERROR;
    }

    t_node * new_node = node_create(value, NULL, (deque -> front));
    if(new_node == NULL){
        return ERROR;
    }

    if((deque -> front) == NULL){
        (deque -> front) = new_node;
        (deque -> back) = new_node;
    }
    else{
        (deque -> front -> previous) = new_node; 
        (deque -> front) = new_node;
    }
    
    (deque -> size)++;
    
    return SUCCESS;
}

/*
    Pushes a new value to the back of a deque
    Performance O(1)
*/
int deque_push_back(t_deque * deque, t_data value){
    if(deque == NULL){
        return ERROR;
    }

    t_node * new_node = node_create(value, (deque -> back), NULL);
    if(new_node == NULL){
        return ERROR;
    }

    if((deque -> front) == NULL){
        (deque -> front) = new_node;
        (deque -> back) = new_node;
    }
    else{
        (deque -> back -> next) = new_node; 
        (deque -> back) = new_node;
    }
    
    (deque -> size)++;
    
    return SUCCESS;
}

/*
    Peeks the value at the front of the deque
    Performance O(1)
*/
int deque_peek_front(t_deque * deque, t_data * out_t_data){
    if(deque == NULL){
        return ERROR;
    }

    if((deque -> front) == NULL){
        return ERROR;
    }

    if(out_t_data != NULL){
        *out_t_data = (deque -> front -> value);
    }

    return SUCCESS;
}

/*
    Peeks the value at the back of the deque
    Performance O(1)
*/
int deque_peek_back(t_deque * deque, t_data * out_t_data){
    if(deque == NULL){
        return ERROR;
    }

    if((deque -> back) == NULL){
        return ERROR;
    }

    if(out_t_data != NULL){
        *out_t_data = (deque -> back -> value);
    }

    return SUCCESS;
}

/*
    Gets a value and removes it from the front of a deque
    Performance O(1)
*/
int deque_pop_front(t_deque * deque, t_data * out_t_data){
    if(deque == NULL){
        return ERROR;
    }

    t_node * front = (deque -> front);
    if((deque -> front) == NULL){
        return ERROR;
    }

    if((front -> next) == NULL){
        (deque -> back) = NULL;
    }
    else{
        (front -> next -> previous) = NULL;
    }

    (deque -> front) = (front -> next);
    (deque -> size)--;

    if(out_t_data != NULL){
        *out_t_data = (front -> value);
    }
    free(front);

    return SUCCESS;
}

/*
    Gets a value and removes it from the back of a deque
    Performance O(1)
*/
int deque_pop_back(t_deque * deque, t_data * out_t_data){
    if(deque == NULL){
        return ERROR;
    }

    t_node * back = (deque -> back);
    if((deque -> back) == NULL){
        return ERROR;
    }

    if((back -> previous) == NULL){
        (deque -> front) = NULL;
    }
    else{
        (back -> previous -> next) = NULL;
    }

    (deque -> back) = (back -> previous);
    (deque -> size)--;

    if(out_t_data != NULL){
        *out_t_data = (back -> value);
    }
    free(back);

    return SUCCESS;
}

/*
    Clears all values in a deque
    Performance O(n)
*/
int deque_clear(t_deque * deque){
    if(deque == NULL){
        return ERROR;
    }

    t_node * node = (deque -> front);
    t_node * previous = NULL;

    while(node != NULL){
        previous = node;
        node = (node -> next);

        free(previous);
    }

    (deque -> front) = NULL;
    (deque -> back) = NULL;
    (deque -> size) = 0;

    return SUCCESS;
}

/*
    Gets the size of a deque
    Performance O(1)
*/
int deque_get_size(t_deque * deque, size_t * out_size){
    if(deque == NULL){
        return ERROR;
    }

    if(out_size != NULL){
        *out_size = (deque -> size);
    }

    return SUCCESS;
}

/*
    Returns if a deque is empty
    Performance O(1)
*/
int deque_is_empty(t_deque * deque){
    if(deque == NULL){
        return ERROR;
    }

    return (deque -> size) == 0;
}


// HELPER FUNCTIONS

static int deque_foreach(t_deque * deque, void (*function)(t_data)){
    if(deque == NULL){
        return ERROR;
    }

    if(function == NULL){
        return ERROR;
    }

    t_node * node = (deque -> front);
    while(node != NULL){
        function(node -> value);

        node = (node -> next);
    }

    return SUCCESS;
}


static t_node * node_create(t_data value, t_node * previous, t_node * next){
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

