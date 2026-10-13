#include <stdio.h>

#include "circular_linked_list.h"

#define ERROR 1
#define SUCCESS 0

#define TRUE 1
#define FALSE 0

typedef struct s_node {
    t_data value;
    struct s_node * previous;
    struct s_node * next;
} t_node;

typedef struct s_circular_linked_list{
    size_t size;
    t_node * origin;
} t_circular_linked_list;

// Prototypes
t_circular_linked_list * circular_linked_list_create(void);
int circular_linked_list_copy(t_circular_linked_list * list, t_circular_linked_list * out);
int circular_linked_list_delete(t_circular_linked_list * list, void (*destructor)(t_data));
int circular_linked_list_append(t_circular_linked_list * list, t_data value);
int circular_linked_list_insert_after(t_circular_linked_list * list, size_t index, t_data value);
int circular_linked_list_insert_before(t_circular_linked_list * list, size_t index, t_data value);
int circular_linked_list_get(t_circular_linked_list * list, size_t index, t_data * out_t_data);
int circular_linked_list_rotate(t_circular_linked_list * list, long offset);
int circular_linked_list_pop(t_circular_linked_list * list, size_t index, t_data * out_t_data);
int circular_linked_list_remove_at(t_circular_linked_list * list, size_t index);
int circular_linked_list_has(t_circular_linked_list * list, t_data value, int (*equals_function)(t_data, t_data));
int circular_linked_list_clear(t_circular_linked_list * list);
int circular_linked_list_get_size(t_circular_linked_list * list, size_t * out_size);
int circular_linked_list_foreach(t_circular_linked_list * list, void (*function)(t_data));
int circular_linked_list_map(t_circular_linked_list * list, t_data (*function)(t_data));
int circular_linked_list_is_empty(t_circular_linked_list * list);
int circular_linked_list_filter(t_circular_linked_list * list, int (*function)(t_data), t_circular_linked_list * out);
int circular_linked_list_sort(t_circular_linked_list * list, int (*compare_function)(t_data, t_data));

// Helper prototypes
static int node_find_from(t_node * start_node, t_data value, int (*equals_function)(t_data, t_data), t_node ** out_node, size_t * out_offset);
static int node_get(t_circular_linked_list * list, size_t index, t_node ** out_node);
static t_node * node_create(t_data value, t_node * previous, t_node * next);
static t_node * node_create_origin(t_data value);

/* 
    Creates a new circular linked list 
    Performance O(1)
*/
t_circular_linked_list * circular_linked_list_create(void){
    t_circular_linked_list * list = malloc(sizeof(*list));

    if(list == NULL){
        perror("Couldn't allocate circular linked list");
        return NULL;
    }

    (list -> size) = 0;
    (list -> origin) = NULL;

    return list;
}

/*
    Copies a list to out
    Performance O(n)
*/
int circular_linked_list_copy(t_circular_linked_list * list, t_circular_linked_list * out){
    if(list == NULL){
        return ERROR;
    }

    if(out == NULL){
        return ERROR;
    }

    t_node * origin = (list -> origin);
    if(origin == NULL){
        return SUCCESS;
    }

    t_node * node = origin;
    do{
        if(circular_linked_list_append(out, node -> value)){
            return ERROR;
        }

        node = (node -> next);
    } while(node != origin);
    
    return SUCCESS;
}

/* 
    Deletes a circular linked list
    Performance O(n)
*/
int circular_linked_list_delete(t_circular_linked_list * list, void (*destructor)(t_data)){
    if(list == NULL){
        return ERROR;
    }

    if(destructor != NULL){
        if(circular_linked_list_foreach(list, destructor)){
            return ERROR;
        }
    }
    
    if(circular_linked_list_clear(list)){
        return ERROR;
    }

    free(list);

    return SUCCESS;
}

/*
    Appends a new value right before the origin of a circular linked list
    Performance O(1)
*/
int circular_linked_list_append(t_circular_linked_list * list, t_data value){
    if(list == NULL){
        return ERROR;
    }

    t_node * origin = (list -> origin);

    
    if(origin == NULL){
        t_node * new_origin = node_create_origin(value);
        if(new_origin == NULL){
            return ERROR;
        }
        
        (list -> origin) = new_origin;
    }
    else{
        t_node * new_node = node_create(value, (origin -> previous), origin);
        if(new_node == NULL){
            return ERROR;
        }

        (origin -> previous -> next) = new_node;
        (origin -> previous) = new_node;
    }

    (list -> size)++;

    return SUCCESS;
}

