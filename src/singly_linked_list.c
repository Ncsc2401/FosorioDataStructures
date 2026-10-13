#include <stdio.h>

#include "singly_linked_list.h"

#define ERROR 1
#define SUCCESS 0

#define TRUE 1
#define FALSE 0

typedef struct s_node {
    t_data value;
    struct s_node * next;
} t_node;

typedef struct s_singly_linked_list{
    size_t size;
    t_node * head;
    t_node * tail;
} t_singly_linked_list;

// Prototypes
t_singly_linked_list * singly_linked_list_create(void);
int singly_linked_list_copy(t_singly_linked_list * list, t_singly_linked_list * out);
int singly_linked_list_delete(t_singly_linked_list * list, void (*destructor)(t_data));
int singly_linked_list_append(t_singly_linked_list * list, t_data value);
int singly_linked_list_insert_after(t_singly_linked_list * list, size_t index, t_data value);
int singly_linked_list_insert_before(t_singly_linked_list * list, size_t index, t_data value);
int singly_linked_list_get(t_singly_linked_list * list, size_t index, t_data * out_t_data);
int singly_linked_list_pop(t_singly_linked_list * list, size_t index, t_data * out_t_data);
int singly_linked_list_remove_at(t_singly_linked_list * list, size_t index);
int singly_linked_list_has(t_singly_linked_list * list, t_data value, int (*equals_function)(t_data, t_data));
int singly_linked_list_find_first_after(t_singly_linked_list * list, t_data value, size_t index, int (*equals_function)(t_data, t_data), size_t * out_index);
int singly_linked_list_remove_first_after(t_singly_linked_list * list, t_data value, size_t index, int (*equals_function)(t_data, t_data));
int singly_linked_list_clear(t_singly_linked_list * list);
int singly_linked_list_get_size(t_singly_linked_list * list, size_t * out_size);
int singly_linked_list_foreach(t_singly_linked_list * list, void (*function)(t_data));
int singly_linked_list_map(t_singly_linked_list * list, t_data (*function)(t_data));
int singly_linked_list_is_empty(t_singly_linked_list * list);
int singly_linked_list_filter(t_singly_linked_list * list, int (*function)(t_data), t_singly_linked_list * out);
int singly_linked_list_sort(t_singly_linked_list * list, int (*compare_function)(t_data, t_data));

// Helper prototypes
static int node_get(t_singly_linked_list * list, size_t index, t_node ** out_node);
static int node_find_from(t_node * start_node, t_data value, int (*equals_function)(t_data, t_data), t_node ** out_node, size_t * out_offset);
static t_node * node_create(t_data value, t_node * next);

/* 
    Creates a new singly linked list 
    Performance O(1)
*/
t_singly_linked_list * singly_linked_list_create(void){
    t_singly_linked_list * list = malloc(sizeof(*list));

    if(list == NULL){
        perror("Couldn't allocate singly linked list");
        return NULL;
    }

    (list -> size) = 0;
    (list -> tail) = NULL;
    (list -> head) = NULL;

    return list;
}

/*
    Copies a list to out
    Performance O(n)
*/
int singly_linked_list_copy(t_singly_linked_list * list, t_singly_linked_list * out){
    if(list == NULL){
        return ERROR;
    }

    if(out == NULL){
        return ERROR;
    }

    t_node * node = (list -> head);
    while(node != NULL){
        if(singly_linked_list_append(out, node -> value)){
            return ERROR;
        }

        node = (node -> next);
    }

    return SUCCESS;
}

/* 
    Deletes a singly linked list
    Performance O(2n)
*/
int singly_linked_list_delete(t_singly_linked_list * list, void (*destructor)(t_data)){
    if(list == NULL){
        return ERROR;
    }

    if(destructor != NULL){
        if(singly_linked_list_foreach(list, destructor)){
            return ERROR;
        }
    }
    
    if(singly_linked_list_clear(list)){
        return ERROR;
    }

    free(list);

    return SUCCESS;
}

/*
    Appends a new value to a singly linked list
    Performance O(1)
*/
int singly_linked_list_append(t_singly_linked_list * list, t_data value){
    if(list == NULL){
        return ERROR;
    }

    t_node * new_node = node_create(value, NULL);
    if(new_node == NULL){
        return ERROR;
    }

    if((list -> head) == NULL){
        (list -> head) = new_node;
    }
    else{
        (list -> tail -> next) = new_node;
    }
    
    (list -> tail) = new_node;
    (list -> size)++;
    
    return SUCCESS;
}

