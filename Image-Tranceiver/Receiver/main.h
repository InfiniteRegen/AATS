#pragma once

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TRUE  1
#define FALSE 0

#define RCVBUFSIZE 1024

void DieWithError(char *errorMessage);
int CreateTCPServerSocket(unsigned short port);
int AcceptTCPConnection(int servSock);
void ReceiveImage(int clntSocket);