/*
    Inserts a new value to a circular linked list after a given index. Doesn't changes the origin
    Performance O(index)
*/
int circular_linked_list_insert_after(t_circular_linked_list * list, size_t index, t_data value){
    if(list == NULL){
        return ERROR;
    }

    t_node * node;
    if(node_get(list, index, &node)){
        return ERROR;
    }

    t_node * new_node = node_create(value, node, (node -> next));
    if(new_node == NULL){
        return ERROR;
    }

    (node -> next -> previous) = new_node;
    (node -> next) = new_node;
    
    (list -> size)++;

    return SUCCESS;
}

/*
    Inserts a new value to a circular linked list before a given index. Doesn't changes the origin
    Performance O(index)
*/
int circular_linked_list_insert_before(t_circular_linked_list * list, size_t index, t_data value){
    if(list == NULL){
        return ERROR;
    }

    t_node * node;
    if(node_get(list, index, &node)){
        return ERROR;
    }

    t_node * new_node = node_create(value, (node -> previous), node);
    if(new_node == NULL){
        return ERROR;
    }

    (node -> previous -> next) = new_node;
    (node -> previous) = new_node;

    (list -> size)++;

    return SUCCESS;
}

/*
    Gets a value from a circular linked list at the given index
    Performance O(index)
*/
int circular_linked_list_get(t_circular_linked_list * list, size_t index, t_data * out_t_data){
    if(list == NULL){
        return ERROR;
    }

    t_node * node;
    if(node_get(list, index, &node)){
        return ERROR;
    }

    if(out_t_data != NULL){
        *out_t_data = (node -> value);
    }

    return SUCCESS;
}

/*
    Rotates the list, changing its origin. Positive offsets rotate left and negative right
    Performance O(n)
*/
int circular_linked_list_rotate(t_circular_linked_list * list, long offset){
    if(list == NULL){
        return ERROR;
    }

    if(offset == 0){
        return SUCCESS;
    }

    if((list -> size) == 0){
        return SUCCESS;
    }

    size_t index;
    if(offset < 0){
        index = (list -> size) - (labs(offset) % (list -> size));
    }
    else{
        index = offset;
    }

    t_node * node;
    if(node_get(list, index, &node)){
        return ERROR;
    }

    (list -> origin) = node;

    return SUCCESS;
}

/*
    Gets a value and removes it from a circular linked list at the given index
    Performance O(index)
*/
int circular_linked_list_pop(t_circular_linked_list * list, size_t index, t_data * out_t_data){
    if(list == NULL){
        return ERROR;
    }

    t_node * node;
    if(node_get(list, index, &node)){
        return ERROR;
    }

    if(node == (list -> origin)){
        if((list -> size) == 1){
            (list -> origin) = NULL;
        }
        else{
            (list -> origin) = (list -> origin -> next);
        }
    }

    (node -> previous -> next) = (node -> next);
    (node -> next -> previous) = (node -> previous);

    if(out_t_data != NULL){
        *out_t_data = (node -> value);
    }

    free(node);

    (list -> size)--;

    return SUCCESS;
}

/*
    Removes it from a circular linked list at the given index
    Performance O(index)
*/
int circular_linked_list_remove_at(t_circular_linked_list * list, size_t index){
    if(list == NULL){
        return ERROR;
    }

    return circular_linked_list_pop(list, index, NULL);
}

/*
    Returns if a given value is in the circular linked list
    Performance O(n)
*/
int circular_linked_list_has(t_circular_linked_list * list, t_data value, int (*equals_function)(t_data, t_data)){
    if(list == NULL){
        return FALSE;
    }

    if(equals_function == NULL){
        return FALSE;
    }

    if(node_find_from((list -> origin), value, equals_function, NULL, NULL)){
        return FALSE;
    }

    return TRUE;
}

/*
    Clears all values in a circular linked list
    Performance O(n)
*/
int circular_linked_list_clear(t_circular_linked_list * list){
    if(list == NULL){
        return ERROR;
    }

    if((list -> origin) == NULL){
        (list -> size) = 0;
        return SUCCESS;
    }

    t_node * origin = (list -> origin);
    t_node * node = origin -> next;
    t_node * previous = NULL;

    while(node != origin){
        previous = node;
        node = (node -> next);

        free(previous);
    }

    free(origin);

    (list -> origin) = NULL;
    (list -> size) = 0;

    return SUCCESS;
}

