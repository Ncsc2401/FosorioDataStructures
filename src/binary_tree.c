#include <stdio.h>

#include "binary_tree.h"

#define TRUE 1
#define FALSE 0

#define ERROR 1
#define SUCCESS 0

typedef struct s_node {
    t_data value;
    
    struct s_node * parent;
    struct s_node * left;
    struct s_node * right;
} t_node;

typedef struct s_binary_tree {
    t_node * root;
    t_node * current; // Cursor
    int (*compare_function)(t_data, t_data); // > 0 => Right; < 0 => Left; == 0 => Equal/Error
} t_binary_tree;

typedef struct {
    t_binary_tree * copy_tree;
    int status;
}  t_copy_context;

typedef struct {
    t_node * node;
    int has;
} t_has_context;

// PROTOTYPES
// MAIN

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

// HELPERS

static t_binary_tree_dir value_dir(t_data value, t_node * node, int (*compare_function)(t_data, t_data));
static t_binary_tree_dir node_dir(t_node * parent, t_node * node);
static int node_count_size(t_node * node, size_t * out);
static int node_count_leaves(t_node * node, size_t * out);
static int node_get_depth(t_node * node, size_t * out);
static int node_get_from_dir(const t_binary_tree * tree, t_binary_tree_dir dir, t_node ** out);
static int node_detach_from_parent(t_node * node);
static int node_detach(t_node * node);
static int node_swap_values(t_node * a, t_node * b);
static int node_is_leaf(t_node * node);
static int node_set_left(t_node * parent, t_node * node);
static int node_set_right(t_node * parent, t_node * node);
static int node_remove(t_node * node, void (*destructor)(t_data), t_node * current, int * out_current_removed);
static int node_find(t_node * from, t_data value, int (*compare_function)(t_data, t_data), t_node ** out);
static int subtree_has_node(t_node * node, t_node * what);
static int node_get_left_most(t_node * from, t_node ** out);
static int node_get_right_most(t_node * from, t_node ** out);
static int node_foreach_pre_order(t_node * start, void (*func)(t_data, void *), void * context);
static int node_foreach_in_order(t_node * start, void (*func)(t_data, void *), void * context);
static int node_foreach_pos_order(t_node * start, void (*func)(t_data, void *), void * context);
static int node_foreach_node_pre_order(t_node * start, void (*func)(t_node *, void *), void * context);
static int node_get_successor(t_node * start, t_node ** out, int search_parents);
static int node_get_predecessor(t_node * start, t_node ** out, int search_parents);
static t_node * node_create(t_data value);

// HELPER HELPERS

static void count_func(t_data value, void * context);
static void copy_func(t_data value, void * context);
static void count_leaves_func(t_node * node, void * context);
static void subtree_has_node_func(t_node * node, void * context);

// END PROTOTYPES

/*
    Creates a binary tree
*/
t_binary_tree * binary_tree_create(int (*compare_function)(t_data, t_data)){
    if(compare_function == NULL){
        return NULL;
    }

    t_binary_tree * tree = malloc(sizeof(*tree));
    if(tree == NULL){
        perror("Couldn't allocate tree");
        return NULL;
    }

    (tree -> current) = NULL;
    (tree -> root) = NULL;
    (tree -> compare_function) = compare_function;

    return tree;
}

/*
    Copies a binary tree from source to destiny.
    where direction indicates the subtree to be copied, setting it to BT_ROOT will copy the entire tree.
    destructor is applied to destiny old content
*/
int binary_tree_copy(const t_binary_tree * source, t_binary_tree_dir where, t_binary_tree * destiny, void (*destructor)(t_data)){
    if(source == NULL){
        return ERROR;
    }

    if(destiny == NULL){
        return ERROR;
    }

    if(source == destiny){
        return ERROR;
    }

    t_node * node = NULL;
    if(node_get_from_dir(source, where, &node)){
        return ERROR;
    }

    if(binary_tree_clear(destiny, destructor)){
        return ERROR;
    }

    (destiny -> compare_function) = (source -> compare_function);

    t_copy_context copy_context = {
        destiny, SUCCESS
    };

    if(node_foreach_pre_order(node, copy_func, &copy_context)){
        return ERROR;
    }

    return copy_context.status;
}

