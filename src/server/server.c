#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/log.h"
#include "./connection_pool.h"

#define THREAD_POOL_SIZE 8

void* SocketThreadMain(void *args)
{

}

int main(int argc, char *argv[])
{
    ConnectionPool *pool = CreateConnectionPool(THREAD_POOL_SIZE, &SocketThreadMain);

    if (pool == NULL)
    {
        error("Could not initialise connection pool");
        return 1;
    }
    
    int serverDescriptor;

    if ((serverDescriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        error("Socket could not start");
        FreeConnectionPool(pool);
        shutdown(serverDescriptor, SHUT_RDWR);
        return 1;
    }

    int port = 8080;

    if (argc > 1)
    {
        char *end;
        port = strtol(argv[1], &end, 10);
    }

    struct sockaddr_in serverAddress;
    int __reuseOpt = 1;

    if (setsockopt(serverDescriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &__reuseOpt, sizeof(__reuseOpt)) < 0)
    {
        error("Setting socket options REUSEADDR & REUSEPORT failed");
        FreeConnectionPool(pool);
        shutdown(serverDescriptor, SHUT_RDWR);
        return 1;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    if (bind(serverDescriptor, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0)
    {
        errorf("Socket could not be bound to port %d", port);
        FreeConnectionPool(pool);
        shutdown(serverDescriptor, SHUT_RDWR);
        return 1;
    }

    while (1)
    {
        logf("Server listening on port %d", port);
        if (listen(serverDescriptor, THREAD_POOL_SIZE / 2) < 0)
        {
            error("Socket failed to listen");
            FreeConnectionPool(pool);
            shutdown(serverDescriptor, SHUT_RDWR);
            return 1;
        }

        struct sockaddr_in incoming;
        socklen_t incomingLength = sizeof(incoming);
        int incomingDescriptor;

        if ((incomingDescriptor = accept(serverDescriptor, (struct sockaddr*) &incoming, &incomingLength)) < 0)
        {
            error("Socket failed to accept connection");
            FreeConnectionPool(pool);
            shutdown(serverDescriptor, SHUT_RDWR);
            return 1;
        }

        logf("Connected to client with address %s, spawning thread", inet_ntoa(incoming.sin_addr));
    }

    FreeConnectionPool(pool);
    shutdown(serverDescriptor, SHUT_RDWR);
    return 0;
}