#include "../main.h"

#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>
#include <linux/input.h>
#include <linux/fb.h>
#include <linux/fb.h>   // for fb_var_screeninfo, FBIOGET_VSCREENINFO

#define	 FBDEV_FILE 		"/dev/fb0"
#define  INPUT_DEVICE_LIST	"/proc/bus/input/devices"
#define  EVENT_STR			"/dev/input/event"
#define  MAX_BUFF			200

int		screen_width;
int		screen_height;
int		bits_per_pixel;
int		line_length;

#define MAX_TOUCH_X	0x740
#define MAX_TOUCH_Y	0x540

void readFirstCoordinate(int fd, int* cx, int* cy)
{
	struct input_event event;
	int readSize;

	while (TRUE) {
		readSize = read(fd, &event, sizeof(event));

		if (readSize == sizeof(event)) {
			if (event.type == EV_ABS) {
				if (event.code == ABS_MT_POSITION_X) {
					*cx = event.value*screen_width/MAX_TOUCH_X;
				}
				else if (event.code == ABS_MT_POSITION_Y) {
					*cy = event.value*screen_height/MAX_TOUCH_Y;
				}
			}
			else if ((event.type == EV_SYN) && (event.code == SYN_REPORT)) {
				break;
			}
		}
	}

	return;
}

#define CUSOR_THICK	5
void drawCoordinate(unsigned char *fb_mem, int cx , int cy, int preX, int preY)
{
	int sx, sy, ex, ey;

	sx = preX - CUSOR_THICK;
	sy = preY - CUSOR_THICK;
	ex = preX + CUSOR_THICK;
	ey = preY + CUSOR_THICK;

	/* erase previous cross */
	if ( sx < 0 )
		sx = 0;

	if ( sy < 0 )
		sy = 0;

	if (ex >= screen_width)
		ex = screen_width - 1;

	if (ey >= screen_height)
		ey = screen_height -1;

	/* draw current cross */
	sx = cx - CUSOR_THICK;
	sy = cy - CUSOR_THICK;
	ex = cx + CUSOR_THICK;
	ey = cy + CUSOR_THICK;

	/* erase previous cross */
	if ( sx < 0 )
		sx = 0;

	if ( sy < 0 )
		sy = 0;

	if (ex >= screen_width)
		ex = screen_width - 1;

	if (ey >= screen_height)
		ey = screen_height -1;

	/* printf("[corX:%d || corY:%d]\n", cx, cy); */
}


void *threadTouch(void *param)
{
	char eventFullPathName[100];
	int	eventnum;
	int	x, y, preX = 0, preY = 0;
	int	fb_fd, fp;

	struct  fb_var_screeninfo fbvar;
	struct  fb_fix_screeninfo fbfix;
	unsigned char   *fb_mapped;
	int		mem_size;

	touch_t	*input = param;

	pthread_detach(pthread_self());

	eventnum = 2;
	sprintf(eventFullPathName, "%s%d", EVENT_STR, eventnum);

	fp = open(eventFullPathName, O_RDONLY);
	if (-1 == fp) {
		printf("%s open fail\n", eventFullPathName);
		return;
	}

    if (access(FBDEV_FILE, F_OK) ) {
        printf("%s: access error\n", FBDEV_FILE);
		close(fp);
        return;
    }

    if ((fb_fd = open(FBDEV_FILE, O_RDWR)) < 0) {
        printf("%s: open error\n", FBDEV_FILE);
		close(fp);
        return;
    }

    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &fbvar)) {
        printf("%s: ioctl error - FBIOGET_VSCREENINFO \n", FBDEV_FILE);
        goto fb_err;
    }

    if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &fbfix)) {
        printf("%s: ioctl error - FBIOGET_FSCREENINFO \n", FBDEV_FILE);
        goto fb_err;
    }

    screen_width    =   fbvar.xres;
    screen_height   =   fbvar.yres;
    bits_per_pixel  =   fbvar.bits_per_pixel;
    line_length     =   fbfix.line_length;

    mem_size    =   screen_width * (screen_height * 4);
    fb_mapped   =   (unsigned char *)mmap(0, mem_size, PROT_READ|PROT_WRITE, MAP_SHARED, fb_fd, 0);

	if (fb_mapped < 0) {
        printf("mmap error!\n");
        goto fb_err;
    }

    initScreen(fb_mapped);

	while (TRUE) {

		readFirstCoordinate(fp, &x, &y);
		if ((x >= 86 && x<= 113) && (y >=284 && y <= 550)) { /* HALT */
			input->thBut = 0;
			input->supBut = 0;
			input->haltBut = 1;
		}
		else if ((x >= 1037 && x <= 1122) && (y>=97 && y<=330)) { /* THREAT */
			//displayOLED("./OLED/threat.img");
			input->thBut = 1;
			input->supBut = 0;
			input->haltBut = 0;
		}
		else if ((x >= 1039 && x <= 1113) && (y>=490 && y<=711)) { /* Supression */
			//displayOLED("./OLED/sup.img");
			input->thBut = 0;
			input->supBut = 1;
			input->haltBut = 0;
		}
		else if ((x >= 1039 && x <= 1113) && (y>=490 && y<=711) && input->modeFlag == 1) { /* HALT while mode is excuted */
			printf("[#touchapp]: HALT\n");
			exit(-1);
		}

		drawCoordinate(fb_mapped, x, y, preX, preY);
		preX = x;
		preY = y;
	}

fb_err:
	close(fb_fd);
	close(fp);

	return;
}
