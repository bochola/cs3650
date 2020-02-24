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

#include "float_vec.h"
#include "barrier.h"
#include "utils.h"

void qsort_floats(floats* fs) {
    // TO-DONE: call qsort to sort the array
    int (*comparison)(float, float);
    comparison = &float_cmp;
    qsort(fs->data, fs->size, sizeof(float), comparison);
}

floats* sample(float* data, long size, int P) {
    // TODO: sample the input data, per the algorithm decription
    return floats_make(10);
}

void sort_worker(int pnum, float* data, long size, int P, floats* samps, long* sizes, barrier* bb) 
{
    floats* xs = floats_make(10);
    // TODO: select the floats to be sorted by this worker

    printf("%d: start %.04f, count %d\n", pnum, samps->data[pnum], xs->size);

    // TODO: some other stuff

    qsort_floats(xs);

    // TODO: probably more stuff

    floats_free(xs);
}

void run_sort_workers(float* data, long size, int P, floats* samps, long* sizes, barrier* bb) {
    pid_t kids[P];
    (void) kids; // suppress unused warning

    // TODO: spawn P processes, each running sort_worker

    for (int ii = 0; ii < P; ++ii) {
        //int rv = waitpid(kids[ii], 0, 0);
        //check_rv(rv);
    }
}

void sample_sort(float* data, long size, int P, long* sizes, barrier* bb) {
    floats* samps = sample(data, size, P);
    run_sort_workers(data, size, P, samps, sizes, bb);
    floats_free(samps);
}

int main(int argc, char* argv[]) {
    alarm(120);

    if (argc != 3) {
        printf("Usage:\n");
        printf("\t%s P data.dat\n", argv[0]);
        return 1;
    }

    const int P = atoi(argv[1]);
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

    void* file = malloc(1024); // TODO: load the file with mmap.
    (void) file; // suppress unused warning.

    // TODO: These should probably be from the input file.
    long count = 100;
    float* data = malloc(1024);

    long sizes_bytes = P * sizeof(long);
    long* sizes = malloc(sizes_bytes); // TODO: This should be shared

    barrier* bb = make_barrier(P);

    sample_sort(data, count, P, sizes, bb);

    free_barrier(bb);

    // TODO: munmap your mmaps

    return 0;
}

