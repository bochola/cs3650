// implement.h

#ifndef IMPLEMENT_H
#define IMPLEMENT_H

#include "astree.h"

int execute(astree* ast);
int execute_cmd(astree* ast);

int run(astree* ast, int op);
int semicolon_cmd(astree* ast);
int background_cmd(astree* ast);
int or_cmd(astree* ast);
int and_cmd(astree* ast);
int pipe_cmd(astree* ast);
int rdr_in_cmd(astree* ast);
int rdr_out_cmd(astree* ast);


#endif
