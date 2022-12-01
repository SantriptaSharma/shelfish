#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>

#include "../lib/log.h"
#include "./connection_pool.h"

ConnectionPool* CreateConnectionPool(int size, ThreadedRoutine routine)
{
    ConnectionPool *pool = malloc(sizeof(*pool));

    pool->size = size;
    pool->current = -1;

    if (pthread_mutex_init(&(pool->mutex), NULL) < 0)
    {
        free(pool);
        return NULL;
    }

    pool->socketDescriptors = malloc(size * sizeof(*(pool->socketDescriptors)));
    pool->threads = malloc(size * sizeof(*(pool->threads)));
    pool->routineInputs = malloc(size *  sizeof(*(pool->routineInputs)));
    for (int i = 0; i < size; i++)
    {
        pool->socketDescriptors[i] = -1;
    }
    pool->routine = routine;

    return pool;
}

void AddConnection(ConnectionPool *pool, int descriptor)
{
    int next = pool->current = (pool->current + 1) % pool->size;

    if (pool->socketDescriptors[next] != -1)
    {
        int i;
        for (i = (next + 1) % pool->size; i != next && pool->socketDescriptors[i] != -1; i = (i + 1) % pool->size) {};

        pool->current = next = i;

        if (pool->socketDescriptors[i] != -1)
        {
            errorf("Connection pool is full, closing new socket %d", descriptor);
            close(descriptor);
            return;
        }
    }

    pool->socketDescriptors[next] = descriptor;

    pool->routineInputs[next].socketDescriptor = descriptor;
    pool->routineInputs[next].mutex = &(pool->mutex);
    pool->routineInputs[next].pool = pool;
    pool->routineInputs[next].poolIndex = next;

    pool->threads[next] = pthread_create(&(pool->threads[next]), NULL, pool->routine, (void*) &(pool->routineInputs[next]));
    logf("Added connection %d to pool at index %d", descriptor, next);
}

void CloseConnection(ConnectionPool *pool, int index)
{
    if (pthread_mutex_trylock(&(pool->mutex)) == 0)
    {
        error("mutex not locked");
        return;
    }

    if (index < pool->size && pool->socketDescriptors[index] != -1)
    {
        pthread_cancel(pool->threads[index]);

        close(pool->socketDescriptors[index]);

        logf("Cancelled existing thread and closed socket for %d", index);
    }

    pool->socketDescriptors[index] = -1;
}

void FreeConnectionPool(ConnectionPool *pool)
{
    pthread_mutex_lock(&(pool->mutex));
    for (int i = 0; i < pool->size; i++)
    {
        CloseConnection(pool, i);
    }
    pthread_mutex_unlock(&(pool->mutex));

    free(pool->threads);
    free(pool->socketDescriptors);
    free(pool->routineInputs);
    pthread_mutex_destroy(&(pool->mutex));
    free(pool);
}