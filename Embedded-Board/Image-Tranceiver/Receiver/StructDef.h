#pragma once

typedef struct recvBuf {
	char sensorNum[2];
	char distance[6];
	char year[5];
	char mon[3];
	char day[3];
	char hour[3];
	char min[3];
	char sec[3];
	int  fileSize;
} recv_t;
