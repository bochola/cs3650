// float_vec.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "float_vec.h"

// Creates a floats struct
floats* floats_make() {

    floats* fs = malloc(sizeof(floats));  // fs is a pointer to a floats
    fs->cap = 1;
    fs->data = calloc(fs->cap, sizeof(float)); // fs->data is a pointer to [capacity] slots of float
    fs->size = 0;   // number of filled spaces in the array
    return fs;
}

// Frees the memory that the given floats is taking up
void floats_free(floats* fs) {

    if (fs)
    {
        // I shouldnt need to free the data entries caus theyre just numbers, right?
       // for (int ii = 0; ii < fs->size; ii++)
       // {
       //     if (fs->data[ii]) {
       //         free(fs->data[ii]);
       //     }
       // }

        free(fs->data);
        free(fs);
    }

}

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


// Adds the given item to the end of the vector, expanding if necessary
void floats_push(floats* fs, float num) {
    //printf("Entered floats_push\n");
    // expand vector if backing array is not big enough
    int ii = fs->size;

    if (fs->size >= fs->cap) {
        int new_cap = fs->cap * 2;
        int pls_cpy = fs->cap * sizeof(float);
        
        void* data_ptr = realloc(fs->data, new_cap * sizeof(float));
        memcpy(data_ptr, fs->data, pls_cpy);
        fs->data = data_ptr;
        fs->cap = new_cap;
        // Can I free data_ptr here? Or would that mess with fs->data
    }

    fs->size++;
    floats_put(fs, ii, num);
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
        printf("%f%s", floats_get(fs, ii), separator);
        fflush(stdout);
    }
}
