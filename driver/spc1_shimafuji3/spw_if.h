#ifndef _SPW_IF_H_
#define _SPW_IF_H_

#include <basic.h>
#include <btron/proctask.h>
#include <btron/tkcall.h>

#include <stdio.h>
#include <stdlib.h>

/* SpaceWire�h���C�o : �f�[�^�ԍ���` */
enum {
	DN_LINKC  = -200,	/* link control register �̑����f�[�^�ԍ� */
	DN_LINKS  = -201,	/* link status register �̑����f�[�^�ԍ� */
	DN_TIMIN  = -202,	/* timein(time-code���M) register �̑����f�[�^�ԍ�, �����݂̂� */
	DN_TIMOUT = -203,	/* timeout(time-code��M) register �̑����f�[�^�ԍ�, �Ǐo���̂� */
	DN_INTC   = -204,	/* interrupt control register �̑����f�[�^�ԍ� */
	DN_SIGN   = -205,	/* fpga signature register �̑����f�[�^�ԍ�, �Ǐo���̂� */
	DN_TXTMO  = -206,	/* ���M�^�C���A�E�g(ms) */
	DN_RXTMO  = -207	/* ��M�^�C���A�E�g(ms) */
};

typedef enum {
	TXSPD_100MBPS =		0,
	TXSPD_50MBPS =		1,
	TXSPD_33MBPS =		2,
	TXSPD_25MBPS =		3,
	TXSPD_20MBPS =		4,
	TXSPD_10MBPS =		9,
	TXSPD_5MBPS =		19,
	TXSPD_2MBPS =		49
} spw_txclk_div;

int spw_open(int portnumber);
int spw_close(int dd);
int spw_connect(int dd);
int spw_read(int dd,UW *buf, UW *size);
int spw_write(int dd,UW *buf, UW size);
int spw_send_timecode(int dd,unsigned char flag_and_timecode);
int spw_get_timecode(int dd,unsigned char* flag_and_timecode);

int spw_set_txspeed(int dd,spw_txclk_div new_txclk_div);
int spw_set_read_timeout(int dd,UW time_out);
int spw_set_write_timeout(int dd,UW time_out);
#endif /*_SPW_IF_H_*/
