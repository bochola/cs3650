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

void qsort_floats(floats* fs) {
    // TO-DONE: call qsort to sort the array
    qsort(fs->data, fs->size, sizeof(float), float_cmp);
}

floats* sample(float* data, int num_proc) {
    // TODO: sample the input data, per the algorithm description
    return floats_make(10);
}

void sort_worker(int pnum, float* data, long size, int P, floats* samps, long* sizes, barrier* bb) 
{
    floats* xs = floats_make(10);
    // TODO: select the floats to be sorted by this worker

    printf("%d: start %.04f, count %d\n", pnum, samps->data[pnum], xs->size);


    qsort_floats(xs);

    // TODO: probably more stuff

    floats_free(xs);
}

void sample_sort(floats* fs, int P, long* sizes, barrier* bb) {
   
    pid_t kids[P];
    (void) kids; // suppress unused warning

    // TODO: spawn P processes, each running sort_worker

    for (int ii = 0; ii < P; ++ii) {
        //int rv = waitpid(kids[ii], 0, 0);
        //check_rv(rv);
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

    void* address = mmap(0, fsize, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    floats* fs = floats_make();

    for (int i = 0; i < fsize; i+= sizeof(float)) {
        float* x = address + i;
        floats_push(fs, *x);
    }
    
    // Make an array of longs to tell how many floats are in each bucket
    long sizes[num_proc];     
    
    float smallest = floats_smallest(fs);
    float largest = floats_largest(fs);

    long bucket_chunks = abs(largest - smallest) / num_proc;
     
    // The numerical range for each bucket shouldnt be less than or equal to 0
    assert(bucket_chunks > 0);

    // Now we initialize the sizes array to signify how many floats will be in each bucket
    for (int i = 0; i < num_proc; i++) {
        
        long start = smallest + (i * bucket_chunks);
        long end = smallest + ((i + 1) * bucket_chunks);

        sizes[i] = in_range(fs, start, end);
    }
    
    barrier* bb = make_barrier(num_proc);
    
    sample_sort(fs, num_proc, sizes, bb);
    
    free_barrier(bb);
    
    munmap(address, fsize);

    return 0;
}
