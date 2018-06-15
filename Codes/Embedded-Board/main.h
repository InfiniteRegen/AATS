#include "defStruct.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#define FALSE 0
#define TRUE  1

#define BAUDRATE        B115200
#define MODEMDEVICE     "/dev/ttyACM0"
#define OLED_DEV_NAME   "/dev/cnoled"
#define _POSIX_SOURCE   1

#define RGB_BLUE  0
#define RGB_GREEN 1
#define RGB_RED   2
#define RGB_INIT  5

/* Network Modules */
void *ArduinoHandler(void *param);
void *AndroidHandler(void *param);
void *SendImage(void *param);

/* Embedded Modules */
void *CameraHandler(void *param);
void *DotMatrixHandler(void *param);
void *DoCountingDown(void *param);
void *TabletTouchHandler(void *param);
void ShowBackgroundImage(touch_t* input);

/* LED & LCD MOdules */
void DisplayOLED(char *fileName);
void DisplayColorLED(short mode);
void *DisplayBusLED(void *param);
void DisplayTLCD(int mode);

int LOOP_SET_1;

int KILLSIG;
int COUNTDOWNLOCK;
int OLEDFD;

int CAPTURELOCK;

int MAXSOCKTHREAD;

int FIREFLAG; // ARDUINO <==> EMBEDDED BOARD

pthread_mutex_t capture_mutex;