/*
    Removes all nodes in tree
*/
int binary_tree_clear(t_binary_tree * tree, void (*destructor)(t_data)){
    if(tree == NULL){
        return ERROR;
    }

    t_node * node = (tree -> root);

    while(node != NULL){
        if(node_is_leaf(node)){
            t_node * previous = node;
            node = (node -> parent);

            node_detach_from_parent(previous);

            if(destructor != NULL){
                destructor(previous -> value);
            }

            free(previous);
            continue;
        }

        if((node -> left) != NULL){
            node = (node -> left);
            continue;
        }
        else if((node -> right) != NULL){
            node = (node -> right);
            continue;
        }
    }

    (tree -> root) = NULL;
    (tree -> current) = NULL;

    return SUCCESS;
}

/*
    Detaches a subtree and makes it the root of another tree.
    It resets the cursor if it was in the detached subtree.
    Out tree root will be replaced and data may be lost.
    Out cursor is set to NULL after the operation
*/
int binary_tree_detach_subtree(t_binary_tree * tree, t_binary_tree_dir where, t_binary_tree * out, void (*destructor)(t_data)){
    if(tree == NULL){
        return ERROR;
    }

    if(tree == out){
        return ERROR;
    }

    t_node * node = NULL;
    if(node_get_from_dir(tree, where, &node)){
        return ERROR;
    }

    if(node == (tree -> root)){
        (tree -> root) = NULL;
    }

    if(subtree_has_node(node, (tree -> current))){
        (tree -> current) = NULL;
    }

    if(node_detach_from_parent(node)){
        return ERROR;
    }

    if(out != NULL){
        if(binary_tree_clear(out, destructor)){
            return ERROR;
        }

        (out -> root) = node;
        (out -> current) = NULL;
        (out -> compare_function) = (tree -> compare_function);
    }
    else{
        if(node_remove(node, destructor, (tree -> current), NULL)){
            return ERROR;
        }
    }

    return SUCCESS;
}

/*
    Deletes a tree.
    May use a destructor for destroying the data. Can be NULL if it is unused.
*/
int binary_tree_delete(t_binary_tree * tree, void (*destructor)(t_data)){
    if(tree == NULL){
        return ERROR;
    }

    if(binary_tree_clear(tree, destructor)){
        return ERROR;
    }

    free(tree);

    return SUCCESS;
}

/*
    Inserts a value to a tree.
    Cursor will point the new node if it was set to NULL.
*/
int binary_tree_insert(t_binary_tree * tree, t_data value){
    if(tree == NULL){
        return ERROR;
    }

    if((tree -> root) == NULL){
        t_node * new_root = node_create(value);
        if(new_root == NULL){
            return ERROR;
        }

        (tree -> root) = new_root;
        (tree -> current) = new_root;
        return SUCCESS;
    }

    t_binary_tree_dir dir = BT_NULL;
    t_node * node = (tree -> root);
    t_node * previous = NULL;
    while(node != NULL){
        dir = value_dir(value, node, (tree -> compare_function));
        previous = node;

        switch(dir){
            case(BT_LEFT):{
                node = (node -> left);
                break;
            }

            case(BT_RIGHT):{
                node = (node -> right);
                break;
            }

            default:{
                return ERROR;
            }
        }
    }

    t_node * new_node = node_create(value);
    if(new_node == NULL){
        return ERROR;
    }

    switch(dir){
        case(BT_LEFT):{
            if(node_set_left(previous, new_node)){
                return ERROR;
            }
            break;
        }

        case(BT_RIGHT):{
            if(node_set_right(previous, new_node)){
                return ERROR;
            }
            break;
        }
        default:{
            return ERROR;
        }
    }

    if((tree -> current) == NULL){
        (tree -> current) = new_node;
    }

    return SUCCESS;
}

