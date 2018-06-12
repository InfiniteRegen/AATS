#pragma once

#include "StructDef.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define TRUE  1
#define FALSE 0

#define IMAGE_EXTENSION		".bmp"
#define WEBPHP_PATH			"./captured/"
#define STORAGE_PATH		"/var/www/captured/"

void ReceiveImage(int clntSocket);
