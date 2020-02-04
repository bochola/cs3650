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
    sv->size = 2;
    // TO-DONE(?): correctly allocate and initialize data structure
    prinf("Created svec sucessfully");
    return sv;
}

void free_svec(svec* sv)
{
    // TO-DONE: free all allocated data
    // Will need to free each item inside sv->data, then free sv
    
    printf("Freeing svec data...");
    
    for (int ii = 0; ii < sv->size; ii++)
    {
        free(sv->data[ii]);
    }
    
    printf("Freeing svec...");    
    free(sv);
    printf("Freed");
    
}

char* svec_get(svec* sv, int ii)
{
    assert((ii >= 0) && (ii < sv->size));
    printf("Retrived data at location  %ld", ii);
    return sv->data[ii];
}

void svec_put(svec* sv, int ii, char* item)
{
    assert(ii >= 0 && ii < sv->size);
    // Use strdup here on item
    char* copy = strdup(item);       // since strdup uses malloc, you need to free
    printf("Placing data at location  %ld", ii);
    sv->data[ii] = copy;             // after you call it. i think
    // TO-DONE: insert the given item into svec
    // Consider ownership of string in collection.
}

void svec_push_back(svec* sv, char* item)
{
    int ii = sv->size;

    // TODO: expand vector if backing array is not big enough
    
    // if(last-is-empty?){
    //     svec_put(sv, ii - 1, item);
    // }
    // else{
    //     int newSize = sv->size * 2;
    //     char** temp = realloc(sv->data, newSize);
    //     
    //     // wait so what do i do here? how do i change sv->data to be temp? is it just
    //     sv->data = temp;
    //     svec_put(sv, ii,  item);
    // }
    
    
}

void svec_swap(svec* sv, int ii, int jj)
{
    // TO-DONE: Swap the items in slots ii and jj
    
    char* slot1 = svec_get(sv, ii);
    char* slot2 = svec_get(sv, jj);
    
    svec_put(sv, jj, slot1);
    svec_put(sv, ii, slot2);
    
}