/*
    Returns TRUE if the subtree has a given value.
    from direction indicates the subtree.
    Also returns FALSE on errors.
*/
int binary_tree_has(const t_binary_tree * tree, t_binary_tree_dir from, t_data value){
    if(tree == NULL){
        return FALSE;
    }

    t_node * node = NULL;
    if(node_get_from_dir(tree, from, &node)){
        return FALSE;
    }

    if(node == NULL){
        return FALSE;
    }

    if(node_find(node, value, (tree -> compare_function), NULL)){
        return FALSE;
    }

    return TRUE;
}

/*
    Returns TRUE if given node is a leaf.
    where direction indicates the subtree.
    Also returns FALSE on errors.
*/
int binary_tree_is_leaf(const t_binary_tree * tree, t_binary_tree_dir where){
    if(tree == NULL){
        return FALSE;
    }

    t_node * node = NULL;
    if(node_get_from_dir(tree, where, &node)){
        return FALSE;
    }

    if(node == NULL){
        return FALSE;
    }

    return node_is_leaf(node);
}

/*
    Returns the size of a subtree.
    from direction indicates the subtree, setting it to BT_ROOT will find the size of the entire tree.
*/
int binary_tree_get_size(const t_binary_tree * tree, t_binary_tree_dir from, size_t * out){
    if(tree == NULL){
        return ERROR;
    }

    t_node * node = NULL;
    if(node_get_from_dir(tree, from, &node)){
        return ERROR;
    }

    size_t size = 0;
    if(node_count_size(node, &size)){
        return ERROR;
    }

    if(out != NULL){
        (*out) = size;
    }

    return SUCCESS;
}

/*
    Returns the amount of leaves in a subtree.
    from direction indicates the subtree.
*/
int binary_tree_count_leaves(const t_binary_tree * tree, t_binary_tree_dir from, size_t * out){
    if(tree == NULL){
        return ERROR;
    }

    t_node * node = NULL;
    if(node_get_from_dir(tree, from, &node)){
        return ERROR;
    }

    size_t leaves = 0;
    if(node_count_leaves(node, &leaves)){
        return ERROR;
    }

    if(out != NULL){
        (*out) = leaves;
    }

    return SUCCESS;
}

/*
    Moves the cursor to a given position.
    dir direction indicates the position.
    BT_NULL can be used to clear the cursor.
*/
int binary_tree_goto(t_binary_tree * tree, t_binary_tree_dir dir){
    if(tree == NULL){
        return ERROR;
    }

    t_node * node = NULL;
    if(node_get_from_dir(tree, dir, &node)){
        return ERROR;
    }

    (tree -> current) = node;

    return SUCCESS;
}

/*
    Moves the cursor if it finds a node with a given value.
*/
int binary_tree_find(t_binary_tree * tree, t_data value){
    if(tree == NULL){
        return ERROR;
    }

    t_node * node = NULL;
    if(node_find((tree -> root), value, (tree -> compare_function), &node)){
        return ERROR;
    }

    (tree -> current) = node;
    return SUCCESS;
}

/*
    Returns the value of a given node.
    dir direction indicates the node.
    BT_CURRENT can be used to check the current value of the cursor.
*/
int binary_tree_peek(const t_binary_tree * tree, t_binary_tree_dir dir, t_data * out){
    if(tree == NULL){
        return ERROR;
    }

    t_node * node = NULL;
    if(node_get_from_dir(tree, dir, &node)){
        return ERROR;
    }

    if(node == NULL){
        return ERROR;
    }

    if(out != NULL){
        (*out) = (node -> value);
    }

    return SUCCESS;
}

/*
    Applies a function for every value in a subtree.
    traverse_method indicates how the it will traverse the tree.
    from direction indicates the subtree.
    func is the function that will be applied. The [void *] parameter is functions that require additional context
    context if the context given to the func. May be NULL if it will not be used.
    For examples, check the binary_tree.c helper helpers section.
*/
int binary_tree_foreach(const t_binary_tree * tree, t_binary_tree_traverse traverse_method, t_binary_tree_dir from, void (*func)(t_data, void *), void * context){
    if(tree == NULL){
        return ERROR;
    }

    t_node * node = NULL;
    if(node_get_from_dir(tree, from, &node)){
        return ERROR;
    }

    switch(traverse_method){
        case BT_INORDER:{
            if(node_foreach_in_order(node, func, context)){
                return ERROR;
            }
            break;
        }

        case BT_POSTORDER:{
            if(node_foreach_pos_order(node, func, context)){
                return ERROR;
            }
            break;
        }

        case BT_PREORDER:{
            if(node_foreach_pre_order(node, func, context)){
                return ERROR;
            }
            break;
        }

        default:{
            return ERROR;
        }
    }

    return SUCCESS;
}

