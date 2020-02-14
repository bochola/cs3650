// astree.c


#include <stdio>
#include <string.h>
#include <stdlib.h>

#include "astree.h"


astree* make_cmd(svec* cmd) {
    
    astree* ast = malloc(sizeof(astree));
    ast->op = 0;
    ast->arg1 = 0;
    ast->arg2 = 0;
    ast->cmd = cmd;
    
}

astree* make_op(char* op, astree* arg1, astree* arg2) {
    
    astree* ast = malloc(sizeof(astree));
    ast->op = op;
    ast->arg1 = 0;
    ast->arg2 = 0;
    ast->op = op;
    ast->arg1 = arg1;
    ast->arg2 = arg2;
    
}

astree* parse(svec* tokens) {
    // An operator is one of (in order of operations): ;, &, ||, &&, |, >, <
    // Search through tokens svec and look for operators in order of importance
    // If an operator is found, make a new astree of that operator, a sub svec on
    // the items to the left and right and parse on those two
    //      
    //      int index = svec_index_of(char* op);
    //      svec* arg1 = sub_svec(tokens, 0, index);
    //      svec* arg2 = sub_svec(tokens, index + 1, svec_length(tokens));
    //      make_op(char* op, parse(arg1), parse(arg2)
    // 
    // If that operator is not found, then move to the next most important 
    // operator. Repeat until reaching the end of the svec
   
    int index = svec_index_of(tokens, ";"); 

    if (index >= 0) {
        svec* arg1 = sub_svec(tokens, 0, index);
        svec* arg2 = sub_svec(tokens, index + 1, svec_length(tokens) - 1);
        make_op(";", parse(arg1), parse(arg2));
    }

}

void print_astree(astree* ast, int acc) {
    
    
    
}
