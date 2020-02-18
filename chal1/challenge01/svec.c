/* This file is lecture notes from CS 3650, Fall 2018 */
/* Author: Nat Tuck */

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "svec.h"

// Creates an svec
svec* make_svec() {
    svec* sv = malloc(sizeof(svec));      // sv is a pointer to an svec
    sv->data = calloc(2, sizeof(char*)); // sv->data is a pointer to 2 slots of char* (strings)
    sv->size = 2;     // total size of the array
    sv->spaces = 0;   // number of filled spaces in the array
    // TO-DONE(?): correctly allocate and initialize data structure
    //printf("Created svec sucessfully\n");
    return sv;
}

// Frees the memory that the given svec is taking up
void free_svec(svec* sv) {
    
    if (sv)
    {
       // printf("Freeing svec data...\n");
        
        for (int ii = 0; ii < sv->spaces; ii++)
        {
            free(sv->data[ii]);
        }
        
        //printf("Freeing svec...\n");    
        free(sv->data);
        free(sv);
        //printf("Freed\n");
    }
    
}

// Returns the number of items in the vector (1 index, not 0 index)
int svec_length(svec* sv) {
    
    return sv->spaces;
}

// Returns the item at the given address
char* svec_get(svec* sv, int ii) {
    //printf("Svec length: %d\n", svec_length(sv));
    //printf("Getting location %d\n", ii);    
    assert((ii >= 0) && (ii < sv->spaces));
    //printf("Retrived data at location  %d\n", ii);
    return sv->data[ii];
}

// Returns the index of a given string
int svec_index_of(svec* sv, char* item) {
    
    for (int ii = 0; ii < sv->spaces; ii++) {
        
        if ((strcmp(sv->data[ii], item)) == 0) {
            return ii;
        }
    }
    
    return -1;
}
// Places a string at the given address
void svec_put(svec* sv, int ii, char* item) {

    assert(ii >= 0 && ii < sv->spaces);
    char* copy;
    if (item) {
        copy = strdup(item);  // since strdup uses malloc, you need to free i think
    }
    else {
        copy = item;
    }

    if (sv->data[ii]) {  // If there is something there
        free(sv->data[ii]);
    }
    
    sv->data[ii] = copy;
}

// Adds the given item to the end of the vector, expanding if necessary
void svec_push_back(svec* sv, char* item) {
    // TO-DONE: expand vector if backing array is not big enough
    int ii = sv->spaces;
    
    if (sv->spaces >= sv->size) {
        int newSize = sv->size * 2;
        sv->data  = realloc(sv->data, newSize * sizeof(char*));
        memset(sv->data + sv->size, 0, sv->size * sizeof(char*));
        sv->size = newSize;
    }
    
    sv->spaces++;
    svec_put(sv, ii, item);
}

// Switches the items in the requested slots
void svec_swap(svec* sv, int ii, int jj) {
    
    char* slot1 = svec_get(sv, ii);
    char* slot2 = svec_get(sv, jj);
    
    sv->data[ii] = slot2;
    sv->data[jj] = slot1;
    
    //printf("Swapping location %d with %d\n", ii, jj);
    
}

// Reverses the order of the items in the vector
void svec_reverse(svec* sv) {
    for (int ii = 0; ii < (sv->spaces / 2); ii++) {
        int jj = sv->spaces - ii - 1;
        svec_swap(sv, ii, jj);
    }
}

// Prints out the contents of sv->data
void svec_print(svec* sv, char* separator) {
    for (int ii = 0; ii < sv->spaces; ii++) {
        printf("%s%s", svec_get(sv, ii), separator);
        fflush(stdout);
    }
}

// Creates a new vector comprised of a section of items from sv
svec* sub_svec(svec* sv, int start_index, int end_index) {
    
    assert((end_index <= sv->spaces) &&  (start_index >= 0));
        
    svec* new_sv = make_svec();
    for (int ii = start_index; ii < end_index; ii++) {
            
        svec_push_back(new_sv, svec_get(sv, ii));
        
    }
    
    return new_sv;
}

svec* svec_clone(svec* sv) {
    return sub_svec(sv, 0, sv->spaces);
}
