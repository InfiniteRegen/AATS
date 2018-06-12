#include "../main.h"

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXPENDING 5
#define RCVBUFSIZE 200

#define ANDROID_DATA 9
#define ANDROID_ON   1
#define ANDROID_OFF  0

void HandleTCPClient(int clntSocket)
{
	char echoBuffer[RCVBUFSIZE];
	int recvMsgSize;
	int iter = 0;

	if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0) {
    	printf("[#ANDROID] recv() function error..\n");
		exit(-1);
	}

	echoBuffer[recvMsgSize] = '\0';
	printf("[#ANDROID] ANDROID_DATA: %d\n", echoBuffer[ANDROID_DATA]); /* Debug Message */

	if (echoBuffer[ANDROID_DATA] == ANDROID_ON) {
		KILLSIG = 0;
		printf("[#ANDROID] AATS: [ON]\n");
	}

	if (echoBuffer[ANDROID_DATA] == ANDROID_OFF) {
		KILLSIG = 1;
		printf("[#ANDROID] AATS: [OFF]\n");
	}

	close(clntSocket);

	return;
}

void* AndroidSignal(void *param)
{
    int servSock;
    int clntSock;
    struct sockaddr_in echoServAddr;
    struct sockaddr_in echoClntAddr;
    unsigned short echoServPort;
    unsigned int clntLen;

    pthread_detach(pthread_self());

    echoServPort = 37500;

    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    	printf("[#Android] socket() function error..\n");
		exit(-1);
    }

    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    echoServAddr.sin_port = htons(echoServPort);

    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0) {
    	printf("[#Android] socket() function error..\n");
		exit(-1);
    }

    if (listen(servSock, MAXPENDING) < 0) {
	    printf("[#Android] listen() function error..\n");
	    exit(-1);
    }

    while (TRUE) {

        clntLen = sizeof(echoClntAddr);

        if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0) {
			printf("[#Android] accept() function error..\n");
			exit(-1);
		}

        printf("[#ANDROID] CLIENT-IP: [%s]\n", inet_ntoa(echoClntAddr.sin_addr));

        HandleTCPClient(clntSock);
    }
}
