#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <mysql.h>

#define RCVBUFSIZE 1024
#define CHOP(x) x[strlen(x)-1] = ' '

#define MYSQL_ID 			"root"
#define MYSQL_PASSWD 		"12345"
#define MYSQL_REMOTE_PORT 	3306
#define MYSQL_TABLE_NAME	"captureTab"
#define MYSQL_DB_NAME		"sensor"

#define IMAGE_EXTENSION		".bmp"
#define WEBPHP_PATH			"./captured/"
#define STORAGE_PATH		"/var/www/captured/"

static int sql_cnt;

typedef struct recvBuf {
	char sensorNum[2];
	char distance[6];
	char year[5];
	char mon[3];
	char day[3];
	char hour[3];
	char min[3];
	char sec[3];
	int fileSize;
} recv_t;

void HandleTCPClient(int clntSocket)
{
    char echoBuffer[RCVBUFSIZE], preBuffer[RCVBUFSIZE];
    int recvMsgSize;
    FILE* fp;
    int iter;
    char fileName[100];
    char temp[20];
    char query[255];
    char dbFileName[255];
    int bufSize;
    int query_stat;
    int recvSizeT;

    long int totalRecv = 0;

    recv_t *recvBuf;

    MYSQL		mysql, *connection=NULL;
    MYSQL_RES*	res;
    MYSQL_ROW	row;

    int fields;

    mysql_init(&mysql);

    if (!(connection = mysql_real_connect(&mysql, NULL, MYSQL_ID, MYSQL_PASSWD, NULL, MYSQL_REMOTE_PORT, (char *)NULL, 0))) {
    	printf("%s", mysql_error(&mysql));
		exit(-1);
    }

	sprintf(query, "USE %s", MYSQL_DB_NAME);
	if (mysql_query(&mysql, query)) {
        printf("%s \n", mysql_error(&mysql));
	    exit(-1);
	}

    if ((recvMsgSize = recv(clntSocket, preBuffer, RCVBUFSIZE, 0)) < 0)
        DieWithError("recv() failed");

    recvBuf = (recv_t *)preBuffer;

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

    bufSize = sizeof(echoBuffer);
    recvSizeT = 0;

    while (TRUE) {

		memset(echoBuffer, 0x00, bufSize);
	    if ((recvMsgSize = recv(clntSocket, echoBuffer, bufSize, 0)) < 0)
            	DieWithError("recv() failed");

	    fwrite(echoBuffer, 1, recvMsgSize, fp);
	    if (recvMsgSize <= 0)
		    break;

	    recvSizeT += recvMsgSize;
	    totalRecv += recvMsgSize;
    }

    fwrite(echoBuffer, 1, recvMsgSize, fp);
    fclose(fp);
    close(clntSocket);

    /*		Debugging message
	printf("(%d, '%s', '%s', '%s%s%s_%s:%s:%s', '%s', %09ld) \n",
		    sql_cnt,
		    recvBuf->sensorNum, recvBuf->distance,
			recvBuf->year, recvBuf->mon, recvBuf->day,
			recvBuf->hour, recvBuf->min, recvBuf->sec,
		    dbFileName,
		    totalRecv);
	*/

    sprintf(query, "insert into %s values (%d, '%s', '%s', '%s%s%s_%s:%s:%s', '%s', %09ld)",
			MYSQL_TABLE_NAME, sql_cnt++,
			recvBuf->sensorNum, recvBuf->distance,
			recvBuf->year, recvBuf->mon, recvBuf->day,
			recvBuf->hour, recvBuf->min, recvBuf->sec,
			dbFileName,
			totalRecv);

    query_stat = mysql_query(connection, query);

    if (query_stat != 0) {
		printf("query error!\n");
		exit(-1);
    }

    mysql_free_result(res);
    mysql_close(&mysql);

    printf("[#Image] <%s>: RECEIVING DONE\n", fileName);

    return;
}
