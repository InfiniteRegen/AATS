#include "../main.h"

#include <sys/signal.h>
#include <sys/types.h>

volatile int STOP = FALSE;

void signal_handler_IO(int status);
int wait_flag = TRUE;

void* ArduinoConnection(void *param)
{
        int fd,c, res;
        struct termios oldtio,newtio;
        struct sigaction saio;
        char buf[255];
        char sendBuf[255];

        int iter = 0;
        ArduCon_t *sensorData = param;

        /* open the device to be non-blocking (read will return immediatly) */
        fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
        if (fd < 0) {
            perror(MODEMDEVICE);
            exit(-1);
        }

        pthread_detach(pthread_self());
        /* install the signal handler before making the device asynchronous */
        saio.sa_handler = signal_handler_IO;
        memset(&saio.sa_mask, 0x00, sizeof(saio.sa_mask));
        saio.sa_flags = 0;
        saio.sa_restorer = NULL;
        sigaction(SIGIO, &saio, NULL);

        /* allow the process to receive SIGIO */
        fcntl(fd, F_SETOWN, getpid());
        /* Make the file descriptor asynchronous (the manual page says only
           O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
        fcntl(fd, F_SETFL, FASYNC);

        tcgetattr(fd, &oldtio); /* save current port settings */
        /* set new port settings for canonical input processing */
        newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
        newtio.c_iflag = IGNPAR | ICRNL;
        newtio.c_oflag = 0;
        newtio.c_lflag = ICANON;
        newtio.c_cc[VMIN] = 1;
        newtio.c_cc[VTIME] = 0;
        tcflush(fd, TCIFLUSH);
        tcsetattr(fd, TCSANOW, &newtio);

        /* loop while waiting for input. normally we would do something
           useful here */
        while (STOP == FALSE) {
                if (FIREFLAG == 1) {
                        printf("[###] WRITING FIRING ROUTINE!!!! \n");
                        memset(sendBuf, '#', sizeof(sendBuf));
                        write(fd, sendBuf, strlen(sendBuf));
                        FIREFLAG = 0;
                }
                memset(buf, 0x00, sizeof(buf));
                memset(sendBuf, 0x00, sizeof(sendBuf));
                usleep(10000);

                /* after receiving SIGIO, wait_flag = FALSE, input is available
                           and can be read */
                if (wait_flag == FALSE) {
                        res = read(fd, buf, 255);
                        buf[res] = 0;
                        for (iter = 0; iter < res; ++iter)
                                printf("%d ", buf[iter]);
                        sensorData->avail = 1;

                        if (res == 1)
                                STOP = TRUE;  /* stop loop if only a CR was input */

                        wait_flag = TRUE; /* wait for new input */
                }
        }
        /* restore old port settings */
        tcsetattr(fd,TCSANOW,&oldtio);
}

/***************************************************************************
* signal handler. sets wait_flag to FALSE, to indicate above loop that     *
* characters have been received.                                           *
***************************************************************************/

void signal_handler_IO(int status)
{
        printf("received SIGIO signal.\n");
        wait_flag = FALSE;

        return;
}
