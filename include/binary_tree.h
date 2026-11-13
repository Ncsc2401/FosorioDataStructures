#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include <stdlib.h>

typedef float t_data;

typedef enum e_binary_tree_dir{
    BT_NULL,
    BT_CURRENT,
    BT_LEFT,
    BT_RIGHT,
    BT_PARENT,
    BT_LEFT_MOST,
    BT_RIGHT_MOST,
    BT_SUCCESSOR,
    BT_PREDECESSOR,
    BT_ROOT
} t_binary_tree_dir;

typedef enum e_binary_tree_traverse{
    BT_INORDER,
    BT_POSTORDER,
    BT_PREORDER
} t_binary_tree_traverse;

typedef struct s_binary_tree t_binary_tree;

t_binary_tree * binary_tree_create(int (*compare_function)(t_data, t_data));
int binary_tree_copy(const t_binary_tree * source, t_binary_tree_dir where, t_binary_tree * destiny, void (*destructor)(t_data));
int binary_tree_clear(t_binary_tree * tree, void (*destructor)(t_data));
int binary_tree_detach_subtree(t_binary_tree * tree, t_binary_tree_dir where, t_binary_tree * out, void (*destructor)(t_data));
int binary_tree_delete(t_binary_tree * tree, void (*destructor)(t_data));
int binary_tree_insert(t_binary_tree * tree, t_data value);
int binary_tree_has(const t_binary_tree * tree, t_binary_tree_dir from, t_data value);
int binary_tree_is_leaf(const t_binary_tree * tree, t_binary_tree_dir where);
int binary_tree_get_size(const t_binary_tree * tree, t_binary_tree_dir from, size_t * out);
int binary_tree_count_leaves(const t_binary_tree * tree, t_binary_tree_dir from, size_t * out);
int binary_tree_goto(t_binary_tree * tree, t_binary_tree_dir dir);
int binary_tree_find(t_binary_tree * tree, t_data value);
int binary_tree_peek(const t_binary_tree * tree, t_binary_tree_dir dir, t_data * out);
int binary_tree_foreach(const t_binary_tree * tree, t_binary_tree_traverse traverse_method, t_binary_tree_dir from, void (*func)(t_data, void *), void * context);
int binary_tree_remove_subtree(t_binary_tree * tree, t_binary_tree_dir where, void (*destructor)(t_data));
int binary_tree_remove(t_binary_tree * tree, t_binary_tree_dir where, void (*destructor)(t_data));
int binary_tree_is_empty(const t_binary_tree * tree);
int binary_tree_get_depth(const t_binary_tree * tree, t_binary_tree_dir from, size_t * out);

#endif