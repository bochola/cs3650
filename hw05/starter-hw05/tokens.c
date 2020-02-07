// tokens.c

#include <stdlib.h>
#include <stdio.h>

#include "svec.h"


int main(int argc, char* argv[]) {
    
    svec* sv = make_svec();
    
    svec_push_back(sv, "hi");

    printf("%s\n", svec_get(sv, 0));

}

/*
 * tokenize(const char* line) {
 *
 *     walk through line
 *     store non-space/non-operator characters in a char[]
 *     (write is-operator helper function)
 *     when reaching a space or op, move the items in th char[] to an svec as one string
 *     (look into memset for clearing the accumulator)
 *     
 *
 *
 */



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




