#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "./connection_pool.h"

ConnectionPool* CreateConnectionPool(int size, ThreadedRoutine routine)
{
    ConnectionPool *pool = malloc(sizeof(*pool));

    pool->size = size;
    pool->next = 0;

    if (pthread_mutex_init(&(pool->mutex), NULL) < 0)
    {
        free(pool);
        return NULL;
    }

    pool->socketDescriptors = calloc(size, sizeof(*(pool->socketDescriptors)));
    pool->threads = calloc(size, sizeof(*(pool->threads)));
    for (int i = 0; i < size; i++)
    {
        pool->threads[i] = -1;
        pool->socketDescriptors[i] = -1;
    }
    pool->routine = routine;
}

void FreeConnectionPool(ConnectionPool *pool)
{
    pthread_mutex_lock(&(pool->mutex));
    for (int i = 0; i < pool->size; i++)
    {
        if (pool->threads[i] != -1)
        {
            pthread_cancel(pool->threads[i]);
            pthread_join(pool->threads[i], NULL);
        
            close(pool->socketDescriptors[i]);
        }
    }
    pthread_mutex_unlock(&(pool)->mutex);
    free(pool->threads);
    free(pool->socketDescriptors);
    pthread_mutex_destroy(&(pool->mutex));
    free(pool);
}