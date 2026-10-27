#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include <stdlib.h>

typedef float t_data;

typedef struct s_binary_tree t_binary_tree;

t_binary_tree * binary_tree_create(float (*compare_function)(t_data, t_data));
int binary_tree_delete(t_binary_tree * tree, void (*destructor)(t_data));
int binary_tree_insert(t_binary_tree * tree, t_data value);
int binary_tree_has(t_binary_tree * tree, t_data value);
int binary_tree_get_depth(t_binary_tree * tree, t_data value, size_t * out_depth);
int binary_tree_get_root(t_binary_tree * tree, t_data * out);
int binary_tree_get_left(t_binary_tree * tree, t_data value, t_data * out);
int binary_tree_get_right(t_binary_tree * tree, t_data value, t_data * out);
int binary_tree_get_parent(t_binary_tree * tree, t_data value, t_data * out);
int binary_tree_get_size(t_binary_tree * tree, size_t * out);
int binary_tree_min(t_binary_tree * tree, t_data * out);
int binary_tree_max(t_binary_tree * tree, t_data * out);
int binary_tree_is_empty(t_binary_tree * tree);
int binary_tree_get_successor(t_binary_tree * tree, t_data value, t_data * out);
int binary_tree_get_predecessor(t_binary_tree * tree, t_data value, t_data * out);
int binary_tree_pre_order(t_binary_tree * tree, void (*func)(t_data));
int binary_tree_in_order(t_binary_tree * tree, void (*func)(t_data));
int binary_tree_pos_order(t_binary_tree * tree, void (*func)(t_data));

#endif