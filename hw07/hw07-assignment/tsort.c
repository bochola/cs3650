// ssort.c

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#include "float_vec.h"
#include "barrier.h"
#include "utils.h"


int float_cmp(const void * x, const void * y) {
    
    if (x > y) {
        return 1;
    }
    else if (x < y) {
        return -1;
    }
    else {
        return 0;
    }
}


long in_range(floats* fs, long start, long end) {
    
    long count = 0;
    int size = floats_size(fs);

    for (int i = 0; i < size; i++) {
        
        float test = floats_get(fs, i);

        if ((test >= start) && (test < end)) {
            count++;
        }
    }

    return count;
}

void qsort_floats(floats* fs) {
    // TO-DONE: call qsort to sort the array
    qsort(fs->data, fs->size, sizeof(float), float_cmp);
}

void sort_worker(floats* fs, long* sizes, long bucket_size, int P, FILE* fstream) 
{
    if (sizes[P] > 0) {
    
        floats* samps = floats_make();
        
        long start = P * bucket_size;
        long end = (P + 1) * bucket_size;

        for (int i = 0; i < fs->size; i++) {
            
            float fl = floats_get(fs, i);

            if ((fl >= start) && (fl < end)) {
                floats_push(samps, fl);
            }
        }

        // There is a chance that samps.size != sizes[P]
        // If so, figure out why and fix iti
        
        qsort_floats(samps);
        
        // Here I need to write to memory in the correct location
        
        for (int j = 0; j < P; j++) {
            // address = address + (sizeof(float) * sizes[j]);
        }
        
        // memcpy(address, samps->data, samps->size);
    }
}

void sample_sort(floats* fs, int num_proc, long* sizes, long bucket, FILE* fstream, barrier* bb) {
   
    // TO-DONE: spawn P processes, each running sort_worker

    for (int i = 0; i < num_proc; i++) {
           
        //sort_worker(fs, sizes, bucket, i, fstream);
    }
        
    for (int ii = 0; ii < num_proc; ++ii) {
        // int rv = waitpid(kids[ii], 0, 0);
        // check_rv(rv);
    }
}

int main(int argc, char* argv[]) {
    alarm(120);

    if (argc != 3) {
        printf("Usage:\n");
        printf("\t%s P data.dat\n", argv[0]);
        return 1;
    }

    const int num_proc = atoi(argv[1]);
    const char* fname = argv[2];

    seed_rng();

    int rv;
    struct stat st;
    rv = stat(fname, &st);
    check_rv(rv);

    const int fsize = st.st_size;
    if (fsize < 8) {
        printf("File too small.\n");
        return 1;
    }

    int fd = open(fname, O_RDWR);
    check_rv(fd);

    printf("fsize: %i\n", fsize);

    FILE* fstream = fopen(fname, "w+");

    floats* fs = floats_make();
    void* read_addr = malloc(fsize * 4);
    
    fseek(fstream, 8, SEEK_SET); // Sets the first 8 bytes of fd to be ignored
    fread(read_addr, fsize, 4, fstream); // Reads in fsize bytes

    fs->size = fsize;
    fs->cap = fsize;
    fs->data = (float*) read_addr;
    
    printf("Passed pushing to a floats, maybe successful?\n");

    // Make an array of longs to tell how many floats are in each bucket
    long sizes[num_proc];     

    floats_print(fs, "\n");
    
    float smallest = floats_smallest(fs);
    float largest = floats_largest(fs);

    long bucket_size = abs(largest - smallest) / num_proc;
     
    // The numerical range for each bucket shouldnt be less than or equal to 0
    if (bucket_size < 0.0001) {
        printf("All values the same, file sorted\n");
        fclose(fstream);
        return 0;
    }

    // Now we initialize the sizes array to signify how many floats will be in each bucket
    for (int i = 0; i < num_proc; i++) {
        
        long start = smallest + (i * bucket_size);
        long end = smallest + ((i + 1) * bucket_size);

        sizes[i] = in_range(fs, start, end);
    }
    
    barrier* bb = make_barrier(num_proc);
    
    sample_sort(fs, num_proc, sizes, bucket_size, fstream, bb);
    
    free_barrier(bb);
    fclose(fstream);

    return 0;
}
