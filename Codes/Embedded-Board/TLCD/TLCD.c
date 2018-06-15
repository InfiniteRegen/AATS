#include "../main.h"

#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define SUCCESS		0
#define FAIL		1

#define DRIVER_NAME			"/dev/cntlcd"
/******************************************************************************
*
*      TEXT LCD FUNCTION
*
******************************************************************************/
#define CLEAR_DISPLAY		0x0001
#define CURSOR_AT_HOME		0x0002

// Entry Mode set
#define MODE_SET_DEF		0x0004
#define MODE_SET_DIR_RIGHT	0x0002
#define MODE_SET_SHIFT		0x0001

// Display on off
#define DIS_DEF				0x0008
#define DIS_LCD				0x0004
#define DIS_CURSOR			0x0002
#define DIS_CUR_BLINK		0x0001

// shift
#define CUR_DIS_DEF			0x0010
#define CUR_DIS_SHIFT		0x0008
#define CUR_DIS_DIR			0x0004

// set DDRAM  address
#define SET_DDRAM_ADD_DEF	0x0080

// read bit
#define BUSY_BIT			0x0080
#define DDRAM_ADD_MASK		0x007F


#define DDRAM_ADDR_LINE_1	0x0000
#define DDRAM_ADDR_LINE_2	0x0040


#define SIG_BIT_E			0x0400
#define SIG_BIT_RW			0x0200
#define SIG_BIT_RS			0x0100

static int fd ;
/***************************************************
read /write  sequence
write cycle
RS,(R/W) => E (rise) => Data => E (fall)

***************************************************/
int IsBusy(void)
{
	unsigned short wdata, rdata;

	wdata = SIG_BIT_RW;
	write(fd, &wdata, 2);

	wdata = SIG_BIT_RW | SIG_BIT_E;
	write(fd, &wdata, 2);

	read(fd, &rdata, 2);

	wdata = SIG_BIT_RW;
	write(fd, &wdata, 2);

	if (rdata &  BUSY_BIT)
		return TRUE;

	return FALSE;
}

int writeCmdT(unsigned short cmd)
{
	unsigned short wdata;

	if (IsBusy())
		return FALSE;

	wdata = cmd;
	write(fd, &wdata, 2);

	wdata = cmd | SIG_BIT_E;
	write(fd, &wdata, 2);

	wdata = cmd ;
	write(fd, &wdata, 2);

	return TRUE;
}

int setDDRAMAddr(int x, int y)
{
	unsigned short cmd = 0;

	if (IsBusy()) {
		perror("setDDRAMAddr busy error.\n");
		return FALSE;
	}

	if (y == 1) {
		cmd = DDRAM_ADDR_LINE_1 + x;
	}
	else if (y == 2) {
		cmd = DDRAM_ADDR_LINE_2 + x;
	}
	else
		return FALSE;

	if (cmd >= 0x80)
		return FALSE;

	if (!writeCmdT(cmd | SET_DDRAM_ADD_DEF)) {
		perror("setDDRAMAddr error\n");
		return FALSE;
	}

	usleep(1000);

	return TRUE;
}

int displayMode(int bCursor, int bCursorblink, int blcd)
{
	unsigned short cmd = 0;

	if (bCursor) {
		cmd = DIS_CURSOR;
	}

	if (bCursorblink) {
		cmd |= DIS_CUR_BLINK;
	}

	if (blcd) {
		cmd |= DIS_LCD;
	}

	if (!writeCmdT(cmd | DIS_DEF))
		return FALSE;

	return TRUE;
}

int writeCh(unsigned short ch)
{
	unsigned short wdata = 0;

	if (IsBusy())
		return FALSE;

	wdata = SIG_BIT_RS | ch;
	write(fd, &wdata, 2);

	wdata = SIG_BIT_RS | ch | SIG_BIT_E;
	write(fd, &wdata, 2);

	wdata = SIG_BIT_RS | ch;
	write(fd, &wdata, 2);

	usleep(1000);

	return TRUE;
}


int setCursorMode(int bMove, int bRightDir)
{
	unsigned short cmd = MODE_SET_DEF;

	if (bMove)
		cmd |= MODE_SET_SHIFT;

	if (bRightDir)
		cmd |= MODE_SET_DIR_RIGHT;

	if (!writeCmdT(cmd))
		return FALSE;

	return TRUE;
}

int functionSet(void)
{
	unsigned short cmd = 0x0038; // 5*8 dot charater , 8bit interface , 2 line

	if (!writeCmdT(cmd))
		return FALSE;

	return TRUE;
}

int writeStr(char* str)
{
	unsigned char wdata;
	int i;

	for (i = 0; i < strlen(str); i++) {
		if (str[i] == '_')
			wdata = (unsigned char)' ';
		else
			wdata = str[i];
		writeCh(wdata);
	}

	return TRUE;
}

#define LINE_NUM 	2
#define COLUMN_NUM	16

int clearScreen(int nline)
{
	int i;
	switch (nline) {
		case 0:
			if (IsBusy()) {
				perror("clearScreen error\n");
				return FALSE;
			}
			if (!writeCmdT(CLEAR_DISPLAY))
				return FALSE;
			return TRUE;

		case 1:
			setDDRAMAddr(0, 1);
			for (i = 0; i <= COLUMN_NUM; i++) {
				writeCh((unsigned char)' ');
			}
			setDDRAMAddr(0, 1);
		break;

		case 2:
			setDDRAMAddr(0, 2);
			for (i = 0; i <= COLUMN_NUM; i++) {
				writeCh((unsigned char)' ');
			}
			setDDRAMAddr(0, 2);
		break;
	}

	return TRUE;
}

void doHelp(void)
{
	printf("Usage:\n");
	printf("tlcdtest w line string :=>display the string  at line  , charater  '_' =>' '\n");
	printf(" ex) tlcdtest w 0 cndi_text_test :=>display 'cndi text test' at line 1 \n");
	printf("tlcdtest c on|off blink line column : \n");
	printf(" => cursor set on|off =>1 or 0 , b => blink 1|0 , line column line position \n");
	printf("tlcdtset c  1 1 2 12  :=> display blink cursor at 2 line , 12 column.\n");
	printf("tlcdtest r [line] : => clear screen or clear line \n");
	printf("tlcdtest r  : => clear screen \n");
	printf("tlcdtest r 1: => clear line 1 \n");

	return;
}


#define CMD_TXT_WRITE		0
#define CMD_CURSOR_POS		1
#define CMD_CEAR_SCREEN		2

void DisplayTLCD(int mode)
{
	int nCmdMode;
	int bCursorOn, bBlink, nline, nColumn;
	char strWtext[COLUMN_NUM+1];

	nline = 2; // LINE NUMBER '1' or '2' only

	fd = open(DRIVER_NAME, O_RDWR);
	if (fd < 0) {
		perror("driver open error.\n");
		return;
	}

	functionSet();

	bCursorOn = 1;
	bBlink = 1;
	nColumn = 1;

	if (mode == 0) {
		clearScreen(nline);
		return;
	}

	clearScreen(nline);
	displayMode(bCursorOn, bBlink, TRUE);
	setDDRAMAddr(nline-1, nColumn);
	setDDRAMAddr(nColumn, nline);
	usleep(2000);
	strcpy(strWtext, "GET OUT!");
	writeStr(strWtext);

	close(fd);

	return;
}