/*
    Removes a subtree.
    where direction indicates the subtree.
    A destructor function may be passed to destroy the data. It can technically also be used for printing the deleted data, for example.
*/
int binary_tree_remove_subtree(t_binary_tree * tree, t_binary_tree_dir where, void (*destructor)(t_data)){
    if(tree == NULL){
        return ERROR;
    }

    t_node * node = NULL;
    if(node_get_from_dir(tree, where, &node)){
        return ERROR;
    }

    if(node == NULL){
        return ERROR;
    }

    if(node == (tree -> root)){
        (tree -> root) = NULL;
    }

    int current_removed = FALSE;
    if(node_remove(node, destructor, (tree -> current), &current_removed)){
        return ERROR;
    }

    if(current_removed){
        (tree -> current) = NULL;
    }

    return SUCCESS;
}

/*
    Removes a node.
    where direction indicates the node
    A destructor function may be passed to destroy the data.
*/
int binary_tree_remove(t_binary_tree * tree, t_binary_tree_dir where, void (*destructor)(t_data)){
    if(tree == NULL){
        return ERROR;
    }

    t_node * node = NULL;
    if(node_get_from_dir(tree, where, &node)){
        return ERROR;
    }

    t_node * replacement = NULL;
    t_node * child = NULL;
    if(!node_get_successor(node, &replacement, FALSE)){
        if(node_swap_values(node, replacement)){
            return ERROR;
        }

        if((replacement -> parent) != NULL){
            if((replacement -> left) != NULL){
                child = (replacement -> left);
            }
            else if((replacement -> right) != NULL){
                child = (replacement -> right);
            }
        }
    }
    else if(!node_get_predecessor(node, &replacement, FALSE)){
        if(node_swap_values(node, replacement)){
            return ERROR;
        }

        if((replacement -> parent) != NULL){
            if((replacement -> left) != NULL){
                child = (replacement -> left);
            }
            else if((replacement -> right) != NULL){
                child = (replacement -> right);
            }
        }
    }
    else if(node_is_leaf(node)){
        replacement = node;
    }
    else{
        return ERROR;
    }

    t_binary_tree_dir replacement_dir = node_dir((replacement -> parent), replacement);
    switch(replacement_dir){
        case BT_LEFT:{
            node_set_left((replacement -> parent), child);
            break;
        }
        case BT_RIGHT:{
            node_set_right((replacement -> parent), child);
            break;
        }
        case BT_NULL:{
            break;
        }
        default:{
            return ERROR;
        }
    }

    if((tree -> root) == replacement){
        (tree -> root) = NULL;
    }

    if(node_detach(replacement)){
        return ERROR;
    }
    
    t_node * current = (tree -> current);
    if(current == node){
        (tree -> current) = NULL;
    }
    else if(current == replacement){
        (tree -> current) = node;
    }

    if(node_remove(replacement, destructor, (tree -> current), NULL)){
        return ERROR;
    }

    return SUCCESS;
}

/*
    Returns TRUE if a tree is empty
*/
int binary_tree_is_empty(const t_binary_tree * tree){
    if(tree == NULL){
        return TRUE;
    }

    if((tree -> root) == NULL){
        return TRUE;
    }

    return FALSE;
}

/*
    Returns the depth of a node
    from direction indicates the node
    Root has depth 0
*/
int binary_tree_get_depth(const t_binary_tree * tree, t_binary_tree_dir from, size_t * out){
    if(tree == NULL){
        return ERROR;
    }

    t_node * node = NULL;
    if(node_get_from_dir(tree, from, &node)){
        return ERROR;
    }

    size_t depth = 0;
    if(node_get_depth(node, &depth)){
        return ERROR;
    }

    if(out != NULL){
        (*out) = depth;
    }

    return SUCCESS;
}

// HELPERS

