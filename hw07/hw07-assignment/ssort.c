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

void sort_worker(floats* fs, long* sizes, long bucket_size, int P, void* address) 
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
            address = address + (sizeof(float) * sizes[j]);
        }
        
        memcpy(address, samps->data, samps->size);
    }
}

void sample_sort(floats* fs, int num_proc, long* sizes, long bucket, void* addr, barrier* bb) {
   
    // TO-DONE: spawn P processes, each running sort_worker

    int kids[num_proc];
    
    for (int i = 0; i < num_proc; i++) {
           
        if ((kids[i] = fork())) {
            // What should the "parents" be doing?
        }
        else {
            sort_worker(fs, sizes, bucket, i, addr);
            break;
        }
    }
        
    for (int ii = 0; ii < num_proc; ++ii) {
        int rv = waitpid(kids[ii], 0, 0);
        check_rv(rv);
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
    
    void* add = NULL;
    size_t length = fsize * sizeof(float);
    //off_t offset = sizeof(long);
    off_t offset = sysconf(_SC_PAGE_SIZE);
    void* address = mmap(add, length, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);
    printf("Made it passed mmapping\n");
    
    floats* fs = floats_make();
    printf("Address: %p\n", address);
   
    for (int i = 0; i < fsize; i++) {
        //printf("%f\n", x[i]);
        //floats_push(fs, x[i]);

        float *x = address + (i * sizeof(float));
        printf("i is: %i\n", i);
        printf("x is: %f\n", *x);
        floats_push(fs, *x);
        
        printf("Floats size: %ld\n", fs->size);

        if (i >= 302) {
            //break;
        }
    }
    
    printf("Passed pushing to a floats, maybe successful?\n");

    // Make an array of longs to tell how many floats are in each bucket
    long sizes[num_proc];     
    
    float smallest = floats_smallest(fs);
    float largest = floats_largest(fs);

    long bucket_size = abs(largest - smallest) / num_proc;
     
    // The numerical range for each bucket shouldnt be less than or equal to 0
    assert(bucket_size > 0);

    // Now we initialize the sizes array to signify how many floats will be in each bucket
    for (int i = 0; i < num_proc; i++) {
        
        long start = smallest + (i * bucket_size);
        long end = smallest + ((i + 1) * bucket_size);

        sizes[i] = in_range(fs, start, end);
    }
    
    barrier* bb = make_barrier(num_proc);
    
    sample_sort(fs, num_proc, sizes, bucket_size, address, bb);
    
    free_barrier(bb);
    
    munmap(address, fsize);

    return 0;
}
