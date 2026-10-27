#include "binary_tree.h"
#include <stdio.h>

#define ERROR 1
#define SUCCESS 0

#define TRUE 1
#define FALSE 0

typedef struct s_node {
    t_data value;
    struct s_node * parent;
    struct s_node * left;
    struct s_node * right;
} t_node;

typedef struct s_binary_tree{
    t_node * root;
    float (*compare_function)(t_data, t_data); // < 0 => RIGHT; > 0 => LEFT; == 0 => ERROR
    size_t size;
} t_binary_tree;

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

// HELPER PROTOTYPES
static t_node * node_create(t_data value, t_node * parent, t_node * left, t_node * right);
static int node_get_left_most_from(t_node * start, t_node ** out);
static int node_get_right_most_from(t_node * start, t_node ** out);
static int node_find_from(t_node * start, float (*compare_function)(t_data, t_data), t_data value, t_node ** out, size_t * out_size);
static int node_is_leaf(t_node * node);
static int node_is_root(t_node * node);
static int node_count_depth(t_node * start, size_t * out_depth);
static int node_pre_order_traverse(t_node * start, void (*func)(t_data));
static int node_in_order_traverse(t_node * start, void (*func)(t_data));
static int node_pos_order_traverse(t_node * start, void (*func)(t_data));

t_binary_tree * binary_tree_create(float (*compare_function)(t_data, t_data)){
    if(compare_function == NULL){   
        return NULL;
    }

    t_binary_tree * tree = malloc(sizeof(*tree));
    if(tree == NULL){
        perror("Couldn't allocate tree");
        return NULL;
    }

    (tree -> compare_function) = compare_function;
    (tree -> size) = 0;
    (tree -> root) = NULL;

    return tree;
}

