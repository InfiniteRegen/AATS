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
#include <time.h>

void *CheckTime(void *param)
{
	threadCount_t *input = param;

	while (TRUE) {
		(input->countNum)--;
		sleep(1);

		while ((input->countNum == -1))
			sleep(1);
	}

	return;
}

unsigned short NUMBER_MAP[10][5] = {
	{0xfe00, 0xfd7f, 0xfb41, 0xf77f, 0xef00},//0
	{0xfe00, 0xfd42, 0xfb7f, 0xf740, 0xef00},//1
	{0xfe00, 0xfd79, 0xfb49, 0xf74f, 0xef00},//2
	{0xfe00, 0xfd49, 0xfb49, 0xf77f, 0xef00},//3
	{0xfe00, 0xfd0f, 0xfb08, 0xf77f, 0xef00},//4
	{0xfe00, 0xfd4f, 0xfb49, 0xf779, 0xef00},//5

	{0xfe00, 0xfd7f, 0xfb49, 0xf779, 0xef00},//6
	{0xfe00, 0xfd07, 0xfb01, 0xf77f, 0xef00},//7
	{0xfe00, 0xfd7f, 0xfb49, 0xf77f, 0xef00},//8
	{0xfe00, 0xfd4f, 0xfb49, 0xf77f, 0xef00}//9
};

/* Two-dimension Array that defines Heart shape */
unsigned short HEART_MAP[2][5] = {
	{0xfe00, 0xfd0f, 0xfb1f, 0xf73e, 0xef7c}, // Left_Heart
	{0xfe7c, 0xfd3e, 0xfb1f, 0xf70f, 0xef00}, // Light_Heart
};

void *threadCount(void* param)
{
	int fd_dot=0, fd_dip=0;
	int index = 0;
	int Left = 0, Right = 0; // Left == LCD_1, Right == LCD_2

	unsigned short wData[2];
	threadCount_t *input = param;

	pthread_detach(pthread_self());

	COUNTDOWNLOCK = 0;

	fd_dot = open("/dev/dotsw", O_RDWR); // open the DOT-Matrix module.
	if (fd_dot < 0) {
		perror("DOT Driver open Error.\n");
		return;
	}

	DisplayColor(RGB_RED);
	while (TRUE) {

		if (input->distance <= 8) {
			input->countNum = 0;
		}

		Left = ((input->countNum) / 10);
		Right = ((input->countNum) % 10);

		wData[0] = NUMBER_MAP[Left][index];
		wData[1] = NUMBER_MAP[Right][index];

		write(fd_dot, (unsigned char*)wData, 4);

		++index;
		if (index >= 5)
			index = 0;

		if ((input->countNum) == -1) {
			FIREFLAG = TRUE;
			input->countFlag = FALSE;
			memset(wData, 0x00, sizeof(wData));
			while (!input->countFlag)
				sleep(1);
			DisplayColor(RGB_BLUE);
		}
	}
	close(fd_dot);

	return;
}
