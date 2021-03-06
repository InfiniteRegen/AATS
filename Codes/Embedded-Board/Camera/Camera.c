#include "../main.h"
#include "Camera.h"

char fileName[FILE_NAME_MAX];
int fileCnt = 0;
char fileCnt_s[5];

CameraData_t *camData;

void *CameraHandler(void *param)
{
	camData = param;

	int fb_fd;
	int ret;
	int index;

	time_t init_time, cur_time;
	struct tm *init_tm, *cur_tm;

	struct  fb_var_screeninfo fbvar;
	struct  fb_fix_screeninfo fbfix;
	unsigned char   *fb_mapped;
	int mem_size;

	pthread_detach(pthread_self());

	if (access(FBDEV_FILE, F_OK)) {
		printf("%s: access error\n", FBDEV_FILE);
		return;
	}

	if ((fb_fd = open(FBDEV_FILE, O_RDWR)) < 0) {
		printf("%s: open error\n", FBDEV_FILE);
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

	mem_size   =  screen_width * screen_height * 4;
	fb_mapped  =  (unsigned char *)mmap(0, mem_size, PROT_READ|PROT_WRITE, MAP_SHARED, fb_fd, 0);

	camData->frame = fb_mapped;
	if (fb_mapped < 0) {
		printf("mmap error!\n");
		goto fb_err;
	}

	initScreen(fb_mapped);

	CreateCamera(0);
	startPreview();

	time(&init_time);
	init_tm = localtime(&init_time);

	while (TRUE) {
		ret = fimc_poll(&m_events_c);
		index = fimc_v4l2_dqbuf(m_cam_fd, 1);

		if (KILLSIG == 1) {
			initScreen(fb_mapped);
			break;
		}

		DrawFromRGB565(fb_mapped, m_buffers_preview[index].virt.p, CAMERA_PREVIEW_WIDTH, CAMERA_PREVIEW_HEIGHT,screen_width,screen_height, camData->saveFlag);
		if (KILLSIG == 1) {
			initScreen(fb_mapped);
			break;
		}

		camData->saveFlag = 0;

		ret = fimc_v4l2_qbuf(m_cam_fd, index);
	}

	stopPreview();
	DestroyCamera();

fb_err:
	close(fb_fd);

	return;
}

static void DrawFromRGB565(unsigned char *displayFrame, unsigned char *videoFrame, int videoWidth, int videoHeight, int dFrameWidth, int dFrameHeight, unsigned short saveFlag)
{
	int x, y;
	int lineLeng;
	unsigned short *videptrTemp;
	unsigned short *videoptr = (unsigned short *)videoFrame;
	lineLeng = dFrameWidth * 4;
	int temp, inv, iter = 0;
	long int totalSize = 0;

	if (camData->saveFlag == 1) {
		strcpy(fileName, "capture");
		sprintf(fileCnt_s,"%d", fileCnt++);
		strcat(fileName, fileCnt_s);
		write_bmp(fileName);
	}

	for (y = 0; y < videoHeight; y++) {
		for (x = 0; x < videoWidth;) {

			totalSize += 12;
			videptrTemp =  videoptr + (videoWidth) * y + x;
			temp = y * lineLeng + x * 4;

			displayFrame[temp + 2] = (unsigned char)((*videptrTemp & 0xF800) >> 8);
			displayFrame[temp + 1] = (unsigned char)((*videptrTemp & 0x07E0) >> 3);
			displayFrame[temp + 0] = (unsigned char)((*videptrTemp & 0x001F) << 3);

			videptrTemp++;
			temp += 4;
			displayFrame[temp + 2] = (unsigned char)((*videptrTemp & 0xF800) >> 8);
			displayFrame[temp + 1] = (unsigned char)((*videptrTemp & 0x07E0) >> 3);
			displayFrame[temp + 0] = (unsigned char)((*videptrTemp & 0x001F) << 3);

			videptrTemp++;
			temp += 4;
			displayFrame[temp + 2] = (unsigned char)((*videptrTemp & 0xF800) >> 8);
			displayFrame[temp + 1] = (unsigned char)((*videptrTemp & 0x07E0) >> 3);
			displayFrame[temp + 0] = (unsigned char)((*videptrTemp & 0x001F) << 3);

			videptrTemp++;
			temp += 4;
			displayFrame[temp + 2] = (unsigned char)((*videptrTemp & 0xF800) >> 8);
			displayFrame[temp + 1] = (unsigned char)((*videptrTemp & 0x07E0) >> 3);
			displayFrame[temp + 0] = (unsigned char)((*videptrTemp & 0x001F) << 3);

			inv = temp;

			if (saveFlag) {
				for (iter=0; iter<4; ++iter) {
					fputc(displayFrame[inv], outfile[outfile_index]);
					fputc(displayFrame[inv+1], outfile[outfile_index]);
					fputc(displayFrame[inv+2], outfile[outfile_index]);
					inv -= 4;
				}
			}
			x+=4;
		}
	}

	if (saveFlag)
		fclose(outfile[outfile_index]);

	return;
}

void write_bmp(char *filename)
{
	unsigned int headers[13];
	int extrabytes;
	int paddedsize;
	int n;

	char pathToSave[100];

	extrabytes = 4 - ((WIDTH * 3) % 4);

	if (extrabytes == 4)
		extrabytes = 0;

	paddedsize = ((WIDTH * 3) + extrabytes) * HEIGHT;

	headers[0]  = paddedsize + 54; // bfSize (whole file size)
	headers[1]  = 0;             // bfReserved (both)
	headers[2]  = 54;            // bfOffbits
	headers[3]  = 40;            // biSize
	headers[4]  = WIDTH; // biWidth
	headers[5]  = HEIGHT; // biHeight

	// Would have biPlanes and biBitCount in position 6, but they're shorts.
	// It's easier to write them out separately (see below) than pretend
	// they're a single int, especially with endian issues...

	headers[7]  = 0;             // biCompression
	headers[8]  = paddedsize;    // biSizeImage
	headers[9]  = 0;             // biXPelsPerMeter
	headers[10] = 0;             // biYPelsPerMeter
	headers[11] = 0;             // biClrUsed
	headers[12] = 0;             // biClrImportant

	sprintf(pathToSave, "%s%s%s", "./captured/", filename, ".bmp");

	outfile_index = (outfile_index + 1) % 5;
	outfile[outfile_index] = fopen(pathToSave, "wb");

	if (outfile[outfile_index] == NULL) {
		printf("BMP File cannot be saved.......\n");
		return;
	}
	strcpy(filename, "capture");

	// Headers begin...
	// When printing ints and shorts, we write out 1 character at a time to avoid endian issues.

	fprintf(outfile[outfile_index], "BM");

	for (n = 0; n <= 5; n++) {
		fprintf(outfile[outfile_index], "%c", headers[n] & 0x000000FF);
		fprintf(outfile[outfile_index], "%c", (headers[n] & 0x0000FF00) >> 8);
		fprintf(outfile[outfile_index], "%c", (headers[n] & 0x00FF0000) >> 16);
		fprintf(outfile[outfile_index], "%c", (headers[n] & (unsigned int) 0xFF000000) >> 24);
	}

	// These next 4 characters are for the biPlanes and biBitCount fields.

	fprintf(outfile[outfile_index], "%c", 1);
	fprintf(outfile[outfile_index], "%c", 0);
	fprintf(outfile[outfile_index], "%c", 24);
	fprintf(outfile[outfile_index], "%c", 0);

	for (n = 7; n <= 12; n++) {
		fprintf(outfile[outfile_index], "%c", headers[n] & 0x000000FF);
		fprintf(outfile[outfile_index], "%c", (headers[n] & 0x0000FF00) >> 8);
		fprintf(outfile[outfile_index], "%c", (headers[n] & 0x00FF0000) >> 16);
		fprintf(outfile[outfile_index], "%c", (headers[n] & (unsigned int) 0xFF000000) >> 24);
	}

	return;
}

static int close_buffers(struct SecBuffer *buffers, int num_of_buf)
{
	int ret, i, j;

	for (i = 0; i < num_of_buf; i++) {
		for (j = 0; j < MAX_PLANES; j++) {
			if (buffers[i].virt.extP[j]) {
				ret = munmap(buffers[i].virt.extP[j], buffers[i].size.extS[j]);
				buffers[i].virt.extP[j] = NULL;
			}
		}
	}

	return 0;
}


static int fimc_poll(struct pollfd *events)
{
	int ret;

	/* 10 second delay is because sensor can take a long time
	 * to do auto focus and capture in dark settings
	 */
	ret = poll(events, 1, 10000);
	if (ret < 0) {
		printf("ERR(%s):poll error\n", __func__);
		return ret;
	}

	if (ret == 0) {
		printf("ERR(%s):No data in 10 secs..\n", __func__);
		return ret;
	}

	return ret;
}

static int fimc_v4l2_querycap(int fp)
{
	struct v4l2_capability cap;

	if (ioctl(fp, VIDIOC_QUERYCAP, &cap) < 0) {
		printf("ERR(%s):VIDIOC_QUERYCAP failed\n", __func__);
		return -1;
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		printf("ERR(%s):no capture devices\n", __func__);
		return -1;
	}

	return 0;
}

static const __u8* fimc_v4l2_enuminput(int fp, int index)
{
	static struct v4l2_input input;

	input.index = index;
	if (ioctl(fp, VIDIOC_ENUMINPUT, &input) != 0) {
		printf("ERR(%s):No matching index found\n", __func__);
		return NULL;
	}

	return input.name;
}

static int fimc_v4l2_s_input(int fp, int index)
{
	struct v4l2_input input;

	input.index = index;

	if (ioctl(fp, VIDIOC_S_INPUT, &input) < 0) {
		printf("ERR(%s):VIDIOC_S_INPUT failed\n", __func__);
		return -1;
	}

	return 0;
}

static int fimc_v4l2_s_fmt(int fp, int width, int height, unsigned int fmt, enum v4l2_field field, unsigned int num_plane)
{
	struct v4l2_format v4l2_fmt;
	struct v4l2_pix_format pixfmt;
	//unsigned int framesize;

	memset(&v4l2_fmt, 0, sizeof(struct v4l2_format));
	v4l2_fmt.type = V4L2_BUF_TYPE;

	memset(&pixfmt, 0, sizeof(pixfmt));

	pixfmt.width = width;
	pixfmt.height = height;
	pixfmt.pixelformat = fmt;
	pixfmt.field = V4L2_FIELD_NONE;

	v4l2_fmt.fmt.pix = pixfmt;

	/* Set up for capture */
	if (ioctl(fp, VIDIOC_S_FMT, &v4l2_fmt) < 0) {
		printf("ERR(%s):VIDIOC_S_FMT failed\n", __func__);
		return -1;
	}

	return 0;
}

int fimc_v4l2_s_fmt_is(int fp, int width, int height, unsigned int fmt, enum v4l2_field field)
{
	struct v4l2_format v4l2_fmt;
	struct v4l2_pix_format pixfmt;

	memset(&pixfmt, 0, sizeof(pixfmt));

	v4l2_fmt.type = V4L2_BUF_TYPE_PRIVATE;

	pixfmt.width = width;
	pixfmt.height = height;
	pixfmt.pixelformat = fmt;
	pixfmt.field = field;

	v4l2_fmt.fmt.pix = pixfmt;

	/* Set up for capture */
	if (ioctl(fp, VIDIOC_S_FMT, &v4l2_fmt) < 0) {
		printf("ERR(%s):VIDIOC_S_FMT failed\n", __func__);
		return -1;
	}

	return 0;
}

static int fimc_v4l2_enum_fmt(int fp, unsigned int fmt)
{
	struct v4l2_fmtdesc fmtdesc;
	int found = 0;

	fmtdesc.type = V4L2_BUF_TYPE;
	fmtdesc.index = 0;

	while (ioctl(fp, VIDIOC_ENUM_FMT, &fmtdesc) == 0) {
		if (fmtdesc.pixelformat == fmt) {
			printf("passed fmt = %#x found pixel format[%d]: %s\n", fmt, fmtdesc.index, fmtdesc.description);
			found = 1;
			break;
		}
		fmtdesc.index++;
	}

	if (!found) {
		printf("unsupported pixel format\n");
		return -1;
	}

	return 0;
}

static int fimc_v4l2_reqbufs(int fp, enum v4l2_buf_type type, int nr_bufs)
{
	struct v4l2_requestbuffers req;

	req.count = nr_bufs;
	req.type = type;
	req.memory = V4L2_MEMORY_TYPE;

	if (ioctl(fp, VIDIOC_REQBUFS, &req) < 0) {
		printf("ERR(%s):VIDIOC_REQBUFS failed\n", __func__);
		return -1;
	}

	return req.count;
}

static int fimc_v4l2_querybuf(int fp, struct SecBuffer *buffers, enum v4l2_buf_type type, int nr_frames, int num_plane)
{
	struct v4l2_buffer v4l2_buf;
	int i, ret;

	for (i = 0; i < nr_frames; i++) {
		v4l2_buf.type = type;
		v4l2_buf.memory = V4L2_MEMORY_TYPE;
		v4l2_buf.index = i;

		ret = ioctl(fp, VIDIOC_QUERYBUF, &v4l2_buf); // query video buffer status
		if (ret < 0) {
			printf("ERR(%s):VIDIOC_QUERYBUF failed\n", __func__);
			return -1;
		}

		buffers[i].size.s = v4l2_buf.length;

		if ((buffers[i].virt.p = (char *)mmap(0, v4l2_buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fp, v4l2_buf.m.offset)) < 0) {
			printf("%s %d] mmap() failed",__func__, __LINE__);
			return -1;
		}
	}

	return 0;
}

static int fimc_v4l2_streamon(int fp)
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE;
	int ret;

	ret = ioctl(fp, VIDIOC_STREAMON, &type);
	if (ret < 0) {
		printf("ERR(%s):VIDIOC_STREAMON failed\n", __func__);
		return ret;
	}

	return ret;
}

