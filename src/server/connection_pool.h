#ifndef CONNECTION_POOL_H_GUARD
#define CONNECTION_POOL_H_GUARD
#include <pthread.h>

typedef void* (*ThreadedRoutine) (void *);

typedef struct __ConnectionPoolCanonical ConnectionPool;

typedef struct __RoutineInputCanonical
{
    int socketDescriptor;
    pthread_mutex_t *mutex;
    ConnectionPool *pool;
    int poolIndex;
} RoutineInput;

typedef struct __ConnectionPoolCanonical {
    int size;
    int current;
    int *socketDescriptors;
    pthread_t *threads;
    RoutineInput *routineInputs;
    pthread_mutex_t mutex;
    ThreadedRoutine routine;
} ConnectionPool;

ConnectionPool* CreateConnectionPool(int size, ThreadedRoutine routine);
void AddConnection(ConnectionPool *pool, int descriptor);
void CloseConnection(ConnectionPool *pool, int index);
void FreeConnectionPool(ConnectionPool *pool);
#endif