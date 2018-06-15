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

#include "../main.h"

void DisplayColorLED(short mode)
{
	static int fd_color = 0;
	unsigned short wColor[3];
	int result = 0;

	if (!fd_color)
		fd_color = open("/dev/colorled", O_RDWR); // OPEN A DEVICE MODULE as a ONLY_READ MODE.

	if (fd_color < 0) {
		perror("color_LED driver open error.\n");
		return;
	}

	switch (mode) {

		case RGB_RED:
			wColor[0] = 0x00;
			wColor[1] = 0x50;
			wColor[2] = 0x50;
			write(fd_color, &wColor, 6);
			break;

		case RGB_GREEN:
			wColor[0] = 0x00;
			wColor[1] = 0x60;
			wColor[2] = 0x00;
			write(fd_color, &wColor, 6);
			break;

		case RGB_BLUE:
			wColor[0] = 0x00;
			wColor[1] = 0x00;
			wColor[2] = 0x60;
			write(fd_color, &wColor, 6);
			break;

		case RGB_INIT:
			wColor[0] = 0x00;
			wColor[1] = 0x00;
			wColor[2] = 0x00;
			write(fd_color, (unsigned char *)wColor, 12);
			close(fd_color);
			break;

		default: /* Do nothing */
			break;
	}

	return;
}
