// implement.c

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "svec.h"
#include "astree.h"
#include "implement.h"

#define SEMICOLON 1
#define BACKGROUND 2
#define OR 3
#define AND 4
#define PIPE 5
#define RDR_FROM 6
#define RDR_TO 7

int execute_ast(astree* ast) {

    if (ast->cmd) {
        char* input = svec_get(ast->cmd, 0);

        if (strcmp(input, "cd") == 0) {
            // TODO: Figure out how to change directories from within a C program
        }

        if (strcmp(input, "exit") == 0) {
            // TODO: free astree
            exit(0);
        }
    }

    int cpid;

    if ((cpid = fork())) {
        // parent process
        printf("Parent pid: %d\n", getpid());
        printf("Parent knows child pid: %d\n", cpid);

        int status;
        waitpid(cpid, &status, 0);

        printf("== executed program complete ==\n");

        printf("child returned with wait code %d\n", status);
        if (WIFEXITED(status)) {
            printf("child exited with exit code (or main returned) %d\n", WEXITSTATUS(status));
        }
    }
    else {
        // child process
        printf("Child pid: %d\n", getpid());
        printf("Child knows parent pid: %d\n", getppid());
        
        int cmd_length = svec_length(ast->cmd);

        char** cmd = malloc(cmd_length * sizeof(char*) + 1);

        for (int ii = 0; ii < svec_length(ast->cmd); ++ii) {
            cmd[ii] = svec_get(ast->cmd, ii);
        }

        // The argv array for the child.
        // Terminated by a null pointer.
        cmd[svec_length(ast->cmd)] = 0;

        printf("== executed program's output: ==\n");

        //execvp(svec_get(ast->cmd), cmd);
        printf("Can't get here, exec only returns on error.");
    }
}


int run(astree* ast, int op) {
    
    switch(op) {

        case SEMICOLON:
            return semicolon_cmd(ast);    
            
        case BACKGROUND:
            return background_cmd(ast);
        
        case OR:
            return or_cmd(ast);
        
        case AND:
            return and_cmd(ast);
        
        case PIPE:
            return pipe_cmd(ast);
        
        case RDR_FROM:
            return rdr_from_cmd(ast);
        
        case RDR_TO:
            return rdr_to_cmd(ast);
        
        default:
            // what to do in default case?
            return 1; // ??? Maybe??
    }
}


int execute(astree* ast) {
    
    int op = 0;
    
    if (ast->op) {
        if ((strcmp(ast->op, ";")) == 0) {
            op = 1;
        }
        else if ((strcmp(ast->op, "&")) == 0) {
            op = 2;
        }
        else if ((strcmp(ast->op, "||")) == 0) {
            op = 3;
        }
        else if ((strcmp(ast->op, "&&")) == 0) {
            op = 4;
        }
        else if ((strcmp(ast->op, "|")) == 0) {
            op = 5;
        }
        else if ((strcmp(ast->op, ">")) == 0) {
            op = 6;
        }
        else if ((strcmp(ast->op, "<")) == 0) {
            op = 7;
        }
        
        return run(ast, op);
    }
    else {
        execute_ast(ast);
    }
}


int semicolon_cmd(astree* ast) {
    // Do first command, then do the second (if there)    
    int first_cmd = execute(ast->branch1);

    if (ast->branch2) {
        return execute(ast->branch2);
    }
    else {
        return first_cmd;
    }
}

int background_cmd(astree* ast) {
    // Do whatever happens in the next command in the background (??)
    return 0;
}

int or_cmd(astree* ast) {
    // Run the first command, and if it completes with a good exitcode, dont run the second

    int first_cmd = execute(ast->branch1);

    if (first_cmd == 1) {
        return execute(ast->branch2);
    }
    else {
        return first_cmd;
    }
    
}

int and_cmd(astree* ast) {
    // Run the first command and if it was good, then run the second

    int first_cmd = execute(ast->branch1);

    if (first_cmd == 0) {
        return execute(ast->branch2);
    }
    else {
        return first_cmd;
    }
}

int pipe_cmd(astree* ast) {
    // Take the output from branch1 and use it as the argument for branch2
    return 0;
}

int rdr_from_cmd(astree* ast) {
    // Take branch2 and use it as an argument for branch1

    return 0;
}

int rdr_to_cmd(astree* ast) {
    // Take the output of branch1 and send it to a file named in branch2

    return 0;
}