static t_binary_tree_dir value_dir(t_data value, t_node * node, int (*compare_function)(t_data, t_data)){
    if(node == NULL){
        return BT_NULL;
    }

    if(compare_function == NULL){
        return BT_NULL;
    }

    int compare_result = compare_function(value, (node -> value));

    if(compare_result < 0){
        return BT_LEFT;
    }
    else if(compare_result > 0){
        return BT_RIGHT;
    } 
    else {
        return BT_CURRENT;
    }
}

static t_binary_tree_dir node_dir(t_node * parent, t_node * node){
    if(parent == NULL){
        return BT_NULL;
    }

    if(node == NULL){
        return BT_NULL;
    }

    if(node == parent){
        return BT_CURRENT;
    }

    if((parent -> left) == node){
        return BT_LEFT;
    }
    else if((parent -> right) == node){
        return BT_RIGHT;
    }

    return BT_NULL;
}

static int node_count_size(t_node * node, size_t * out){
    size_t size = 0;
    if(node_foreach_in_order(node, count_func, &size)){
        return ERROR;
    }

    if(out != NULL){
        (*out) = size;
    }

    return SUCCESS;
}

static int node_count_leaves(t_node * node, size_t * out){
    size_t leaves = 0;
    if(node_foreach_node_pre_order(node, count_leaves_func, &leaves)){
        return ERROR;
    }

    if(out != NULL){
        (*out) = leaves;
    }

    return SUCCESS;
}

static int node_get_depth(t_node * node, size_t * out){
    if(node == NULL){
        return ERROR;
    }

    size_t depth = 0;
    while((node -> parent) != NULL){
        depth++;
        node = (node -> parent);
    }

    if(out != NULL){
        (*out) = depth;
    }

    return SUCCESS;
}

static int node_get_from_dir(const t_binary_tree * tree, t_binary_tree_dir dir, t_node ** out){
    if(tree == NULL){
        return ERROR;
    }

    t_node * current = (tree -> current);

    t_node * node = NULL;
    switch(dir){
        case BT_NULL:{
            node = NULL;
            break;
        }

        case BT_CURRENT:{
            node = (tree -> current);
            break;
        }
        
        case BT_ROOT:{
            node = (tree -> root);
            break;
        }

        case BT_LEFT: {
            if(current == NULL || (current -> left) == NULL){
                return ERROR;
            }

            node = (current -> left);
            break;
        }

        case BT_RIGHT:{
            if(current == NULL || (current -> right) == NULL){
                return ERROR;
            }

            node = (current -> right);
            break;
        }

        case BT_PARENT:{
            if(current == NULL || (current -> parent) == NULL){
                return ERROR;
            }

            node = (current -> parent);
            break;
        }

        case BT_LEFT_MOST:{
            if(node_get_left_most(current, &node)){
                return ERROR;
            }
            break;
        }

        case BT_RIGHT_MOST:{
            if(node_get_right_most(current, &node)){
                return ERROR;
            }
            break;
        }

        case BT_SUCCESSOR:{
            if(node_get_successor(current, &node, TRUE)){
                return ERROR;
            }
            break;
        }
        
        case BT_PREDECESSOR:{
            if(node_get_predecessor(current, &node, TRUE)){
                return ERROR;
            }
            break;
        }
        
        default:{
            return ERROR;
        }
    }

    if(out != NULL){
        (*out) = node;
    }

    return SUCCESS;
}

static int node_detach_from_parent(t_node * node){
    if(node == NULL){
        return ERROR;
    }

    if((node -> parent) == NULL){
        return SUCCESS;
    }

    t_binary_tree_dir dir = node_dir((node -> parent), node);
    switch(dir){
        case BT_LEFT:{
            (node -> parent -> left) = NULL;
            break;
        }
        case BT_RIGHT:{
            (node -> parent -> right) = NULL;
            break;
        }
        case BT_NULL:{
            break;
        }
        default:{
            return ERROR;
        }
    }

    (node -> parent) = NULL;
    return SUCCESS;
}

