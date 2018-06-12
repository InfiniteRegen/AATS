#include "../main.h"

#define MAX_BUF 30

volatile int STOP = FALSE;
int fd;

void* writeSerialData(void *param)
{
	char buf_2[5];

	pthread_detach(pthread_self());

	while (TRUE) {
		if (FIREFLAG == 1) {
			memset(buf_2, 10, sizeof(buf_2));
			printf("[#SERIAL] Write Complished..\n");
			write(fd, buf_2, (int)strlen(buf_2));
			FIREFLAG = 0;
		}
		sleep(1);
	}
}

void* ArduinoConnection(void *param)
{
	static char buf[MAX_BUF];
	static char buf_2[MAX_BUF];
	int c, res;
	struct termios oldtio,newtio;
	int iter=0;
	char *token;
	pthread_t serial;

	ArduCon_t *sensorData = param;

	pthread_detach(pthread_self());

	fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY );
	if (fd < 0) {
		perror(MODEMDEVICE);
		exit(-1);
	}

	tcgetattr(fd, &oldtio); /* save current port settings */

	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;

	/* set input mode (non-canonical, no echo,...) */
	newtio.c_lflag = 0;

	newtio.c_cc[VTIME] = 0;   /* inter-character timer unused */
	newtio.c_cc[VMIN] = 5;   /* blocking read until 5 chars received */

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &newtio);

	printf("####### #######################\n");
	//pthread_create(&serial, NULL, writeSerialData, NULL);

	while (STOP == FALSE) { /* loop for input */
		memset(buf, 0x00, sizeof(buf));

		res = read(fd, buf, MAX_BUF);
		buf[res] = 0;

		printf("BUF:%s\n", buf);
		token = strtok(buf, ", ");
		sensorData->sensorNum = atoi(token);
		token = strtok(NULL, ", ");
		sensorData->distance = atoi(token);
		sensorData->avail = 1;
	}

	tcsetattr(fd, TCSANOW, &oldtio);
	LOOP_SET_1 = 1;

	return;
}
