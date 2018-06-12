/*
 *	This header file has a information about all user-defined structure.
 */
#include <time.h>

#define MAX_FILE_NAME 100
#define TRUE 1
#define FALSE 0

typedef struct ArduCon {
	unsigned short sensorNum;
	unsigned short distance;
	unsigned short avail;
} ArduCon_t;

typedef struct CameraData {
	char			fileName[MAX_FILE_NAME];
	unsigned char 	*frame;
	unsigned short 	saveFlag; /* 1: Save, 0: No save */
	unsigned char	*fb_mapped;
} CameraData_t;

typedef struct eventSend {
	char			fileName[100];
	struct tm*		cur_time;
	unsigned short	sensorNum;
	unsigned short	distance;
} eventInfo_t;

typedef struct countingTen {
	short countNum;
	short countFlag;
	short distance;
} threadCount_t;

typedef struct touchInfo {
	short thBut; /* Threat Button */
	short supBut; /* Supression Button */
	short haltBut; /* 0:off 1:on */
	char  fileName[100]; /* OLED */
	short modeFlag; /* Flag that distinguish threat or supression mode */
} touch_t;

typedef struct OLEDInfo {
	char *fileName;
} OLED_t;

typedef struct BUSInfo {
	short distance;
} bus_t;

typedef struct colorInfo {
	short mode;
} color_t;