int binary_tree_delete(t_binary_tree * tree, void (*destructor)(t_data)){
    if(tree == NULL){
        return ERROR;
    }

    t_node * node = (tree -> root);

    while(node != NULL){
        if(node_is_leaf(node)){
            t_node * previous = node;
            node = (node -> parent);

            if(node != NULL){
                if((node -> left) == previous){
                    (node -> left) = NULL;
                }
                else{
                    (node -> right) = NULL;
                }
            }

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

    free(tree);

    return SUCCESS;
}

int binary_tree_insert(t_binary_tree * tree, t_data value){
    if(tree == NULL){
        return ERROR;
    }

    if((tree -> root) == NULL){
        t_node * new_root = node_create(value, NULL, NULL, NULL);
        if(new_root == NULL){
            return ERROR;
        }
        (tree -> root) = new_root;
        (tree -> size)++;

        return SUCCESS;
    }

    t_node * node = (tree -> root);
    t_node * previous = NULL;
    while(node != NULL){
        if((tree -> compare_function)((node -> value), value) > 0){
            previous = node;
            node = (node -> right);
            continue;
        }
        else if((tree -> compare_function)((node -> value), value) < 0){
            previous = node;
            node = (node -> left);
            continue;
        }
        else{
            return ERROR;
        }
    }

    t_node * new_node = node_create(value, previous, NULL, NULL);
    if(new_node == NULL){
        return ERROR;
    }

    // value > previous => right
    if((tree -> compare_function)((previous -> value), value) > 0){
        (previous -> right) = new_node;
    }
    // value < previous => left
    else if((tree -> compare_function)((previous -> value), value) < 0){
        (previous -> left) = new_node;
    }

    (tree -> size)++;
    return SUCCESS;
}

int binary_tree_has(t_binary_tree * tree, t_data value){
    if(tree == NULL){
        return FALSE;
    }

    return !node_find_from((tree -> root), (tree -> compare_function), value, NULL, NULL);
}

int binary_tree_get_depth(t_binary_tree * tree, t_data value, size_t * out_depth){
    if(tree == NULL){
        return ERROR;
    }

    size_t depth;
    if(node_find_from((tree -> root), (tree -> compare_function), value, NULL, &depth)){
        return ERROR;
    }

    if(out_depth != NULL){
        (*out_depth) = depth;
    }

    return SUCCESS;
}

int binary_tree_get_root(t_binary_tree * tree, t_data * out){
    if(tree == NULL){
        return ERROR;
    }

    if((tree -> root) == NULL){
        return ERROR;
    }

    if(out != NULL){
        (*out) = (tree -> root -> value);
    }

    return SUCCESS;
}

int binary_tree_get_left(t_binary_tree * tree, t_data value, t_data * out){
    if(tree == NULL){
        return ERROR;
    }

    t_node * node;
    if(node_find_from((tree -> root), (tree -> compare_function), value, &node, NULL)){
        return ERROR;
    }

    if((node -> left) == NULL){
        return ERROR;
    }

    if(out != NULL){
        (*out) = (node -> left -> value);
    }

    return SUCCESS;
}

int binary_tree_get_right(t_binary_tree * tree, t_data value, t_data * out){
    if(tree == NULL){
        return ERROR;
    }

    t_node * node;
    if(node_find_from((tree -> root), (tree -> compare_function), value, &node, NULL)){
        return ERROR;
    }

    if((node -> right) == NULL){
        return ERROR;
    }

    if(out != NULL){
        (*out) = (node -> right -> value);
    }

    return SUCCESS;
}

int binary_tree_get_parent(t_binary_tree * tree, t_data value, t_data * out){
    if(tree == NULL){
        return ERROR;
    }

    t_node * node;
    if(node_find_from((tree -> root), (tree -> compare_function), value, &node, NULL)){
        return ERROR;
    }

    if((node -> parent) == NULL){
        return ERROR;
    }

    if(out != NULL){
        (*out) = (node -> parent -> value);
    }

    return SUCCESS;
}

int binary_tree_get_size(t_binary_tree * tree, size_t * out){
    if(tree == NULL){
        return ERROR;
    }

    if(out != NULL){
        (*out) = (tree -> size);
    }

    return SUCCESS;
}

int binary_tree_max(t_binary_tree * tree, t_data * out){
    if(tree == NULL){
        return ERROR;
    }

    t_node * node;
    if(node_get_right_most_from((tree -> root), &node)){
        return ERROR;
    }

    if(out != NULL){
        (*out) = (node -> value);
    }

    return SUCCESS;
}

int binary_tree_min(t_binary_tree * tree, t_data * out){
    if(tree == NULL){
        return ERROR;
    }

    t_node * node;
    if(node_get_left_most_from((tree -> root), &node)){
        return ERROR;
    }

    if(out != NULL){
        (*out) = (node -> value);
    }
    
    return SUCCESS;
}

int binary_tree_is_empty(t_binary_tree * tree){
    if(tree == NULL){
        return TRUE;
    }

    return (tree -> root) == NULL;
}

int binary_tree_get_successor(t_binary_tree * tree, t_data value, t_data * out){
    if(tree == NULL){
        return ERROR;
    }

    t_node * node;
    if(node_find_from((tree -> root), (tree -> compare_function), value, &node, NULL)){
        return ERROR;
    }

    t_node * successor = node;
    if((node -> right) != NULL){
        if(node_get_left_most_from((node -> right), &successor)){
            return ERROR;
        }
    }

    if(out != NULL){
        (*out) = (successor -> value);
    }
}

int binary_tree_get_predecessor(t_binary_tree * tree, t_data value, t_data * out){
    if(tree == NULL){
        return ERROR;
    }

    t_node * node;
    if(node_find_from((tree -> root), (tree -> compare_function), value, &node, NULL)){
        return ERROR;
    }

    t_node * predecessor = node;
    if((node -> left) != NULL){
        if(node_get_right_most_from((node -> left), &predecessor)){
            return ERROR;
        }
    }

    if(out != NULL){
        (*out) = (predecessor -> value);
    }
}

int binary_tree_pre_order(t_binary_tree * tree, void (*func)(t_data)){
    if(tree == NULL){
        return ERROR;
    }

    if(func == NULL){
        return ERROR;
    }

    return node_pre_order_traverse((tree -> root), func);
}

int binary_tree_in_order(t_binary_tree * tree, void (*func)(t_data)){
    if(tree == NULL){
        return ERROR;
    }

    if(func == NULL){
        return ERROR;
    }

    return node_in_order_traverse((tree -> root), func);
}

int binary_tree_pos_order(t_binary_tree * tree, void (*func)(t_data)){
    if(tree == NULL){
        return ERROR;
    }

    if(func == NULL){
        return ERROR;
    }

    return node_pos_order_traverse((tree -> root), func);
}

// HELPER FUNCTIONS

static t_node * node_create(t_data value, t_node * parent, t_node * left, t_node * right){
    t_node * node = malloc(sizeof(*node));
    if(node == NULL){
        perror("Couldn't allocate node");
        return NULL;
    }

    (node -> value) = value;
    (node -> parent) = parent;
    (node -> left) = left;
    (node -> right) = right;
    
    return node;
}

static int node_get_left_most_from(t_node * start, t_node ** out){
    t_node * node = start;
    if(node == NULL){
        return ERROR;
    }

    while((node -> left) != NULL){
        node = (node -> left);
    }

    if(out != NULL){
        (*out) = node;
    }

    return SUCCESS;
}

static int node_get_right_most_from(t_node * start, t_node ** out){
    t_node * node = start; 
    if(node == NULL){
        return ERROR;
    }

    while((node -> right) != NULL){
        node = (node -> right);
    }

    if(out != NULL){
        (*out = node);
    }

    return SUCCESS;
}

static int node_count_depth(t_node * start, size_t * out_depth){
    if(start == NULL){
        return ERROR;
    }

    size_t depth = 1;
    t_node * node = start;
    while((node -> parent) != NULL){
        node = (node -> parent);
        depth++;
    }

    if(out_depth != NULL){
        (*out_depth) = depth;
    }

    return SUCCESS;
}

static int node_find_from(t_node * start, float (*compare_function)(t_data, t_data), t_data value, t_node ** out, size_t * out_depth){
    if(start == NULL){
        return ERROR;
    }

    size_t depth;
    if(node_count_depth(start, &depth)){
        return ERROR;
    }

    t_node * node = start;
    while(node != NULL){
        if(compare_function((node -> value), value) == 0){
            break;
        }
        else if(compare_function((node -> value), value) > 0){
            node = (node -> right);
        }
        else if(compare_function((node -> value), value) < 0){
            node = (node -> left);
        }

        depth++;
    }

    if(node == NULL){
        return ERROR;
    }
    
    if(out != NULL){
        (*out) = node;
    }

    if(out_depth != NULL){
        (*out_depth) = depth;
    }

    return SUCCESS;
}

static int node_is_leaf(t_node * node){
    if(node == NULL){
        return FALSE;
    }

    return (node -> left) == NULL && (node -> right) == NULL;
}

static int node_is_root(t_node * node){
    if(node == NULL){
        return FALSE;
    }

    return (node -> parent) == NULL;
}

static int node_pre_order_traverse(t_node * start, void (*func)(t_data)){
    if(start == NULL){
        return SUCCESS;
    }

    if(func == NULL){
        return ERROR;
    }

    func(start -> value);
    if(node_pre_order_traverse((start -> left), func)){
        return ERROR;
    }

    if(node_pre_order_traverse((start -> right), func)){
        return ERROR;
    }

    return SUCCESS;
}

static int node_in_order_traverse(t_node * start, void (*func)(t_data)){
    if(start == NULL){
        return SUCCESS;
    }

    if(func == NULL){
        return ERROR;
    }

    if(node_pre_order_traverse((start -> left), func)){
        return ERROR;
    }

    func(start -> value);

    if(node_pre_order_traverse((start -> right), func)){
        return ERROR;
    }

    return SUCCESS;
}

static int node_pos_order_traverse(t_node * start, void (*func)(t_data)){
    if(start == NULL){
        return SUCCESS;
    }

    if(func == NULL){
        return ERROR;
    }

    if(node_pre_order_traverse((start -> left), func)){
        return ERROR;
    }

    if(node_pre_order_traverse((start -> right), func)){
        return ERROR;
    }

    func(start -> value);

    return SUCCESS;
}