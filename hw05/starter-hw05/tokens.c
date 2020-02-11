// tokens.c

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "svec.h"


int main(int argc, char* argv[]) {
    
    svec* sv = make_svec();
    

    svec_push_back(sv, "there");
    svec_push_back(sv, "hi");
    svec_push_back(sv, "good");
    svec_push_back(sv, "fuckin");
    svec_push_back(sv, "buddy");
    
    svec_put(sv, 3, "friendly");
    svec_swap(sv, 0, 1);
    
    for (int ii = 0; ii < sv->spaces; ii++) {
        printf("%s\n", svec_get(sv, ii));
    }

    printf("\nReverse! Reverse!\n");

    svec_reverse(sv);

    for (int ii = 0; ii < sv->spaces; ii++) {
        printf("%s\n", svec_get(sv, ii));
    }

    free_svec(sv);

}

int isOp(char x) {

    return ((x == '&') || (x == '<') || (x == '>') || (x == '|') || (x == ';'));

}

int add_token(svec* sv, const char* line, int start) {

    int numChars = 0;

    while (isOp(line[numChars + start])) {
        
    }
    
}

svec* tokenize(const char* line) {
/*
 *     walk through line
 *     store non-space/non-operator characters in a char[]
 *     (write is-operator helper function)
 *     when reaching a space or op, move the items in th char[] to an svec as one string
 *     (look into memset for clearing the accumulator)
 */

    svec* sv make_svec();

    for (int ii = 0; ii < strlen(line); ii++) {

        if (isspace(line[ii])) {
            continue;
        }
        else {
            ii = ii + add_token(sv, line, ii);
        }
    }

    svec_reverse(sv);

    return sv;
}



/* TODO:

   while (1) {
     printf("tokens$ ");
     fflush(stdout);
     line = read_line()
     if (that was EOF) {
        exit(0);
     }
     tokens = tokenize(line);
     foreach token in reverse(tokens):
       puts(token)
   }

*/