static int node_detach(t_node * node){
    if(node == NULL){
        return ERROR;
    }

    if(node_detach_from_parent(node)){
        return ERROR;
    }

    if((node -> left) != NULL && (node -> left -> parent) == node){
        (node -> left -> parent) = NULL;
    }

    if((node -> right) != NULL && (node -> right -> parent) == node){
        (node -> right -> parent) = NULL;
    }

    (node -> left) = NULL;
    (node -> right) = NULL;

    return SUCCESS;
}

static int node_swap_values(t_node * a, t_node * b){
    if(a == NULL){
        return ERROR;
    }

    if(b == NULL){
        return ERROR;
    }

    t_data temp = (a -> value);
    (a -> value) = (b -> value);
    (b -> value) = temp;

    return SUCCESS;
}

static int node_is_leaf(t_node * node){
    if(node == NULL){
        return FALSE;
    }

    return ((node -> left) == NULL && (node -> right) == NULL);
}

static int node_set_left(t_node * parent, t_node * node){
    if(parent == NULL){
        return ERROR;
    }

    if((parent -> left) != NULL){
        (parent -> left -> parent) = NULL;
    }

    (parent -> left) = node;

    if(node != NULL){
        (node -> parent) = parent;
    }

    return SUCCESS;
}

static int node_set_right(t_node * parent, t_node * node){
    if(parent == NULL){
        return ERROR;
    }

    if((parent -> right) != NULL){
        (parent -> right -> parent) = NULL;
    }

    (parent -> right) = node;

    if(node != NULL){
        (node -> parent) = parent;
    }

    return SUCCESS;
}

static int node_remove(t_node * node, void (*destructor)(t_data), t_node * current, int * out_current_removed){
    if(node == NULL){
        return ERROR;
    }

    int current_removed = FALSE;
    t_node * stop = (node -> parent);
    t_node * child_node = node;
    while(child_node != stop){
        if((child_node -> left) != NULL){
            child_node = (child_node -> left);
            continue;
        }
        else if((child_node -> right != NULL)){
            child_node = (child_node -> right);
            continue;
        }
        else{
            t_node * temp = child_node;
            child_node = (child_node -> parent);
            
            if(child_node != NULL){
                if((child_node -> left) == temp){
                    (child_node -> left) = NULL;
                }
                else{
                    (child_node -> right) = NULL;
                }
            }

            if(temp == current){
                current_removed = TRUE;
            }

            if(destructor != NULL){
                destructor(temp -> value);
            }
            free(temp);
        }
    }

    if(out_current_removed != NULL){
        (*out_current_removed) = current_removed;
    }

    return SUCCESS;
}

static int node_find(t_node * from, t_data value, int (*compare_function)(t_data, t_data), t_node ** out){
    if(from == NULL){
        return ERROR;
    }

    if(compare_function == NULL){
        return ERROR;
    }

    t_node * node = from;
    while(node != NULL){
        t_binary_tree_dir dir = value_dir(value, node, compare_function);

        switch (dir){
            case BT_LEFT:{
                node = (node -> left);
                break;
            }

            case BT_RIGHT:{
                node = (node -> right);
                break;
            }

            case BT_CURRENT:{
                if(out != NULL){
                    (*out) = node;
                }
                return SUCCESS;
            }

            default:{
                return ERROR;
            }
        }
    }

    return ERROR;
}

static int subtree_has_node(t_node * node, t_node * what){
    if(node == NULL){
        return FALSE;
    }

    if(what == NULL){
        return FALSE;
    }

    t_has_context has_context = {
        what, FALSE
    };
    if(node_foreach_node_pre_order(node, subtree_has_node_func, &has_context)){
        return FALSE;
    }

    return has_context.has;
}

static int node_get_left_most(t_node * from, t_node ** out){
    if(from == NULL){
        return ERROR;
    }

    t_node * node = from;
    while((node -> left) != NULL){
        node = (node -> left);
    }
    
    if(out != NULL){
        (*out) = node;
    }

    return SUCCESS;
}

static int node_get_right_most(t_node * from, t_node ** out){
    if(from == NULL){
        return ERROR;
    }

    t_node * node = from;
    while((node -> right) != NULL){
        node = (node -> right);
    }
    
    if(out != NULL){
        (*out) = node;
    }

    return SUCCESS;
}

