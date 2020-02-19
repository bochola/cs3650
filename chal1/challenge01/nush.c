#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "svec.h"
#include "tokens.h"
#include "astree.h"
#include "implement.h"

int main_helper(FILE* input);

int main(int argc, char* argv[]) {
    char cmd[256];

    if (argc == 1) {
        while (1) {
             
            printf("nush$ ");
            fflush(stdout);
            main_helper(stdin);
        }
    }
    else {
        
        FILE* file = fopen(argv[1], "r");
        int i = 0;
        while (1) {
            //printf("%d\n", i++);
            main_helper(file); 
        }
    }

    return 0;
}

int main_helper(FILE* input) {
    char cmd[256];

    char* read_in = fgets(cmd, 256, input);
    if (!read_in) {
        exit(0);
    }
    svec* tokens = tokenize(cmd);
    //svec_print(tokens, " ");
    astree* parsed = parse(tokens);
    //print_astree(parsed, 0);
    //printf("Executing: ");
    //svec_print(tokens, " ");
    //printf("\n");
    execute(parsed);
    //printf("after execute\n");

    free_svec(tokens);
    free_astree(parsed);

}
