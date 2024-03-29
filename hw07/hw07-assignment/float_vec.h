// Author: Nat Tuck
// CS3650 starter code

#ifndef FLOAT_VEC_H
#define FLOAT_VEC_H

typedef struct floats {
    long size;
    long cap;
    float* data;
} floats;

floats* floats_make();
void floats_push(floats* xs, float xx);
void floats_free(floats* xs);

int   floats_size(floats* fs);
float floats_largest(floats* fs);
float floats_smallest(floats* fs);
float floats_get(floats* fs, int ii);
void  floats_put(floats* fs, int ii, float num);
void  floats_swap(floats* fs, int ii, int jj);
void  floats_print(floats* xs, char* separator);


#endif
