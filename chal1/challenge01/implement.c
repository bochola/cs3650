// implement.c

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "svec.h"
#include "astree.h"
#include "implement.h"

#define SEMICOLON 1
#define BACKGROUND 2
#define OR 3
#define AND 4
#define PIPE 5
#define RDR_IN 6
#define RDR_OUT 7

int execute_cmd(astree* ast) {

    if (ast->cmd) {
        char* input = svec_get(ast->cmd, 0);

        if (strcmp(input, "cd") == 0) {
            int change = chdir(svec_get(ast->cmd, 1));
            
            if (change == -1) {
                printf("Invalid directory\n");
            }
            
            return change;
            
        }

        if (strcmp(input, "exit") == 0) {
            free_astree(ast);
            exit(0);
        }
    }

    int cpid;
    //printf("Execute fork...\n");

    if ((cpid = fork())) {
        // parent process
        //printf("Parent pid: %d\n", getpid());
        //printf("Parent knows child pid: %d\n", cpid);

        int status;
        waitpid(cpid, &status, 0);
        return WEXITSTATUS(status); 
    }
    else {
        // child process
        
        int cmd_length = svec_length(ast->cmd);
        char** cmd = malloc((1 + cmd_length) * sizeof(char*));

        for (int ii = 0; ii < cmd_length; ++ii) {
            cmd[ii] = svec_get(ast->cmd, ii);
        }

        // The argv array for the child.
        // Terminated by a null pointer.
        cmd[cmd_length] = 0;

        execvp(svec_get(ast->cmd, 0), cmd);
        free(cmd);
        exit(1);
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
        
        case RDR_IN:
            return rdr_in_cmd(ast);
        
        case RDR_OUT:
            return rdr_out_cmd(ast);
        
        default:
            // what to do in default case?
            printf("Should never get here, default case in run() in implement.c");
            return 1; // ??? Maybe??
    }
}


int execute(astree* ast) {
    
    if (!ast) {
        return 0;
    }

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
        else if ((strcmp(ast->op, "<")) == 0) {
            op = 6;
        }
        else if ((strcmp(ast->op, ">")) == 0) {
            op = 7;
        }
        
        return run(ast, op);
    }
    else {
        return execute_cmd(ast);
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
    //print_astree(ast, 0);
    int cpid;

    if ((cpid = fork())) {
        
       if (ast->branch2) {
           //printf("Branch2 executing...\n");
           return execute(ast->branch2);
       }

       return 0;
    }
    else {
        execute(ast->branch1);
        //printf("Exited\n");
        _exit(0);
    }
    assert(0);
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
    // Take the output from branch1 and use it as the input for branch2
    int cpid1;

    if ((cpid1 = fork())) {
        //parent 1
        int status_child1;
        waitpid(cpid1, &status_child1, 0);
        return WEXITSTATUS(status_child1);
    }
    else {
        int pipe_fd[2]; //branch2 input, branch1 output
        pipe(pipe_fd);
        int writing_fd = pipe_fd[1]; 
        int reading_fd = pipe_fd[0]; 
        //child 1
        int cpid2;

        if ((cpid2 = fork())) {
            // parent 2 = child 1
            // Get branch2 input file descriptor
            close(writing_fd); 
            close(0);
            dup(reading_fd);
            close(reading_fd);

            int exit_branch2 = execute(ast->branch2);
            int status_child2;
            waitpid(cpid2, &status_child2, 0);
            exit(exit_branch2);
        }
        else {
            // Child 2
            // Get branch1 output file descriptor
            close(reading_fd);
            close(1);
            dup(writing_fd);
            close(writing_fd);

            int exit_branch1 = execute(ast->branch1);
            exit(exit_branch1);
        }
    }
    return 0;
}

int rdr_in_cmd(astree* ast) {
    // Take branch2 and use it as an argument for branch1
    
    int cpid;

    if ((cpid = fork())) {
        
        int status;
        waitpid(cpid, &status, 0);
        return WEXITSTATUS(status);
    }
    else {

        char* path = svec_get(ast->branch2->cmd, 0);
        int fd = open(path, O_RDONLY);
        close(0);
        dup(fd);
        close(fd);

        int exit_status = execute(ast->branch1);
        exit(exit_status);
    }
    assert(0); 
}

int rdr_out_cmd(astree* ast) {
    // Take the output of branch1 and send it to a file named in branch2

    int cpid;

    if ((cpid = fork())) {
        
        int status;
        waitpid(cpid, &status, 0);
        return WEXITSTATUS(status);
    }
    else {

        char* path = svec_get(ast->branch2->cmd, 0);
        int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        close(1);
        dup(fd);
        close(fd);

        execute(ast->branch1);
    }

    return 0;
}
