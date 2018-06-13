#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <mysql.h>

#include "../FileReceiver.h"

#define CHOP(x) x[strlen(x)-1] = ' '

#define MYSQL_ID 			"root"
#define MYSQL_PASSWD 		"12345"
#define MYSQL_REMOTE_PORT 	3306
#define MYSQL_TABLE_NAME	"captureTab"
#define MYSQL_DB_NAME		"sensor"

void InitSQL();
void SaveRecvData(recv_t *recvBuf);
