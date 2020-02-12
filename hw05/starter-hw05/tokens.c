// tokens.c

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "svec.h"

int isOp(char x) {

    return ((x == '&') || (x == '<') || (x == '>') || (x == '|') || (x == ';'));

}

int add_Op(svec* sv, const char* line, int start) {

    int numChars = 0;
    
    while ((numChars + start) < strlen(line)) {
        
        if (isOp(line[numChars + start])) {
            numChars++;
        }
        else {
            break;
        }
    }
    
    // The following three lines are adapted from Nat Tuck's tokenize.c "read_number" function
    // I couldn't figure out a different way to copy over a string section from an existing
    // string.
    
    // Allocates enough memory to hold the string of valid operators
    char* opList = malloc(numChars + 1);
    // Copies over the characters from line into opList
    memcpy(opList, line + start, numChars);
    // Addds the null character to the end of the string
    opList[numChars] = 0;
    
    svec_push_back(sv, opList);
    free(opList);
    
    return numChars;

}

int add_Token(svec* sv, const char* line, int start) {

    int numChars = 0;

    while ((numChars + start) < strlen(line)) {
        
        char tokenIndex = line[numChars + start];
        
        if (!(isOp(tokenIndex) || (isspace(tokenIndex)))) {
            numChars++;
        }
        else {
            break;
        }
    }

    // The following three lines are adapted from Nat Tuck's tokenize.c "read_number" function
    // I couldn't figure out a different way to copy over a string section from an existing
    // string.

    // Allocates enough memory to hold the token string
    char* token = malloc(numChars + 1);
    // Copies over the characters from line into token
    memcpy(token, line + start, numChars);
    // Addds the null character to the end of the string
    token[numChars] = 0;

    svec_push_back(sv, token);
    free(token);

    return numChars;

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
            ii = ii + add_Op(sv, line, ii);
        }
        else {
            ii = ii + add_Token(sv, line, ii);

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



