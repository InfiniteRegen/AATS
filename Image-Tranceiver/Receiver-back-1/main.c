#include "main.h"

#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

void *ThreadMain(void *arg);

struct ThreadArgs {
    int clntSock;
};

int main(int argc, char** argv)
{
    int servSock;
    int clntSock;
    unsigned short echoServPort;
    pthread_t threadID;
    struct ThreadArgs *threadArgs;

    if (argc != 2) {
        fprintf(stderr,"Usage:  %s <SERVER PORT>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);

    servSock = CreateTCPServerSocket(echoServPort);

    while (TRUE) {
        clntSock = AcceptTCPConnection(servSock);

        if ((threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs))) == NULL)
           DieWithError("malloc() failed");
        threadArgs->clntSock = clntSock;

        if (pthread_create(&threadID, NULL, ThreadMain, (void *) threadArgs) != 0)
            DieWithError("pthread_create() failed");

        printf("With thread %ld\n", (long int) threadID);
    }

    return 0;
}

void *ThreadMain(void *threadArgs)
{
    int clntSock;

    pthread_detach(pthread_self());

    clntSock = ((struct ThreadArgs *) threadArgs) -> clntSock;
    free(threadArgs);

    HandleTCPClient(clntSock);

    return (NULL);
}
