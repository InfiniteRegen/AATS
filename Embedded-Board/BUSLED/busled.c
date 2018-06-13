#include "../main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void *displayBusLED(void *param)
{
	int fd=0;
	short numberOfLED=0;
	unsigned short wData=0;

	pthread_detach(pthread_self());
	bus_t *input = param;

	fd = open("/dev/busled", O_RDWR); // OPEN A DEVICE MODULE as a ONLY_READ MODE.

	if (fd < 0) {
		perror("color_LED driver open error.\n");
		return;
	}

	while (TRUE) {

		numberOfLED = ((input->distance) / 2);
		if (numberOfLED >= 9)
			numberOfLED = 8;

		numberOfLED = 8 - numberOfLED;
		wData = 1;
		wData = wData << numberOfLED;

		write(fd, (unsigned char *)&wData, 2);
		sleep(1);
	}

	close(fd);

	return;
}
