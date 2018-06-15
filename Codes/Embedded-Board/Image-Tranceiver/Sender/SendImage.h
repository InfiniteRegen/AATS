#pragma once

#include "../../main.h"
#include "StructDef.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_BUF 1024

#define REMOTE_IP	 "192.168.10.65"
#define REMOTE_PORT  36000

#define STORAGE_PATH "./captured/"

void *SendImage(void *param);
void Formatting(send_t *sendBuf, eventInfo_t *data);