static int fimc_v4l2_streamoff(int fp)
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE;
	int ret;

	ret = ioctl(fp, VIDIOC_STREAMOFF, &type);
	if (ret < 0) {
		printf("ERR(%s):VIDIOC_STREAMOFF failed\n", __func__);
		return ret;
	}

	return ret;
}

static int fimc_v4l2_qbuf(int fp, int index)
{
	struct v4l2_buffer v4l2_buf;
	int ret;

	v4l2_buf.type = V4L2_BUF_TYPE;
	v4l2_buf.memory = V4L2_MEMORY_TYPE;
	v4l2_buf.index = index;

	ret = ioctl(fp, VIDIOC_QBUF, &v4l2_buf);
	if (ret < 0) {
		printf("ERR(%s):VIDIOC_QBUF failed\n", __func__);
		return ret;
	}

	return 0;
}

static int fimc_v4l2_dqbuf(int fp, int num_plane)
{
	struct v4l2_buffer v4l2_buf;
	int ret;

	v4l2_buf.type = V4L2_BUF_TYPE;
	v4l2_buf.memory = V4L2_MEMORY_TYPE;

	ret = ioctl(fp, VIDIOC_DQBUF, &v4l2_buf);
	if (ret < 0) {
		printf("ERR(%s):VIDIOC_DQBUF failed, dropped frame\n", __func__);
		return ret;
	}

	return v4l2_buf.index;
}

