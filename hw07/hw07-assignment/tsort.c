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

typedef struct sort_job {

    floats* fs;
    floats* medians;
    long* sizes;
    int thread_num;
    barrier* bb;

} sort_job;

int float_cmp(const void * x, const void * y) {
    
    const float* a = x;
    const float* b = y;

    if (*a > *b) {
        return 1;
    }
    else if (*a < *b) {
        return -1;
    }
    else {
        return 0;
    }
}


floats* in_range(floats* fs, float start, float end) {
    
    floats* range = floats_make();
    int size = floats_size(fs);

    for (int i = 0; i < size; i++) {
        
        float test = floats_get(fs, i);

        if ((test >= start) && (test < end)) {
            floats_push(range, test);
        }
    }

    return range;
}

void qsort_floats(floats* fs) {
    // TO-DONE: call qsort to sort the array
    qsort(fs->data, fs->size, sizeof(float), float_cmp);
}

void* sort_worker(void *arg) {
    
    sort_job* job = (sort_job*)arg;

    float start = floats_get(job->medians, job->thread_num);
    float end = floats_get(job->medians, job->thread_num + 1);

    floats* range = in_range(job->fs, start, end); 
    job->sizes[job->thread_num] = range->size;
    
    qsort_floats(range);

    // Wait on barrier here

    int count = 0;
    for (int i = 0; i < job->thread_num; i++) {
        count += job->sizes[i];
    }

    memcpy(&job->fs->data[count], range->data, range-> size * sizeof(float));

    floats_free(range); 

    return 0;
}

void sample_sort(floats* fs, int num_threads, long* sizes, barrier* bb) {
    
    int num_samples = 3 * (num_threads - 1);
    
    floats* sampled = floats_make();
    
    for (int i = 0; i < num_samples; i++) {

        int loc = random() % fs->size;
        
        floats_push(sampled, floats_get(fs, loc));
    }

    qsort_floats(sampled);
    
    floats* medians = floats_make();
    floats_push(medians, -INFINITY);

    for (int i = 1; i < num_samples; i += 3) {
        floats_push(medians, floats_get(sampled, i));
    }

    floats_push(medians, INFINITY);
    floats_free(sampled);

    pthread_t threads[num_threads];

    for (int i = 0; i < num_threads; i++) {
        
        sort_job* job = malloc(sizeof(job));
        job->fs = fs;
        job->medians = medians;
        job->sizes = sizes;
        job->thread_num = i;
        job->bb = bb;

        int rv = pthread_create(&threads[i], 0, sort_worker, job); 
        check_rv(rv);
        
    }
        
    for (int i = 0; i < num_threads; ++i) {
         int rv = pthread_join(threads[i], 0);
         check_rv(rv);
    }

}

int main(int argc, char* argv[]) {
    alarm(120);

    if (argc != 4) {
        printf("Usage:\n");
        printf("\t%s T data.dat output.dat\n", argv[0]);
        return 1;
    }

    const int num_threads = atoi(argv[1]);
    const char* fname = argv[2];
    const char* out_name = argv[3];
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

    int in_fd = open(fname, O_RDONLY);
    check_rv(in_fd);

    long num_floats;
    read(in_fd, &num_floats, sizeof(long));

    floats* fs = floats_make();
    void* read_addr = malloc(num_floats * sizeof(float));
    
    read(in_fd, read_addr, num_floats * sizeof(float)); // Reads in fsize bytes
    close(in_fd);
    
    fs->size = num_floats;
    fs->cap = num_floats;
    fs->data = (float*) read_addr;
    
    long sizes[num_threads];     

   // barrier* bb = make_barrier(num_threads);
      
    sample_sort(fs, 1, sizes, 0);
   // 
   // free_barrier(bb);
   //
    
    
    int out_fd = open(out_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    write(out_fd, &num_floats, sizeof(long));
    write(out_fd, fs->data, num_floats * sizeof(float)); 
    
    close(out_fd);

    return 0;
}
