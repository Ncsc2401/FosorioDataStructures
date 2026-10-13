#ifndef CIRCULAR_LINKED_LIST_H
#define CIRCULAR_LINKED_LIST_H
#include <stdlib.h>

typedef float t_data;

typedef struct s_circular_linked_list t_circular_linked_list;

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

#endif