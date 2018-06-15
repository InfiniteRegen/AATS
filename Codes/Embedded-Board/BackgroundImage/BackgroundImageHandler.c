#include "../main.h"
#include "BackgroundImageHandler.h"

#include <fcntl.h>      // for O_RDWR
#include <sys/ioctl.h>  // for ioctl
#include <sys/mman.h>
#include <linux/fb.h>   // for fb_var_screeninfo, FBIOGET_VSCREENINFO

#define FBDEV_FILE        "/dev/fb0"
#define BIT_VALUE_24BIT   24

#define BACKGROUND_IMAGE_PATH "./main_back.bmp"

void read_bmp(char* filename, char** pDib, char** data, int* cols, int* rows)
{
        BITMAPFILEHEADER bmpHeader;
        BITMAPINFOHEADER *bmpInfoHeader;
        unsigned int size;
        unsigned char magicNum[2];
        int nread;
        FILE *fp;

        fp  =  fopen(filename, "rb");
        if (fp == NULL) {
                printf("ERROR\n");
                return;
        }

        // identify bmp file
        magicNum[0]   =   fgetc(fp);
        magicNum[1]   =   fgetc(fp);
        printf("magicNum : %c%c\n", magicNum[0], magicNum[1]);

        if (magicNum[0] != 'B' && magicNum[1] != 'M') {
                printf("It's not a bmp file!\n");
                fclose(fp);
                return;
        }

        nread   =   fread(&bmpHeader.bfSize, 1, sizeof(BITMAPFILEHEADER), fp);
        size    =   bmpHeader.bfSize - sizeof(BITMAPFILEHEADER);
        *pDib   =   (unsigned char *)malloc(size);  // DIB Header(Image Header)
        fread(*pDib, 1, size, fp);
        bmpInfoHeader   =   (BITMAPINFOHEADER *)*pDib;

        printf("nread : %d\n", nread);
        printf("size : %d\n", size);

        // check 24bit
        if (BIT_VALUE_24BIT != (bmpInfoHeader->biBitCount)) { // bit value
                printf("It supports only 24bit bmp!\n");
                fclose(fp);
                return;
        }

        *cols   =   bmpInfoHeader->biWidth;
        *rows   =   bmpInfoHeader->biHeight;
        *data   =   (char *)(*pDib + bmpHeader.bfOffBits - sizeof(bmpHeader) - 2);
        fclose(fp);

        return;
}

void close_bmp(char** pDib)     // DIB(Device Independent Bitmap)
{
        free(*pDib);

        return;
}

//void *showBackground(void *param)
void ShowBackgroundImage(touch_t* input)
{
        int i, j, k, t;
        int fbfd;
        int screen_width;
        int screen_height;
        int bits_per_pixel;
        int line_length;
        int coor_x, coor_y;
        int cols = 0, rows = 0;
        int mem_size;

        char *pData, *data;
        char r, g, b;
        unsigned long bmpdata[1280*960];
        unsigned long pixel;
        unsigned char   *pfbmap;
        unsigned long   *ptr;
        struct  fb_var_screeninfo fbvar;
        struct  fb_fix_screeninfo fbfix;

        //pthread_detach(pthread_self());

        read_bmp(BACKGROUND_IMAGE_PATH, &pData, &data, &cols, &rows);
        printf("[#BackGround] columns = %d, rows = %d\n", cols, rows);

        for (j = 0; j < rows; j++) {
                k = j * cols * 3;
                t = (rows - 1 - j) * cols;

                for (i = 0; i < cols; i++) {
                        b = *(data + (k + i * 3));
                        g = *(data + (k + i * 3 + 1));
                        r = *(data + (k + i * 3 + 2));

                        pixel = ((r<<16) | (g<<8) | b);
                        bmpdata[t + i] = pixel; // save bitmap data bottom up
                }
        }

        close_bmp(&pData);

        if ((fbfd = open(FBDEV_FILE, O_RDWR)) < 0) {
                printf("%s: open error\n", FBDEV_FILE);
                exit(1);
        }

        if (ioctl(fbfd, FBIOGET_VSCREENINFO, &fbvar)) {
                printf("%s: ioctl error - FBIOGET_VSCREENINFO \n", FBDEV_FILE);
                exit(1);
        }

        if (ioctl(fbfd, FBIOGET_FSCREENINFO, &fbfix)) {
                printf("%s: ioctl error - FBIOGET_FSCREENINFO \n", FBDEV_FILE);
                exit(1);
        }

        if (fbvar.bits_per_pixel != 32) {
                fprintf(stderr, "bpp is not 32\n");
                exit(1);
        }

        screen_width    =   fbvar.xres;
        screen_height   =   fbvar.yres;
        bits_per_pixel  =   fbvar.bits_per_pixel;
        line_length     =   fbfix.line_length;
        mem_size        =   line_length * screen_height;

        pfbmap = (unsigned char *)mmap(0, mem_size, PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);

        if ((unsigned)pfbmap == (unsigned)-1) {
                perror("fbdev mmap\n");
                exit(1);
        }

        // fb clear - black
        for (coor_y = 0; coor_y < screen_height; coor_y++) {
                ptr = (unsigned long *)pfbmap + (screen_width * coor_y);

                for (coor_x = 0; coor_x < screen_width; coor_x++) {
                        *ptr++ = 0x000000;
                }
        }

        // direction for image generating : (0,0)-> (1,0)-> (2,0)-> ...-> (row, column)
        while (TRUE) {
                for (coor_y = 0; coor_y < rows; coor_y++) {
                        ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
                        for (coor_x = 0; coor_x < cols; coor_x++) {
                                *ptr++ = bmpdata[coor_x + (coor_y * cols)];
                        }
                }

                if (input->thBut == 1) {
                        printf("[#BACKGROUND] THREAT: [ON]\n");
                        input->modeFlag = 1;
                        break;
                }
                else if (input->supBut == 1) {
                        printf("[#BACKGROUND] SUPRESSION: [ON]\n");
                        input->modeFlag = 1;
                        break;
                }
                else if (input->haltBut == 1) {
                        printf("[#BACKGROUND] HALT: [ON]\n");
                        printf("**************************************\n");
                        printf("*********[ THE AATS IS SHUTTING DOWN ]*********\n");
                        printf("**************************************\n");

                        for (coor_y = 0; coor_y < rows; coor_y++) {
                                ptr = (unsigned long*)pfbmap + (screen_width * coor_y);
                                for (coor_x = 0; coor_x < cols; coor_x++)
                                        *ptr++ = 0x0000000;
                        }

                        munmap(pfbmap, mem_size);
                        close(fbfd);
                        exit(-1);
                }
        }
        munmap(pfbmap, mem_size);
        close(fbfd);

        return;
}
