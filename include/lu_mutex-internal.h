#ifndef LU_MUTEX_INTERNAL_H
#define LU_MUTEX_INTERNAL_H

#include <pthread.h>
#include <semaphore.h>
#include "lu_log-internal.h"

typedef struct lu_lock_s{

  pthread_mutex_t mtx;

}lu_lock_t;


int lu_lock_s(pthread_mutex_t *mtx) {
    pthread_mutex_init(mtx, NULL);
    return 0;
}

void destroy_lu_lock_s(lu_lock_t *lock) {
    pthread_mutex_destroy(&lock->mtx);
}

#endif /* LU_MUTEX_INTERNAL_H */