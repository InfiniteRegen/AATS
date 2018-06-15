#include "SQLHandler.h"

static int sql_cnt;

MYSQL		mysql, *connection=NULL;
MYSQL_RES*	res;
MYSQL_ROW	row;

void InitSQL()
{
    char query[255];
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

    return;
}

void SaveRecvData(recv_t *recvBuf, long int totalRecv)
{
    char query[255];
    char dbFileName[255];
    int queryStatus;

    sprintf(dbFileName, "%s%s%s%s_%s_%s_%s%s", WEBPHP_PATH,
            recvBuf->year, recvBuf->mon, recvBuf->day,
            recvBuf->hour, recvBuf->min, recvBuf->sec,
            IMAGE_EXTENSION);

    sprintf(query, "insert into %s values (%d, '%s', '%s', '%s%s%s_%s:%s:%s', '%s', %09ld)",
            MYSQL_TABLE_NAME, sql_cnt++,
            recvBuf->sensorNum, recvBuf->distance,
            recvBuf->year, recvBuf->mon, recvBuf->day,
            recvBuf->hour, recvBuf->min, recvBuf->sec,
            dbFileName,
            totalRecv);

    /*		Debugging message
	printf("(%d, '%s', '%s', '%s%s%s_%s:%s:%s', '%s', %09ld) \n",
		    sql_cnt,
		    recvBuf->sensorNum, recvBuf->distance,
			recvBuf->year, recvBuf->mon, recvBuf->day,
			recvBuf->hour, recvBuf->min, recvBuf->sec,
		    dbFileName,
		    totalRecv);
	*/

    queryStatus = mysql_query(connection, query);

    if (queryStatus != 0) {
        printf("query error!\n");
        exit(-1);
    }

    mysql_free_result(res);
    mysql_close(&mysql);

    return;
}
