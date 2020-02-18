// astree.c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "astree.h"

astree* make_cmd(svec* cmd) {
    
    astree* ast = malloc(sizeof(astree));
    ast->op = 0;
    ast->branch1 = 0;
    ast->branch2 = 0;
    ast->cmd = svec_clone(cmd);
    
}

astree* make_op(char* op, astree* branch1, astree* branch2) {
    
    astree* ast = malloc(sizeof(astree));
    ast->op = strdup(op);
    ast->branch1 = branch1;
    ast->branch2 = branch2;
    ast->cmd = 0;
    
}

void free_astree(astree* ast) {
    
    if (ast->op) {
        free(ast->op);
    }
    if (ast->cmd) {
        free_svec(ast->cmd);
    }
    if (ast->branch1) {
        free_astree(ast->branch1);
    }
    if (ast->branch2) {
        free_astree(ast->branch2);
    }

    free(ast);
}

astree* parse(svec* tokens) {
    // An operator is one of (in order of operations): ;, &, ||, &&, |, >, <
    // Search through tokens svec and look for operators in order of importance
    // If an operator is found, make a new astree of that operator, a sub svec on
    // the items to the left and right and parse on those two
    // If that operator is not found, then move to the next most important 
    // operator. Repeat until reaching the end of the svec
    
    if (svec_length(tokens) < 1) {
        free_svec(tokens);
        return 0;
    }
    
        
    char* op_list[] = {";", "&", "||", "&&", "|", ">", "<"};
    
    for (int ii = 0; ii < 7; ii++) {

        char* op = op_list[ii];

        int index = svec_index_of(tokens, op);

        if (index >= 0) {
            svec* branch1 = sub_svec(tokens, 0, index);
            svec* branch2 = sub_svec(tokens, index + 1, svec_length(tokens));
            astree* tree = make_op(op, parse(branch1), parse(branch2));
            free_svec(branch1);
            free_svec(branch2);

            return tree;
        }
    }

    return make_cmd(tokens);
}

void print_astree(astree* ast, int acc) {
   // acc is the accumulator to count how many layers deep a tree is

    for (int ii = 0; ii <= acc; ii++) {
        printf("    ");
    }

    if (ast->op) {
        printf("%s\n", ast->op);
        print_astree(ast->branch1, acc + 1);
        print_astree(ast->branch2, acc + 1);
    }

    if (ast->cmd) {
        svec_print(ast->cmd, " ");
        printf("\n");
    }
}

    

