// Author: Nat Tuck
// CS3650 starter code

#ifndef FLOAT_VEC_H
#define FLOAT_VEC_H

typedef struct floats {
    int size;
    int cap;
    float* data;
} floats;

floats* floats_make(int nn);
void floats_free(floats* fs);

int   floats_size(floats* fs);
float floats_get(floats* fs, int ii);
void  floats_puts(floats* fs, int ii, float num);
void  floats_push(floats* xs, float xx);
void  floats_swap(floats* fs, int ii, int jj);
void  floats_print(floats* xs);

#endif
