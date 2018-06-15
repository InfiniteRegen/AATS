#include "AndroidSignalHandler.h"

void *AndroidHandler(void *param)
{
    int servSock, androidSock;
    unsigned int androidLen;
    struct sockaddr_in serverAddr, androidAddr;

    pthread_detach(pthread_self());

    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    	printf("[#Android] socket() function error..\n");
		exit(-1);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(SERVER_PORT);

    if (bind(servSock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
    	printf("[#Android] socket() function error..\n");
		exit(-1);
    }

    if (listen(servSock, MAXPENDING) < 0) {
	    printf("[#Android] listen() function error..\n");
	    exit(-1);
    }

    while (TRUE) {

        androidLen = sizeof(androidAddr);

        if ((androidSock = accept(servSock, (struct sockaddr *) &androidAddr, &androidLen)) < 0) {
			printf("[#Android] accept() function error..\n");
			exit(-1);
		}

        printf("[#ANDROID] CLIENT-IP: [%s]\n", inet_ntoa(androidAddr.sin_addr));

        ReceiveAndroidSignal(androidSock);
    }
}

void ReceiveAndroidSignal(int androidSock)
{
	char dataBuffer[RCVBUFSIZE];
	int recvMsgSize;

	if ((recvMsgSize = recv(androidSock, dataBuffer, RCVBUFSIZE, 0)) < 0) {
    	printf("[#ANDROID] recv() function error..\n");
		exit(-1);
	}

	dataBuffer[recvMsgSize] = '\0';
	printf("[#ANDROID] ANDROID_DATA: %d\n", dataBuffer[ANDROID_DATA]); /* Debug Message */

	if (dataBuffer[ANDROID_DATA] == ANDROID_ON) {
		KILLSIG = 0;
		printf("[#ANDROID] AATS: [ON]\n");
	}

	if (dataBuffer[ANDROID_DATA] == ANDROID_OFF) {
		KILLSIG = 1;
		printf("[#ANDROID] AATS: [OFF]\n");
	}

	close(androidSock);

	return;
}
