// Author: Nat Tuck
// CS3650 starter code

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>

#include "barrier.h"

barrier*
make_barrier(int nn)
{
    barrier* bb = malloc(sizeof(barrier));
    assert(bb != 0);

    bb->seen  = 0;
    bb->count = nn;

    pthread_mutex_init(&bb->wall, 0);
    pthread_mutex_init(&bb->seen_lock, 0);
    pthread_cond_init(&bb->condvar, 0);

    return bb;
}

void
barrier_wait(barrier* bb)
{
    pthread_mutex_lock(&bb->seen_lock);
    bb->seen++;
    pthread_mutex_unlock(&bb->seen_lock);
   

    pthread_mutex_lock(&bb->wall);
    while (bb->seen < bb->count) {
        pthread_cond_wait(&bb->condvar, &bb->wall);
    }
    if (bb->seen >= bb->count) {
        pthread_cond_broadcast(&bb->condvar);
    }
    pthread_mutex_unlock(&bb->wall);
}

void
free_barrier(barrier* bb)
{
    if (bb) {
        pthread_mutex_destroy(&bb->wall);
        pthread_mutex_destroy(&bb->seen_lock);
        pthread_cond_destroy(&bb->condvar);
        free(bb);
    }
}

