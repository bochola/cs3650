/* This file is lecture notes from CS 3650, Fall 2018 */
/* Author: Nat Tuck */

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "svec.h"

svec* make_svec()
{
    svec* sv = malloc(sizeof(svec));      // sv is a pointer to an svec
    sv->data = malloc(2 * sizeof(char*)); // sv->data is a pointer to 2 slots of char* (strings)
    sv->size = 2;     // total size of the array
    sv->spaces = 0;   // number of filled spaces in the array
    // TO-DONE(?): correctly allocate and initialize data structure
    printf("Created svec sucessfully\n");
    return sv;
}

void free_svec(svec* sv)
{
    // TO-DONE: free all allocated data
    // Will need to free each item inside sv->data, then free sv
    if (sv)
    {
        printf("Freeing svec data...\n");
        
        int ii;
        for (ii = 0; ii < sv->size; ii++)
        {
            free(sv->data[ii]);
        }
        
        printf("Freeing svec...\n");    
        free(sv);
        printf("Freed\n");
    }
    
}

char* svec_get(svec* sv, int ii)
{
    assert((ii >= 0) && (ii < sv->size));
    printf("Retrived data at location  %ld\n", ii);
    return sv->data[ii];
}

void svec_put(svec* sv, int ii, char* item)
{
    // TO-DONE: insert the given item into svec
    // Consider ownership of string in collection.
    
    assert(ii >= 0 && ii < sv->size);
    // Use strdup here on item
    char* copy = strdup(item);  // since strdup uses malloc, you need to free i think
    
    if (strcmp(sv->data[ii], "\0") == 0) {  // If there is nothing there
        printf("Placing data at location  %ld\n", ii);
        sv->data[ii] = copy;
        sv->spaces++;
    }
    else {
        printf("Replacing data at location %ld\n", ii);
        sv->data[ii] = copy;
    }
    
}

void svec_push_back(svec* sv, char* item)
{
    // TO-DONE: expand vector if backing array is not big enough
    
    int ii = sv->size;
    
    
    if (strcmp(sv->data[ii - 1], "\0") == 0)
    {
        printf("Adding item...\n");
        svec_put(sv, ii - 1, item);
    }
    else
    {
        printf("No free spaces. Reallocating...\n");
        int newSize = sv->size * 2;
        // I could add in a clause here checking if spaces < size
        // But that feels redundant
        sv->data  = realloc(sv->data, newSize * sizeof(char*));
        svec_put(sv, ii,  item);
    }
    
    sv->spaces++;
    printf("Added.\n");
}

void svec_swap(svec* sv, int ii, int jj)
{
    // TO-DONE: Swap the items in slots ii and jj
    
    char* slot1 = svec_get(sv, ii);
    char* slot2 = svec_get(sv, jj);
    
    svec_put(sv, jj, slot1);
    svec_put(sv, ii, slot2);
    
    printf("Swapping location %ld with %ld\n", ii, jj);
    
}
