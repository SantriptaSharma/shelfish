#ifndef CONNECTION_POOL_H_GUARD
#define CONNECTION_POOL_H_GUARD
#include <pthread.h>

typedef void* (*ThreadedRoutine) (void *);

typedef struct __ConnectionPoolCanonical {
    int size;
    int next;
    int *socketDescriptors;
    pthread_mutex_t mutex;
    pthread_t *threads;
    ThreadedRoutine routine;
} ConnectionPool;

ConnectionPool* CreateConnectionPool(int size, ThreadedRoutine routine);
void FreeConnectionPool(ConnectionPool *pool);
#endif