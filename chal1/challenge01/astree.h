// astree.h

// The header file for an abstract syntax tree

#ifndef ASTREE_H
#define ASTREE_H

#include <stdlib.h>
#include "svec.h"

typedef struct astree {
    
    //An astree is one of:
    //      char* op
    //      char* cmd

    // An operator is one of (in order of operations): ;, &, ||, &&, |, <, >
    char* op;
    
    // A command is anything else that is not an op
    svec* cmd; 
    
    // The other branches of the tree
    struct astree* branch1;
    struct astree* branch2;
    
    
} astree;

void free_astree(astree* ast);
astree* make_cmd(svec* cmd);
astree* make_op(svec* op, astree* branch1, astree* branch2);

astree* parse(svec* token_list);

#endif
