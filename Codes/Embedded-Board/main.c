#include "main.h"

#define OLED_INIT_PATH			"./OLED/init.img"
#define OLED_SUPRESSION_PATH	"./OLED/sup.img"
#define OLED_THREAT_PATH		"./OLED/threat.img"

#define MAX_CURRENT_SOCK		20

int main(int argc, char** argv)
{
	int pthreadReturn;

	pthread_t backgroundThread;

	pthread_t ArduConThread;
	ArduCon_t *sensorData;

	pthread_t CameraThread;
	CameraData_t *camData;

	pthread_t socketThread[MAX_CURRENT_SOCK];
	eventInfo_t *sockSend;

	pthread_t countThread;
	dotMatrixHandler_t *countParam;

	pthread_t secInfoThread;

	pthread_t touchThread;
	touch_t  *touchParam;

	pthread_t OLEDThread;

	pthread_t AndroidThread;

	bus_t  *busParam;
	pthread_t busThread;

	time_t cur_time;
	struct tm *cur_tm;

	int sockIndex = 0;
	int iFileIndex;
	char fileName[MAX_FILE_NAME];
	KILLSIG = FALSE; // received from android.

	COUNTDOWNLOCK = TRUE;
	FIREFLAG = FALSE;
	MAXSOCKTHREAD = 5;

	touchParam = (touch_t *)malloc(sizeof(touch_t));
	pthreadReturn = pthread_create(&touchThread, NULL, TabletTouchHandler, touchParam);

	if (pthreadReturn < 0) {
		printf("Failed to create Touch Thread!!\n");
		exit(-1);
	}

	/* INIT OLED DRIVE*/
	OLEDFD = open(OLED_DEV_NAME, O_RDWR);
	Init_OLED();
	if (OLEDFD < 0) {
		printf("[#OLED] Cannot open Driver \n");
		exit(-1);
	}

	/* INIT Background Image */
	ShowBackgroundImage(touchParam);
	DisplayColorLED(RGB_GREEN); // display GREEN

	/* Arduino <-[data]-> Embedded Board */
	sensorData = (ArduCon_t *)malloc(sizeof(ArduCon_t));
	pthreadReturn = pthread_create(&ArduConThread, NULL, ArduinoHandler, sensorData); // Arduino Connection Thread.
	if (pthreadReturn < 0) {
		printf("Failed to create ArduinoHandler Thread!!\n");
		exit(-1);
	}

	/* Real-time security camera */
	camData = (CameraData_t *)malloc(sizeof(CameraData_t));
	pthreadReturn = pthread_create(&CameraThread, NULL, CameraHandler, camData);
	if (pthreadReturn < 0) {
		printf("Failed to create Camera Thread!!\n");
		exit(-1);
	}

	/* Init counter & Set it 11 seconds */
	countParam = (dotMatrixHandler_t *)malloc(sizeof(dotMatrixHandler_t));
	countParam->countNum = 11; // sec

	pthreadReturn = pthread_create(&AndroidThread, NULL, AndroidHandler, NULL);
	if (pthreadReturn < 0) {
		printf("Failed to create Touch Thread!!\n");
		exit(-1);
	}


	/* Init BusLED */
	busParam = (bus_t *)malloc(sizeof(bus_t));
	pthreadReturn = pthread_create(&busThread, NULL, DisplayBusLED, busParam);
	if (pthreadReturn < 0) {
		printf("Failed to create busLED !!\n");
		exit(-1);
	}

	while (TRUE) {

		if (KILLSIG == 1) { /* POWER OFF SIG from ANDROID */
			DisplayOLED(OLED_INIT_PATH);
			DisplayColorLED(RGB_INIT);
			usleep(10);
			break;
		}

		/* OLED HANDLING */
		if (touchParam->supBut == TRUE) {
			DisplayOLED(OLED_SUPRESSION_PATH);
			touchParam->supBut = 0;
		}
		else if (touchParam->thBut == TRUE) {
			DisplayOLED(OLED_THREAT_PATH);
			touchParam->thBut = 0;
		}

		DisplayColorLED(RGB_GREEN); /* display GREEN */

		if (sensorData->avail == TRUE ) {

			sensorData->avail = FALSE;
			DisplayTLCD(1);
			printf("Sensor Num: %d <==> Distance: %d\n", sensorData->sensorNum, sensorData->distance);
			busParam->distance = sensorData->sensorNum;
			countParam->distance = sensorData->distance;
			/* Define a camera capture event */
			printf("saving a file..... \n");
			camData->saveFlag = TRUE;

			time(&cur_time);
			cur_tm = localtime(&cur_time);
			sockSend = (eventInfo_t *)malloc(sizeof(eventInfo_t));
			sprintf(sockSend->fileName, "%s%d%s", "capture", ++iFileIndex, ".bmp");

			sockSend->cur_time = cur_tm;
			sockSend->sensorNum = sensorData->sensorNum;
			sockSend->distance = sensorData->distance;

			sockIndex = sockIndex % 20;
			pthreadReturn = pthread_create(&socketThread[sockIndex++], NULL, SendImage, sockSend);
			if (pthreadReturn < 0) {
				printf("Failed to Create SOCKET Thread!!\n");
				exit(-1);
			}

			if (COUNTDOWNLOCK == 1 && sensorData->distance <= 17 ) {

				countParam->countNum = 11;
				countParam->countFlag = 1;
				pthreadReturn = pthread_create(&countThread, NULL, DotMatrixHandler, countParam);

				if (pthreadReturn < 0) {
					printf("Failed to create thread: CountThread()\n");
					exit(-1);
				}

				pthreadReturn = pthread_create(&secInfoThread, NULL, DoCountingDown, countParam);
				if (pthreadReturn < 0) {
					printf("Failed to create thread: DoCountingDown()\n");
					exit(-1);
				}

			}
			else if (countParam->countFlag == 0) { // only when counting is already over.
					countParam->countNum = 11;
					countParam->countFlag = 1;
			}

		} //End of sensorData->available
	} // End of infinite while loop

	close(OLEDFD);

	return 0;
}
