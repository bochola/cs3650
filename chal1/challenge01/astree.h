// astree.h

// The header file for an abstract syntax tree

#ifndef ASTREE_H
#define ASTREE_H

#include <stdlib.h>


typedef struct astree* {
    
    //An astree is one of:
    //      char* op
    //      char* cmd

    // An operator is one of (in order of operations): ;, &, ||, &&, |, <, >
    char* op;
    
    // A command is anything else that is not an op and not a space
    char* cmd; 
    
    // The other branches of the tree
    struct astree* branch1;
    struct astree* branch2;
    
    
} astree;

void free_astree(astree* ast);
astree* make_cmd(char* cmd);
astree* make_op(char* op, astree* arg1, astree* arg2);


#endif