static int fimc_v4l2_s_ctrl(int fp, unsigned int id, unsigned int value)
{
	struct v4l2_control ctrl;
	int ret;

	ctrl.id = id;
	ctrl.value = value;

	ret = ioctl(fp, VIDIOC_S_CTRL, &ctrl);
	if (ret < 0) {
		printf("ERR(%s):VIDIOC_S_CTRL(id = %#x (%d), value = %d) failed ret = %d\n",
				__func__, id, id-V4L2_CID_PRIVATE_BASE, value, ret);

		return ret;
	}

	return ctrl.value;
}


int CreateCamera(int index)
{
	int ret = 0;

	m_cam_fd = open(CAMERA_DEV_NAME, O_RDWR);
	if (m_cam_fd < 0) {
		printf("ERR(%s):Cannot open %s (error : %s)\n", __func__, CAMERA_DEV_NAME, strerror(errno));
		return -1;
	}

	ret = fimc_v4l2_querycap(m_cam_fd);
	CHECK(ret);
	if (!fimc_v4l2_enuminput(m_cam_fd, index)) {
		printf("m_cam_fd(%d) fimc_v4l2_enuminput fail\n", m_cam_fd);
		return -1;
	}
	ret = fimc_v4l2_s_input(m_cam_fd, index);
	CHECK(ret);

	return 0;
}
void DestroyCamera()
{
	if (m_cam_fd > -1) {
		close(m_cam_fd);
		m_cam_fd = -1;
	}

	return;
}

