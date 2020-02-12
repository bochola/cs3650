// tokens.c

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "tokens.h"
#include "svec.h"

int is_op(char x) {

    return ((x == '&') || (x == '<') || (x == '>') || (x == '|') || (x == ';'));

}

int add_op(svec* sv, const char* line, int start) {

    int num_chars = 0;
    
    while ((num_chars + start) < strlen(line)) {
        
        if (is_op(line[num_chars + start])) {
            num_chars++;
        }
        else {
            break;
        }
    }
    
    // The following three lines are adapted from Nat Tuck's tokenize.c "read_number" function
    // I couldn't figure out a different way to copy over a string section from an existing
    // string.
    
    // Allocates enough memory to hold the string of valid operators
    char* op_list = malloc(num_chars + 1);
    // Copies over the characters from line into opList
    memcpy(op_list, line + start, num_chars);
    // Addds the null character to the end of the string
    op_list[num_chars] = 0;
    
    svec_push_back(sv, op_list);
    free(op_list);
    
    return num_chars;

}

int add_token(svec* sv, const char* line, int start) {

    int num_chars = 0;

    while ((num_chars + start) < strlen(line)) {
        
        char token_index = line[num_chars + start];
        
        if (!(is_op(token_index) || (isspace(token_index)))) {
            num_chars++;
        }
        else {
            break;
        }
    }

    // The following three lines are adapted from Nat Tuck's tokenize.c "read_number" function
    // I couldn't figure out a different way to copy over a string section from an existing
    // string.

    // Allocates enough memory to hold the token string
    char* token = malloc(num_chars + 1);
    // Copies over the characters from line into token
    memcpy(token, line + start, num_chars);
    // Addds the null character to the end of the string
    token[num_chars] = 0;

    svec_push_back(sv, token);
    free(token);

    return num_chars;

}

svec* tokenize(const char* line) {
/*
 *     walk through line
 *     store non-space/non-operator characters in a char[]
 *     (write is-operator helper function)
 *     when reaching a space or op, move the items in th char[] to an svec as one string
 *     (look into memset for clearing the accumulator)
 */

    svec* sv = make_svec();

    for (int ii = 0; ii < strlen(line);) {

        if (isspace(line[ii])) {
            ii++; 
        }
        else if (isOp(line[ii])) {
            ii = ii + add_op(sv, line, ii);
        }
        else {
            ii = ii + add_token(sv, line, ii);

        }
    }

    return sv;
}

int main(int _argc, char* _argv[]) {

    char line[256];
    while (1) {
         
        printf("tokens$ ");
        fflush(stdout);
        char* read_in = fgets(line, 256, stdin);
        if (!read_in) {
            exit(0);
        }
        svec* tokens = tokenize(line);
        svec_reverse(tokens);
        svec_print(tokens);
        free_svec(tokens);
   }

}



