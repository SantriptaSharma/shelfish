#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "../lib/log.h"

int main(int argc, char *argv[])
{
    int port = 8080;
    const char * ip = "127.0.0.1";

    if (argc > 1)
    {
        char *end;
        port = strtol(argv[1], &end, 10);
    
        ip = argc > 2 ? argv[2] : ip;
    }

    logf("Using ip address %s and port %d. Attempting to connect to server...", ip, port);

    int serverDescriptor, clientDescriptor;

    if ((serverDescriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        error("Could not create socket");
        return 1;
    }


    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &serverAddress.sin_addr) <= 0)
    {
        errorf("Invalid ip address: %s", ip);
        return 1;
    }

    if ((clientDescriptor = connect(serverDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress))) < 0)
    {
        errorf("Could not connect to server at %s:%d", ip, port);
        return 1;
    }

    logf("Connected to %s:%d", ip, port);

    read(serverDescriptor, NULL, 1);

    return 0;
}