/*
    Inserts a new value to a singly linked list after a given index
    Performance O(index)
*/
int singly_linked_list_insert_after(t_singly_linked_list * list, size_t index, t_data value){
    if(list == NULL){
        return ERROR;
    }

    if(index >= (list -> size)){
        return ERROR;
    }

    t_node * node;
    if(node_get(list, index, &node)){
        return ERROR;
    }

    t_node * new_node = node_create(value, (node -> next));
    if(new_node == NULL){
        return ERROR;
    }

    if((node -> next) == NULL){
        (list -> tail) = new_node;
    }

    (node -> next) = new_node;
    (list -> size)++;

    return SUCCESS;
}

/*
    Inserts a new value to a singly linked list before a given index
    Performance O(index)
*/
int singly_linked_list_insert_before(t_singly_linked_list * list, size_t index, t_data value){
    if(list == NULL){
        return ERROR;
    }

    if(index >= (list -> size)){
        return ERROR;
    }

    if(index == 0){
        t_node * head = (list -> head);
        
        t_node * new_head = node_create(value, head);
        if(new_head == NULL){
            return ERROR;
        }

        (list -> head) = new_head;
    }
    else{
        t_node * node;
        if(node_get(list, index - 1, &node)){
            return ERROR;
        }

        t_node * new_node = node_create(value, (node -> next));
        if(new_node == NULL){
            return ERROR;
        }
        
        (node -> next) = new_node;
    }

    (list -> size)++;

    return SUCCESS;
}