int startPreview(void)
{
	int i;
	//v4l2_streamparm streamparm;

	if (m_cam_fd <= 0) {
		printf("ERR(%s):Camera was closed\n", __func__);
		return -1;
	}

	memset(&m_events_c, 0, sizeof(m_events_c));
	m_events_c.fd = m_cam_fd;
	m_events_c.events = POLLIN | POLLERR;

	/* enum_fmt, s_fmt sample */
	int ret = fimc_v4l2_enum_fmt(m_cam_fd,m_preview_v4lformat);
	CHECK(ret);

	ret = fimc_v4l2_s_fmt(m_cam_fd, CAMERA_PREVIEW_WIDTH, CAMERA_PREVIEW_HEIGHT, m_preview_v4lformat, V4L2_FIELD_ANY, PREVIEW_NUM_PLANE);
	CHECK(ret);

	fimc_v4l2_s_fmt_is(m_cam_fd, CAMERA_PREVIEW_WIDTH, CAMERA_PREVIEW_HEIGHT, m_preview_v4lformat, (enum v4l2_field) IS_MODE_PREVIEW_STILL);
	CHECK(ret);

	ret = fimc_v4l2_s_ctrl(m_cam_fd, V4L2_CID_CACHEABLE, 1);
	CHECK(ret);

	ret = fimc_v4l2_reqbufs(m_cam_fd, V4L2_BUF_TYPE, MAX_BUFFERS);
	CHECK(ret);

	ret = fimc_v4l2_querybuf(m_cam_fd, m_buffers_preview, V4L2_BUF_TYPE, MAX_BUFFERS, PREVIEW_NUM_PLANE);
	CHECK(ret);

	/* start with all buffers in queue  to capturer video */
	for (i = 0; i < MAX_BUFFERS; i++) {
		ret = fimc_v4l2_qbuf(m_cam_fd, i);
		CHECK(ret);
	}

	ret = fimc_v4l2_streamon(m_cam_fd);
	CHECK(ret);

	return 0;
}

int stopPreview(void)
{
	int ret;

	if (m_cam_fd <= 0) {
		printf("ERR(%s):Camera was closed\n", __func__);
		return -1;
	}

	ret = fimc_v4l2_streamoff(m_cam_fd);
	CHECK(ret);

	close_buffers(m_buffers_preview, MAX_BUFFERS);

	fimc_v4l2_reqbufs(m_cam_fd, V4L2_BUF_TYPE, 0);

	return ret;
}

void initScreen(unsigned char *fb_mem)
{
	int coor_x, coor_y;
	unsigned long *ptr;

	for (coor_y = 0; coor_y < screen_height; coor_y++) {
		ptr = (unsigned long *)fb_mem + screen_width * coor_y;
		for (coor_x = 0; coor_x < screen_width; coor_x++) {
			*ptr++ = 0x00000000;
		}
	}

	return;
}
