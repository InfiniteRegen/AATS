#include "defStruct.h" // user defined structure.

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
/* ARDUINO CONNECTION */
#define BAUDRATE        B115200
#define MODEMDEVICE     "/dev/ttyACM0"
#define OLED_DEV_NAME   "/dev/cnoled"
#define _POSIX_SOURCE   1 /* POSIX compliant source */

#define RGB_BLUE  0
#define RGB_GREEN 1
#define RGB_RED   2
#define RGB_INIT  5

void *ArduinoConnection(void *param);
void *camera_main(void *param);
void *SendImage(void *param);
void *threadCount(void *param);
void *CheckTime(void *param);
void *threadTouch(void *param);
//void *showBackground(void *param);
void showBackground(touch_t* input);
//void *displayOLED(void *param);
void displayOLED(char *fileName);
void DisplayColor(short mode);

void *AndroidSignal(void *param);
void *displayBusLED(void *param);
void displayTLCD(int mode);

int LOOP_SET_1; // Connection Loop Escape Condtion.

int KILLSIG;
int COUNTDOWNLOCK;
int OLEDFD;

int CAPTURELOCK;

int MAXSOCKTHREAD;

int FIREFLAG; // ARDUINO <==> EMBEDDED BOARD

pthread_mutex_t capture_mutex;
