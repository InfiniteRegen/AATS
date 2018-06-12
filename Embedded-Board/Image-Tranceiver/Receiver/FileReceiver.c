#include "main.h"
#include "FileReceiver.h"
#include "SQL-Handler/SQLHandler.h"

void ReceiveImage(int clntSocket)
{
    char imageBuffer[RCVBUFSIZE], infoBuffer[RCVBUFSIZE];
    int recvMsgSize;
    FILE* fp;
    int iter;
    char fileName[100];
    char temp[20];

    char dbFileName[255];
    int bufSize;
    int recvSizeT;

    long int totalRecv = 0;
    recv_t *recvBuf;
    int fields;

	InitSQL(); /* Initalizae MYSQL & Try to establish session */
    if ((recvMsgSize = recv(clntSocket, infoBuffer, RCVBUFSIZE, 0)) < 0)
        DieWithError("recv() failed");

    recvBuf = (recv_t *)infoBuffer;

    /*  		Debugging message
	printf("Sensor: %s <=> Distance: %s <=> {%s,%s,%s} <=> {%s:%s:%s} [%d] \n",
		    recvBuf->sensorNum, recvBuf->distance,
		    recvBuf->year, recvBuf->mon, recvBuf->day,
		    recvBuf->hour, recvBuf->min, recvBuf->sec,
		    recvBuf->fileSize);
	*/

    sprintf(fileName, "%s%s%s%s_%s_%s_%s%s", STORAGE_PATH,
			recvBuf->year, recvBuf->mon, recvBuf->day,
		    recvBuf->hour, recvBuf->min, recvBuf->sec,
			IMAGE_EXTENSION);

    sprintf(dbFileName, "%s%s%s%s_%s_%s_%s%s", WEBPHP_PATH,
			recvBuf->year, recvBuf->mon, recvBuf->day,
		    recvBuf->hour, recvBuf->min, recvBuf->sec,
			IMAGE_EXTENSION);

    fp = fopen(fileName, "wb");

    if (fp == NULL) {
		printf("file open error\n");
		exit(-1);
    }

    bufSize = sizeof(imageBuffer);
    recvSizeT = 0;

    while (TRUE) {

		memset(imageBuffer, 0x00, bufSize);
	    if ((recvMsgSize = recv(clntSocket, imageBuffer, bufSize, 0)) < 0)
            	DieWithError("recv() failed");

	    fwrite(imageBuffer, 1, recvMsgSize, fp);
	    if (recvMsgSize <= 0)
		    break;

	    recvSizeT += recvMsgSize;
	    totalRecv += recvMsgSize;
    }

    fwrite(imageBuffer, 1, recvMsgSize, fp);
    fclose(fp);
    close(clntSocket);

	SaveRecvData();

    printf("[#Image] <%s>: RECEIVING DONE\n", fileName);

    return;
}