/*
    Gets the size of a circular linked list
    Performance O(1)
*/
int circular_linked_list_get_size(t_circular_linked_list * list, size_t * out_size){
    if(list == NULL){
        return ERROR;
    }

    if(out_size != NULL){
        *out_size = (list -> size);
    }

    return SUCCESS;
}

/*
    Executes a function for each value in the circular linked list
    Performance O(nO(function))
*/
int circular_linked_list_foreach(t_circular_linked_list * list, void (*function)(t_data)){
    if(list == NULL){
        return ERROR;
    }

    if(function == NULL){
        return ERROR;
    }

    if((list -> origin) == NULL){
        return SUCCESS;
    }

    t_node * origin = (list -> origin);
    t_node * node = origin;

    do{
        function(node -> value);
        node = (node -> next);
    }while(node != origin);

    return SUCCESS;
}

/*
    Maps all values in a circular linked list
    Performance O(nO(function))
*/
int circular_linked_list_map(t_circular_linked_list * list, t_data (*function)(t_data)){
    if(list == NULL){
        return ERROR;
    }

    if(function == NULL){
        return ERROR;
    }

    if((list -> origin) == NULL){
        return SUCCESS;
    }

    t_node * origin = (list -> origin);
    t_node * node = origin;

    do{
        (node -> value) = function(node -> value);
        node = (node -> next);
    } while(node != origin);

    return SUCCESS;
}

/*
    Returns if given list is empty
    Performance O(1)
*/
int circular_linked_list_is_empty(t_circular_linked_list * list){
    if(list == NULL){
        return ERROR;
    }

    return (list -> size) == 0;
}

/*
    Filters a list and appends it to out
    O(nO(function))
*/
int circular_linked_list_filter(t_circular_linked_list * list, int (*function)(t_data), t_circular_linked_list * out){
    if(list == NULL){
        return ERROR;
    }

    if(function == NULL){
        return ERROR;
    }

    if(out == NULL){
        return ERROR;
    }

    t_node * origin = (list -> origin);
    if(origin == NULL){
        return SUCCESS;
    }
    
    t_node * node = origin;

    do{
        if(function(node -> value)){
            if(circular_linked_list_append(out, node -> value)){
                return ERROR;
            }
        }
        node = (node -> next);
    } while(node != origin);

    return SUCCESS;
}

/*
    Sorts a list through exchange sort
    Performance O(n²)
*/
int circular_linked_list_sort(t_circular_linked_list * list, int (*compare_function)(t_data, t_data)){
    if(list == NULL){
        return ERROR;
    }

    if(compare_function == NULL){
        return ERROR;
    }

    t_node * origin = (list -> origin);
    t_node * pivot = origin;

    do{
        t_node * node = pivot -> next;
        while(node != origin){
            if(compare_function(node -> value, pivot -> value) < 0){
                t_data temp = (pivot -> value);
                (pivot -> value) = (node -> value);
                (node -> value) = temp;
            }
            node = (node -> next);
        }
        pivot = (pivot -> next);
    } while(pivot != origin);

    return SUCCESS;
}

// HELPER FUNCTIONS

static int node_find_from(t_node * start_node, t_data value, int (*equals_function)(t_data, t_data), t_node ** out_node, size_t * out_offset){
    if(start_node == NULL){
        return ERROR;
    }

    if(equals_function == NULL){
        return ERROR;
    }

    size_t offset = 0;
    t_node * node = start_node;
    do{
        if(equals_function((node -> value), value)){
            if(out_node != NULL){
                *out_node = node;
            }

            if(out_offset != NULL){
                *out_offset = offset;
            }

            return SUCCESS;
        }

        node = (node -> next);
        offset++;
    } while(node != start_node);

    return ERROR;
}

static int node_get(t_circular_linked_list * list, size_t index, t_node ** out_node){
    if(list == NULL){
        return ERROR;
    }

    if((list -> size) == 0){
        return ERROR;
    }

    size_t true_index = index % (list -> size);

    t_node * node = (list -> origin);
    for(size_t i = 0; i < true_index; i++){
        node = (node -> next);
    }

    if(out_node != NULL){
        *out_node = node;
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

static t_node * node_create_origin(t_data value){
    t_node * node = node_create(value, NULL, NULL);

    if(node == NULL){
        return NULL;
    }

    (node -> next) = node;
    (node -> previous) = node;

    return node;
}