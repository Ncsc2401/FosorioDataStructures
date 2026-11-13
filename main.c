#include <stdio.h>
#include <string.h>

#include "binary_tree.h"

#define BUFFER_SIZE 256

typedef enum e_op{
    OP_NULL,
    OP_GOTO_LEFT,
    OP_GOTO_RIGHT,
    OP_GOTO_ROOT,
    OP_GOTO_PARENT,
    OP_GOTO_LEFT_MOST,
    OP_GOTO_RIGHT_MOST,
    OP_GOTO_SUCCESSOR,
    OP_GOTO_PREDECESSOR,
    OP_PRINT,
    OP_REMOVE,
    OP_STOP
} t_op;

#define COMMAND_NULL "null"
#define COMMAND_LEFT "left"
#define COMMAND_RIGHT "right"
#define COMMAND_PARENT "parent"
#define COMMAND_LEFT_MOST "left_most"
#define COMMAND_RIGHT_MOST "right_most"
#define COMMAND_SUCCESSOR "successor"
#define COMMAND_PREDECESSOR "predecessor"
#define COMMAND_ROOT "root"
#define COMMAND_PRINT "print"
#define COMMAND_REMOVE "remove"
#define COMMAND_STOP "stop"

t_op string_to_op(char * string);

int compare(float a, float b){
    return b - a;
}

void print(float a, void * context){
    printf("%.2f\n", a);
}

int main(void){
    t_binary_tree * tree = binary_tree_create(compare);

    binary_tree_insert(tree, 5);
    binary_tree_insert(tree, 8);
    binary_tree_insert(tree, 3);
    binary_tree_insert(tree, 2);
    binary_tree_insert(tree, 10);
    binary_tree_insert(tree, 1);
    binary_tree_insert(tree, 6);

    binary_tree_goto(tree, BT_ROOT);

    char buffer[BUFFER_SIZE];
    buffer[0] = '\0';

    while(1){
        fgets(buffer, BUFFER_SIZE, stdin);

        for(size_t i = 0; i < BUFFER_SIZE; i++){
            if(buffer[i] <= 32){
                buffer[i] = '\0';
            }
        }

        int result = 0;
        t_op op = string_to_op(buffer);
        switch (op)
        {
            case OP_NULL:{
                result = binary_tree_goto(tree, BT_NULL);
                break;
            }
            case OP_GOTO_LEFT:{
                result = binary_tree_goto(tree, BT_LEFT);
                break;
            }
            case OP_GOTO_RIGHT:{
                result = binary_tree_goto(tree, BT_RIGHT);
                break;
            }
            case OP_GOTO_ROOT:{
                result = binary_tree_goto(tree, BT_ROOT);
                break;
            }
            case OP_GOTO_PARENT:{
                result = binary_tree_goto(tree, BT_PARENT);
                break;
            }
            case OP_GOTO_LEFT_MOST:{
                result = binary_tree_goto(tree, BT_LEFT_MOST);
                break;
            }
            case OP_GOTO_RIGHT_MOST:{
                result = binary_tree_goto(tree, BT_RIGHT_MOST);
                break;
            }
            case OP_GOTO_SUCCESSOR:{
                result = binary_tree_goto(tree, BT_SUCCESSOR);
                break;
            }
            case OP_GOTO_PREDECESSOR:{
                result = binary_tree_goto(tree, BT_PREDECESSOR);
                break;
            }
            case OP_PRINT:{
                float v;
                result = binary_tree_peek(tree, BT_CURRENT, &v);

                if(!result){
                    printf("%.2f\n", v);
                }

                break;
            }
            case OP_REMOVE:{
                result = binary_tree_remove(tree, BT_CURRENT, NULL);
                break;
            }
            case OP_STOP:{
                return 0;
            }
        }

        if(result){
            printf("ERROR\n");
        }
    }

    return 0;
}

t_op string_to_op(char * string){
    if(string == NULL){
        return OP_NULL;
    }

    if(!strncmp(string, COMMAND_NULL, strlen(COMMAND_NULL))){
        return OP_NULL;
    }
    else if(!strncmp(string, COMMAND_LEFT, strlen(COMMAND_LEFT))){
        return OP_GOTO_LEFT;
    }
    else if(!strncmp(string, COMMAND_RIGHT, strlen(COMMAND_RIGHT))){
        return OP_GOTO_RIGHT;
    }
    else if(!strncmp(string, COMMAND_LEFT_MOST, strlen(COMMAND_LEFT_MOST))){
        return OP_GOTO_LEFT_MOST;
    }
    else if(!strncmp(string, COMMAND_RIGHT_MOST, strlen(COMMAND_RIGHT_MOST))){
        return OP_GOTO_RIGHT_MOST;
    }
    else if(!strncmp(string, COMMAND_SUCCESSOR, strlen(COMMAND_SUCCESSOR))){
        return OP_GOTO_SUCCESSOR;
    }
    else if(!strncmp(string, COMMAND_PREDECESSOR, strlen(COMMAND_PREDECESSOR))){
        return OP_GOTO_PREDECESSOR;
    }
    else if(!strncmp(string, COMMAND_PARENT, strlen(COMMAND_PARENT))){
        return OP_GOTO_PARENT;
    }
    else if(!strncmp(string, COMMAND_ROOT, strlen(COMMAND_ROOT))){
        return OP_GOTO_ROOT;
    }
    else if(!strncmp(string, COMMAND_PRINT, strlen(COMMAND_PRINT))){
        return OP_PRINT;
    }
    else if(!strncmp(string, COMMAND_REMOVE, strlen(COMMAND_REMOVE))){
        return OP_REMOVE;
    }
    else if(!strncmp(string, COMMAND_STOP, strlen(COMMAND_STOP))){
        return OP_STOP;
    }

    return OP_NULL;

}