static int node_foreach_pre_order(t_node * start, void (*func)(t_data, void *), void * context){
    if(start == NULL){
        return SUCCESS;
    }

    if(func == NULL){
        return ERROR;
    }

    func(start -> value, context);
    if(node_foreach_pre_order(start -> left, func, context)){
        return ERROR;
    }
    if(node_foreach_pre_order(start -> right, func, context)){
        return ERROR;
    }

    return SUCCESS;
}

static int node_foreach_in_order(t_node * start, void (*func)(t_data, void *), void * context){
    if(start == NULL){
        return SUCCESS;
    }

    if(func == NULL){
        return ERROR;
    }

    if(node_foreach_in_order((start -> left), func, context)){
        return ERROR;
    }
    func(start -> value, context);
    if(node_foreach_in_order((start -> right), func, context)){
        return ERROR;
    }

    return SUCCESS;
}

static int node_foreach_pos_order(t_node * start, void (*func)(t_data, void *), void * context){
    if(start == NULL){
        return SUCCESS;
    }

    if(func == NULL){
        return ERROR;
    }

    if(node_foreach_pos_order(start -> left, func, context)){
        return ERROR;
    }
    if(node_foreach_pos_order(start -> right, func, context)){
        return ERROR;
    }
    func(start -> value, context);

    return SUCCESS;
}

static int node_foreach_node_pre_order(t_node * start, void (*func)(t_node *, void *), void * context){
    if(start == NULL){
        return SUCCESS;
    }

    if(func == NULL){
        return ERROR;
    }


    func(start, context);
    if(node_foreach_node_pre_order(start -> left, func, context)){
        return ERROR;
    }
    if(node_foreach_node_pre_order(start -> right, func, context)){
        return ERROR;
    }

    return SUCCESS;
}

static int node_get_successor(t_node * start, t_node ** out, int search_parents){
    if(start == NULL){
        return ERROR;
    }

    t_node * successor = NULL;
    if((start -> right) != NULL){
        node_get_left_most((start -> right), &successor);
    }
    else if(search_parents){
        t_node * node = start;
        while((node -> parent) != NULL){
            t_binary_tree_dir dir = node_dir((node -> parent), node);
            if(dir == BT_LEFT){
                successor = (node -> parent);
                break;
            }

            node = (node -> parent);
        }
    }

    if(successor == NULL){
        return ERROR;
    }

    if(out != NULL){
        (*out) = successor;
    }

    return SUCCESS;
}

static int node_get_predecessor(t_node * start, t_node ** out, int search_parents){
    if(start == NULL){
        return ERROR;
    }

    t_node * predecessor = NULL;
    if((start -> left) != NULL){
        node_get_right_most((start -> left), &predecessor);
    }
    else if(search_parents){
        t_node * node = start;
        while((node -> parent) != NULL){
            if((node -> parent -> right) == node){
                predecessor = (node -> parent);
                break;
            }

            node = (node -> parent);
        }
    }

    if(predecessor == NULL){
        return ERROR;
    }

    if(out != NULL){
        (*out) = predecessor;
    }

    return SUCCESS;
}

static t_node * node_create(t_data value){
    t_node * node = malloc(sizeof(*node));
    if(node == NULL){
        return NULL;
    }

    (node -> parent) = NULL;
    (node -> value) = value;
    (node -> right) = NULL;
    (node -> left) = NULL;

    return node;
}

// HELPER HELPERS

static void count_func(t_data value, void * context){
    size_t * counter = context;
    (*counter)++;
}

static void copy_func(t_data value, void * context){
    t_copy_context * copy_context = context;
    
    t_binary_tree * copy_tree = (copy_context -> copy_tree);
    if(binary_tree_insert(copy_tree, value)){
        (copy_context -> status) = ERROR;
    }
}

static void count_leaves_func(t_node * node, void * context){
    size_t * leaves = context;
    if(node_is_leaf(node)){
        (*leaves)++;
    }
}

static void subtree_has_node_func(t_node * node, void * context){
    t_has_context * has_context = context;

    if(node == (has_context -> node)){
        (has_context -> has) = TRUE;
    }
}