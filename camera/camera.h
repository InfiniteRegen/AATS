#include "videodev2.h"
#include "SecBuffer.h"
#include "bitmap.h" // Additional Header for BMP Files

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     // for open/close
#include <fcntl.h>      // for O_RDWR
#include <sys/ioctl.h>  // for ioctl
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <sys/poll.h>
#include <linux/fb.h>
#include <linux/videodev2.h>
#include <time.h> // time functio // time functionn

#define BIT_VALUE_24BIT   24

#define V4L2_CID_CACHEABLE (V4L2_CID_BASE+40)

#define FBDEV_FILE        "/dev/fb0" // used in main() function
#define FILE_NAME_MAX     100

#define WIDTH   640
#define HEIGHT  480
//#define WIDTH   960
//#define HEIGHT  800

#define CHECK(return_value)                                          \
    if (return_value < 0) {                                          \
        printf("%s::%d fail. errno: %s, m_camera_id = %d\n",             \
             __func__, __LINE__, strerror(errno), 0);      \
        return -1;                                                   \
    }

#define CHECK_PTR(return_value)                                      \
    if (return_value < 0) {                                          \
        printf("%s::%d fail, errno: %s, m_camera_id = %d\n",             \
             __func__,__LINE__, strerror(errno), 0);       \
        return NULL;                                                 \
    }

#define CAMERA_PREVIEW_WIDTH       640
#define CAMERA_PREVIEW_HEIGHT      480
//#define CAMERA_PREVIEW_WIDTH       960
//#define CAMERA_PREVIEW_HEIGHT      800
#define MAX_BUFFERS     8

#define MAX_PLANES      (1)
#define V4L2_BUF_TYPE   V4L2_BUF_TYPE_VIDEO_CAPTURE

#define PREVIEW_NUM_PLANE (1)


#define V4L2_MEMORY_TYPE V4L2_MEMORY_MMAP

#define CAMERA_DEV_NAME   "/dev/video0"
#define PREVIEW_MODE 1

enum scenario_mode {
    IS_MODE_PREVIEW_STILL,
    IS_MODE_PREVIEW_VIDEO,
    IS_MODE_CAPTURE_STILL,
    IS_MODE_CAPTURE_VIDEO,
    IS_MODE_MAX
};


/*======================{ FUNCTION DEF }=======================*/
void write_bmp(char *filename);
static int close_buffers(struct SecBuffer *buffers, int num_of_buf);
static int fimc_poll(struct pollfd *events);
static int fimc_v4l2_querycap(int fp);
static const __u8* fimc_v4l2_enuminput(int fp, int index);
static int fimc_v4l2_s_input(int fp, int index);
static int fimc_v4l2_s_fmt(int fp, int width, int height, unsigned int fmt, enum v4l2_field field, unsigned int num_plane);
int fimc_v4l2_s_fmt_is(int fp, int width, int height, unsigned int fmt, enum v4l2_field field);
static int fimc_v4l2_enum_fmt(int fp, unsigned int fmt);
static int fimc_v4l2_reqbufs(int fp, enum v4l2_buf_type type, int nr_bufs);
static int fimc_v4l2_querybuf(int fp, struct SecBuffer *buffers, enum v4l2_buf_type type, int nr_frames, int num_plane);
static int fimc_v4l2_streamon(int fp);
static int fimc_v4l2_streamoff(int fp);
static int fimc_v4l2_qbuf(int fp, int index );
static int fimc_v4l2_dqbuf(int fp, int num_plane);
static int fimc_v4l2_s_ctrl(int fp, unsigned int id, unsigned int value);
int CreateCamera(int index);
void DestroyCamera();
int startPreview(void);
int stopPreview(void);
void initScreen(unsigned char *fb_mem );
static void DrawFromRGB565(unsigned char *displayFrame, unsigned char *videoFrame,int videoWidth, int videoHeight, int dFrameWidth,int dFrameHeight, unsigned short saveFlag);
/*========================={ GLOBAL VARIABLE }================================*/
static int m_preview_v4lformat = V4L2_PIX_FMT_RGB565;
static int  m_cam_fd;
static struct SecBuffer m_buffers_preview[MAX_BUFFERS];
static struct pollfd   m_events_c;
static int   screen_width;
static int   screen_height;
static int   bits_per_pixel;
static int   line_length;

FILE    *outfile[5];
int	outfile_index=-1;
