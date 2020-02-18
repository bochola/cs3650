#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "svec.h"
#include "tokens.h"
#include "astree.h"
#include "implement.h"

int main(int argc, char* argv[]) {
    char cmd[256];

    if (argc == 1) {
        while (1) {
             
            printf("nush$ ");
            fflush(stdout);
            char* read_in = fgets(cmd, 256, stdin);
            if (!read_in) {
                exit(0);
            }
            svec* tokens = tokenize(cmd);
            astree* parsed = parse(tokens);
            //print_astree(parsed, 0);
            execute(parsed);

            free_svec(tokens);
            free_astree(parsed);
        }
    }
    else {
        memcpy(cmd, "echo", 5);
    }

    return 0;
}
