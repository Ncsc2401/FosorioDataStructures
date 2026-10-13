#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H

#include <stdlib.h>

typedef float data;

typedef struct s_singly_linked_list t_singly_linked_list;

// Prototypes
t_singly_linked_list * singly_linked_list_create(void);
int singly_linked_list_copy(t_singly_linked_list * list, t_singly_linked_list * out);
int singly_linked_list_delete(t_singly_linked_list * list, void (*destructor)(data));
int singly_linked_list_append(t_singly_linked_list * list, data value);
int singly_linked_list_insert_after(t_singly_linked_list * list, size_t index, data value);
int singly_linked_list_insert_before(t_singly_linked_list * list, size_t index, data value);
int singly_linked_list_get(t_singly_linked_list * list, size_t index, data * out_data);
int singly_linked_list_pop(t_singly_linked_list * list, size_t index, data * out_data);
int singly_linked_list_remove_at(t_singly_linked_list * list, size_t index);
int singly_linked_list_has(t_singly_linked_list * list, data value, int (*equals_function)(data, data));
int singly_linked_list_find_first_after(t_singly_linked_list * list, data value, size_t index, int (*equals_function)(data, data), size_t * out_index);
int singly_linked_list_remove_first_after(t_singly_linked_list * list, data value, size_t index, int (*equals_function)(data, data));
int singly_linked_list_clear(t_singly_linked_list * list);
int singly_linked_list_get_size(t_singly_linked_list * list, size_t * out_size);
int singly_linked_list_foreach(t_singly_linked_list * list, void (*function)(data));
int singly_linked_list_map(t_singly_linked_list * list, data (*function)(data));
int singly_linked_list_is_empty(t_singly_linked_list * list);
int singly_linked_list_filter(t_singly_linked_list * list, int (*function)(data), t_singly_linked_list * out);
int singly_linked_list_sort(t_singly_linked_list * list, int (*compare_function)(data, data));

#endif