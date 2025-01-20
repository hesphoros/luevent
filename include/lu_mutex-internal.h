#ifndef LU_MUTEX_INTERNAL_H
#define LU_MUTEX_INTERNAL_H

#include <pthread.h>
#include <mutex>
#include <semaphore.h>
#include "lu_log-internal.h"

typedef struct lu_lock_s{
private:
  pthread_mutex_t mtx;
public:
  lu_lock_s() {
    pthread_mutex_init(&mtx, NULL);
  }

}lu_lock_t;

#endif /* LU_MUTEX_INTERNAL_H */