/*
    Gets a value from a singly linked list at the given index
    Performance O(index)
*/
int singly_linked_list_get(t_singly_linked_list * list, size_t index, t_data * out_t_data){
    if(list == NULL){
        return ERROR;
    }

    if(index >= (list -> size)){
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
    Gets a value and removes it from a singly linked list at the given index
    Performance O(index)
*/
int singly_linked_list_pop(t_singly_linked_list * list, size_t index, t_data * out_t_data){
    if(list == NULL){
        return ERROR;
    }

    if(index >= (list -> size)){
        return ERROR;
    }

    t_node * to_remove;
    if(index == 0){
        to_remove = (list -> head);

        (list -> head) = (to_remove -> next);
    }
    else{
        t_node * node;
        if(node_get(list, index - 1, &node)){
            return ERROR;
        }

        to_remove = (node -> next);
        (node -> next) = (to_remove -> next);

        if((to_remove -> next) == NULL){
            (list -> tail) = node;
        }
    }

    if(out_t_data != NULL){
        *out_t_data = (to_remove -> value);
    }

    free(to_remove);

    (list -> size)--;

    return SUCCESS;
}

/*
    Removes it from a singly linked list at the given index
    Performance O(index)
*/
int singly_linked_list_remove_at(t_singly_linked_list * list, size_t index){
    if(list == NULL){
        return ERROR;
    }

    if(index >= (list -> size)){
        return ERROR;
    }

    return singly_linked_list_pop(list, index, NULL);
}

/*
    Returns if a given value is in the singly linked list
    Performance O(n)
*/
int singly_linked_list_has(t_singly_linked_list * list, t_data value, int (*equals_function)(t_data, t_data)){
    if(list == NULL){
        return FALSE;
    }

    if(equals_function == NULL){
        return FALSE;
    }

    if(node_find_from((list -> head), value, equals_function, NULL, NULL)){
        return FALSE;
    }

    return TRUE;
}

/*
    Gets the index of the first occurrence of a value after a given index in a linked list
    Performance O(n)
*/
int singly_linked_list_find_first_after(t_singly_linked_list * list, t_data value, size_t index, int (*equals_function)(t_data, t_data), size_t * out_index){
    if(list == NULL){
        return ERROR;
    }

    if(equals_function == NULL){
        return ERROR;
    }

    t_node * node;
    if(node_get(list, index, &node)){
        return ERROR;
    }

    size_t offset;
    if(node_find_from((node -> next), value, equals_function, NULL, &offset)){
        return ERROR;
    }

    if(out_index != NULL){
        *out_index = index + offset + 1;
    }

    return SUCCESS;
}

/*
    Removes the first occurrence of a value after a given index in a linked list
    Performance O(2n)
*/
int singly_linked_list_remove_first_after(t_singly_linked_list * list, t_data value, size_t index, int (*equals_function)(t_data, t_data)){
    if(list == NULL){
        return ERROR;
    }

    if(equals_function == NULL){
        return ERROR;
    }

    t_node * node;
    if(node_get(list, index, &node)){
        return ERROR;
    }

    size_t offset;
    if(node_find_from((node -> next), value, equals_function, NULL, &offset)){
        return ERROR;
    }

    return singly_linked_list_remove_at(list, index + offset + 1);
}

/*
    Clears all values in a singly linked list
    Performance O(n)
*/
int singly_linked_list_clear(t_singly_linked_list * list){
    if(list == NULL){
        return ERROR;
    }

    t_node * node = (list -> head);
    t_node * previous = NULL;

    while(node != NULL){
        previous = node;
        node = (node -> next);

        free(previous);
    }

    (list -> head) = NULL;
    (list -> size) = 0;

    return SUCCESS;
}

/*
    Gets the size of a linked list
    Performance O(1)
*/
int singly_linked_list_get_size(t_singly_linked_list * list, size_t * out_size){
    if(list == NULL){
        return ERROR;
    }

    if(out_size != NULL){
        *out_size = (list -> size);
    }

    return SUCCESS;
}

/*
    Executes a function for each value in the singly linked list
    Performance O(nO(function))
*/
int singly_linked_list_foreach(t_singly_linked_list * list, void (*function)(t_data)){
    if(list == NULL){
        return ERROR;
    }

    if(function == NULL){
        return ERROR;
    }

    t_node * node = (list -> head);

    while(node != NULL){
        function(node -> value);
        node = (node -> next);
    }

    return SUCCESS;
}

/*
    Maps all values in a singly linked list
    Performance O(nO(function))
*/
int singly_linked_list_map(t_singly_linked_list * list, t_data (*function)(t_data)){
    if(list == NULL){
        return ERROR;
    }

    if(function == NULL){
        return ERROR;
    }

    t_node * node = (list -> head);

    while(node != NULL){
        (node -> value) = function(node -> value);
        node = (node -> next);
    }

    return SUCCESS;
}

/*
    Returns if given list is empty
    Performance O(1)
*/
int singly_linked_list_is_empty(t_singly_linked_list * list){
    if(list == NULL){
        return ERROR;
    }

    return (list -> size) == 0;
}

/*
    Applies a filter and appends the matching values to out
    O(NO(function))
*/
int singly_linked_list_filter(t_singly_linked_list * list, int (*function)(t_data), t_singly_linked_list * out){
    if(list == NULL){
        return ERROR;
    }

    if(function == NULL){
        return ERROR;
    }

    if(out == NULL){
        return ERROR;
    }

    t_node * node = (list -> head);

    while(node != NULL){
        if(function(node -> value)){
            if(singly_linked_list_append(out, node -> value)){
                return ERROR;
            }
        }
        node = (node -> next);
    }

    return SUCCESS;
}

/*
    Sorts the list through exchange sort
    O(n²)
*/
int singly_linked_list_sort(t_singly_linked_list * list, int (*compare_function)(t_data, t_data)){
    if(list == NULL){
        return ERROR;
    }

    if(compare_function == NULL){
        return ERROR;
    }

    t_node * pivot = (list -> head);

    while(pivot != NULL){
        t_node * node = pivot -> next;
        while(node != NULL){
            if(compare_function(node -> value, pivot -> value) < 0){
                t_data temp = pivot -> value;
                pivot -> value = node -> value;
                node -> value = temp;
            }
            node = node -> next;
        }
        pivot = pivot -> next;
    }
    return SUCCESS;
}

// HELPER FUNCTIONS

static int node_get(t_singly_linked_list * list, size_t index, t_node ** out_node){
    if(list == NULL){
        return ERROR;
    }

    if(index >= (list -> size)){
        return ERROR;
    }

    t_node * node = (list -> head);
    for(size_t i = 0; i < index; i++){
        node = (node -> next);
    }

    if(out_node != NULL){
        *out_node = node;
    }
    
    return SUCCESS;
}

static int node_find_from(t_node * start_node, t_data value, int (*equals_function)(t_data, t_data), t_node ** out_node, size_t * out_offset){
    if(start_node == NULL){
        return ERROR;
    }

    if(equals_function == NULL){
        return ERROR;
    }

    size_t offset = 0;
    t_node * node = start_node;
    while(node != NULL){
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
    }

    return ERROR;
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

