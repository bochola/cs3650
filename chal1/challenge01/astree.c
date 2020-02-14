// astree.c


#include <stdio>
#include <string.h>
#include <stdlib.h>

#include "astree.h"


astree* make_cmd(char* cmd) {
    
    astree* ast = malloc(sizeof(astree));
    ast->op = 0;
    ast->arg1 = 0;
    ast->arg2 = 0;
    
    if (cmd) {
        ast->cmd = cmd;
    }
    else {
        //Maybe exit(0); or something?
    }
    
}

astree* make_op(char* op, astree* arg1, astree* arg2) {
    
    astree* ast = malloc(sizeof(astree));
    ast->op = op;
    ast->arg1 = 0;
    ast->arg2 = 0;

    if (cmd) {
        ast->cmd = cmd;
    }
    else {
        //Maybe exit(0); or something?
    }
    
    if (arg1) {
        ast->arg1 = arg1;
    }
    else {
        //Maybe exit(0); or something?
    }
    
    if (arg2) {
        ast->arg2 = arg2;
    }
    else {
        //Maybe exit(0); or something?
    }
    
}
