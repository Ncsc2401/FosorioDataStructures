#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include <stdlib.h>


typedef float data;

typedef struct s_doubly_linked_list t_doubly_linked_list;

// Prototypes
t_doubly_linked_list * doubly_linked_list_create(void);
int doubly_linked_list_copy(t_doubly_linked_list * list, t_doubly_linked_list * out);
int doubly_linked_list_delete(t_doubly_linked_list * list, void (*destructor)(data));
int doubly_linked_list_append(t_doubly_linked_list * list, data value);
int doubly_linked_list_insert_after(t_doubly_linked_list * list, size_t index, data value);
int doubly_linked_list_insert_before(t_doubly_linked_list * list, size_t index, data value);
int doubly_linked_list_get(t_doubly_linked_list * list, size_t index, data * out_data);
int doubly_linked_list_pop(t_doubly_linked_list * list, size_t index, data * out_data);
int doubly_linked_list_remove_at(t_doubly_linked_list * list, size_t index);
int doubly_linked_list_has(t_doubly_linked_list * list, data value, int (*equals_function)(data, data));
int doubly_linked_list_find_first_after(t_doubly_linked_list * list, data value, size_t index, int (*equals_function)(data, data), size_t * out_index);
int doubly_linked_list_remove_first_after(t_doubly_linked_list * list, data value, size_t index, int (*equals_function)(data, data));
int doubly_linked_list_clear(t_doubly_linked_list * list);
int doubly_linked_list_get_size(t_doubly_linked_list * list, size_t * out_size);
int doubly_linked_list_foreach(t_doubly_linked_list * list, void (*function)(data));
int doubly_linked_list_map(t_doubly_linked_list * list, data (*function)(data));
int doubly_linked_list_is_empty(t_doubly_linked_list * list);
int doubly_linked_list_filter(t_doubly_linked_list * list, int (*function)(data), t_doubly_linked_list * out);
int doubly_linked_list_sort(t_doubly_linked_list * list, int (*compare_function)(data, data));

#endif