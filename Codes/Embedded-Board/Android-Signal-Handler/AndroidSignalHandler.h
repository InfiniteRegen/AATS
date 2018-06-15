#pragma once

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

#define SERVER_PORT 37500

void ReceiveAndroidSignal(int clntSocket);
