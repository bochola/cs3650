// length-sort.c

#include <stdio.h>
#include <string.h>
#include "svec.h"

void sort(svec* xs)
{
    printf("Entering sort");
    // TODO: sort input vector by string length
    // see "man 3 strlen"
    if (xs->size > 1) {
        for (int ii = 0; ii < xs->size;) {
            // The following if-else clause was adapted from the wikipedia page for gnome sort
            // https://en.wikipedia.org/wiki/Gnome_sort
            if ((ii == 0) || (strlen(xs->data[ii]) >= strlen(xs->data[ii - 1]))) {
                ii++;
            }
            else {
                svec_swap(xs, ii, ii - 1);
                ii--;
            }
        }
    }
}

void chomp(char* text)
{
    printf("Chomping");
    // TO-DONE: Modify input string to remove trailing newline ('\n')
    int length = strlen(text);
    char last = text[length - 1];
    char next = text[length - 2];

    if((last == 110) && (next == 92)) { // ascii codes for "n" and "\" respectively
        text[length - 1] = '0'; // changes \n to the nul terminator \0
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Usage:\n  %s input-file\n", argv[0]);
        return 1;
    }
    
    FILE* fh = fopen(argv[1], "r");
    if (!fh) {
        perror("open failed");
        return 1;
    }
    
    svec* xs = make_svec();
    char temp[128];
    
    //What does this while loop do? I havent touched it other than the printf
    while (1) {
        printf("Entering while loop in main()");
        char* line = fgets(temp, 128, fh);
        if (!line) {
            break;
        }
        
        chomp(line);
        svec_push_back(xs, line);
    }
    
    fclose(fh);
    
    sort(xs);
    
    for (int ii = 0; ii < xs->size; ++ii) {
        char* line = svec_get(xs, ii);
        printf("%s\n", line);
    }

    free_svec(xs);
    return 0;
}
