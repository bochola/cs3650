// astree.h

// The header file for an abstract syntax tree

#ifndef ASTREE_H
#define ASTREE_H


typedef struct astree {
    
    //An astree is one of:
    //      char* op
    //      char* cmd

    // An operator is one of (in order of operations): ;, &, ||, &&, |, <, >
    char* op;
    
    // A command is anything else that is not an op and not a space
    char* cmd; 
    
    
    
    
} astree;



#endif
