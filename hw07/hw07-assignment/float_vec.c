// Author: Nat Tuck
// CS3650 starter code

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "float_vec.h"

/*
typedef struct floats {
    long size;
    long cap;
    float* data;
} floats;
*/

floats* floats_make()
{
    floats* xs = malloc(sizeof(floats));
    xs->size = 0;
    xs->cap  = 1;
    xs->data = malloc(xs->cap * sizeof(float));
    return xs;
}

void floats_push(floats* xs, float xx)
{
    if (xs->size >= xs->cap) {
        xs->cap *= 2;
        xs->data = realloc(xs->data, xs->cap * sizeof(float));
    }

    xs->data[xs->size] = xx;
    xs->size += 1;
}

void floats_free(floats* xs)
{
    if (xs) {
        free(xs->data);
        free(xs);
    }
}


// ----------------------------------

// Returns the number of items in the vector (1 index, not 0 index)
int floats_size(floats* fs) {

    return fs->size;
}

float floats_largest(floats* fs) {
    
    float largest = fs->data[0];

    for (int i = 0; i < fs->size; i++) {
        
        if (fs->data[i] > largest) {
            largest = fs->data[i];
        }
    }

    return largest;
    
}

float floats_smallest(floats* fs) {

    float smallest = fs->data[0];

        for (int i = 0; i < fs->size; i++) {

            if (fs->data[i] < smallest) {
                smallest = fs->data[i];
            }
        }

    return smallest;
}


// Returns the item at the given address
float floats_get(floats* fs, int ii) {
    assert((ii >= 0) && (ii < fs->size));
    return fs->data[ii];
}

// Places a float at the given address
void floats_put(floats* fs, int ii, float num) {

    assert(ii >= 0 && ii < fs->size);
    fs->data[ii] = num;
}

// Switches the items in the requested slots
void floats_swap(floats* fs, int ii, int jj) {

    float slot1 = floats_get(fs, ii);
    float slot2 = floats_get(fs, jj);

    fs->data[ii] = slot2;
    fs->data[jj] = slot1;

}

// Prints the given floats
void floats_print(floats* fs, char* separator) {
    for (int ii = 0; ii < fs->size; ii++) {
        printf("%.0f%s", floats_get(fs, ii), separator);
        fflush(stdout);
    }
}
