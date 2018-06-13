#include "Sender.h"

void *SendImage(void *param)
{
	FILE *fp;
	int sock;
	struct sockaddr_in echoServAddr;
	unsigned short echoServPort;
	int totalCount = 0;
	char servIP[35];
	char fileBuffer[MAX_BUF];
	char sensorInfo[MAX_BUF];

	struct tm cur_time;
	unsigned short sensorNum, distance;
	char fileName[MAX_FILE_NAME], pathToRead[MAX_FILE_NAME];

	long int file_len = 0;
	int readByte = 0;

	send_t sendBuf;

	pthread_detach(pthread_self());

	eventInfo_t *data = param;

	/* Formatting */
	sprintf(sendBuf.sensorNum, "%d", data->sensorNum);
	sprintf(sendBuf.distance, "%05d", data->distance);
	sprintf(sendBuf.year, "%04d", data->cur_time->tm_year + 1900);
	sprintf(sendBuf.mon, "%02d", data->cur_time->tm_mon + 1);
	sprintf(sendBuf.day, "%02d", data->cur_time->tm_mday);
	sprintf(sendBuf.hour, "%02d", data->cur_time->tm_hour);
	sprintf(sendBuf.min, "%02d", data->cur_time->tm_min);
	sprintf(sendBuf.sec, "%02d", data->cur_time->tm_sec);
	strcpy(fileName, data->fileName);

	free(param);

	memset(pathToRead, 0x00, sizeof(pathToRead));
	sprintf(pathToRead, "%s%s", STORAGE_PATH, fileName);

	while (TRUE) {
		fp = fopen(pathToRead, "rb");
		if (fp == NULL) {
			sleep(1);
			continue;
		}
		else
			break;
	}

	strcpy(servIP, REMOTE_IP);
	echoServPort = REMOTE_PORT;

	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket(): ERROR\n");
		return;
	}

	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family      = AF_INET;
	echoServAddr.sin_addr.s_addr = inet_addr(servIP);
	echoServAddr.sin_port        = htons(echoServPort);

	if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0) {
		perror("connect(): ERROR\n");
		return;
	}

	while (TRUE) {
		fseek(fp, 0L, SEEK_END);
		file_len = ftell(fp);
		fseek(fp, 0L, SEEK_SET);

		if (file_len >= 921654)
			break;

		sleep(1);
	}

	sendBuf.fileSize = file_len;

	printf("<%s>: [TRANSFERRING]...\n", fileName);
	if (send(sock, (char *)&sendBuf, sizeof(sendBuf), 0) < 0) {
		perror("[ERROR]__>[FileSize]\n");
		return;
	}

	totalCount = 0;
	while (TRUE) {
		memset(fileBuffer, 0x00, sizeof(fileBuffer));
		readByte = fread(fileBuffer, 1, MAX_BUF, fp);

		if (readByte <= 0)
			break;

		if (send(sock, fileBuffer, readByte, 0) < 0) {
			perror("[ERROR]__>[Contents]\n");
			return;
		}
	}

	printf("<%s>----> [TRANSFER ACCOMPLISHED]!!\n", fileName);
	close(sock);

	return;
}
