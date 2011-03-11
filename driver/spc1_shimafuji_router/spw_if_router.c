#include <basic.h>
#include <btron/proctask.h>
#include <btron/tkcall.h>

#include <stdio.h>
#include <stdlib.h>


#include "spw_if_router.h"

int spwr_open(int portnum)
{
	ER er;
	UW data;
	int asize;
	int dd;
	if(portnum==4){
		dd = tk_opn_dev((UB*)"spwa", TD_UPDATE);
	}else if(portnum==5){
		dd = tk_opn_dev((UB*)"spwb", TD_UPDATE);
	}else if(portnum==6){
		dd = tk_opn_dev((UB*)"spwc", TD_UPDATE);
	}else{
		printf("open error : invalid port number\n");
		return -1;
	}

	if (dd < 0) {
		printf("open error (er=0x%8x)\n", dd);
		return dd;
	}

	/* Enable Timecode output to Port 1,2,3
	*/
	data=0x0000000e; //enable timecode output to port 1,2,3
	er = tk_swri_dev(dd, DN_TCODEEN, &data, sizeof(data), &asize);

	er = tk_srea_dev(dd, DN_SIGN, &data, sizeof(data), &asize);
	if (er == E_PAR) printf("E_PAR");
	else {
		printf("fpga ver. = 0x%04x\n", (unsigned short)(0x0000ffff & data));
	}

	return dd;
}


int spwr_close(int dd)
{
	if (dd > 0) {
		return 0;
		tk_cls_dev(dd, 0);
	} else {
		return 1;
	}
}


int spwr_set_link1_txspeed(int dd,spwr_txclk_div txclk_div)
{
	ER er;
	UW data;
	int asize;

	er = tk_srea_dev(dd, DN_SPW1CSR, &data, sizeof(data), &asize);
	if (asize != 4) {
		return E_IO;
	}
	data = data | (((UW)txclk_div << 24) & 0x3f000000);
	er = tk_swri_dev(dd, DN_SPW1CSR, &data, sizeof(data), &asize);
	if (asize != 4) {
		return E_IO;
	}
	return er;
}


int spwr_set_link2_txspeed(int dd,spwr_txclk_div txclk_div)
{
	ER er;
	UW data;
	int asize;

	er = tk_srea_dev(dd, DN_SPW2CSR, &data, sizeof(data), &asize);
	if (asize != 4) {
		return E_IO;
	}
	data = data | (((UW)txclk_div << 24) & 0x3f000000);
	er = tk_swri_dev(dd, DN_SPW2CSR, &data, sizeof(data), &asize);
	if (asize != 4) {
		return E_IO;
	}
	return er;
}


int spwr_set_link3_txspeed(int dd,spwr_txclk_div txclk_div)
{
	ER er;
	UW data;
	int asize;

	er = tk_srea_dev(dd, DN_SPW3CSR, &data, sizeof(data), &asize);
	if (asize != 4) {
		return E_IO;
	}
	data = data | (((UW)txclk_div << 24) & 0x3f000000);
	er = tk_swri_dev(dd, DN_SPW3CSR, &data, sizeof(data), &asize);
	if (asize != 4) {
		return E_IO;
	}
	return er;
}

int spwr_set_txspeed(int dd,int port, spwr_txclk_div txclk_div)
{
	if (port == 1) return spwr_set_link1_txspeed(dd,txclk_div);
	else if (port == 2) return spwr_set_link2_txspeed(dd,txclk_div);
	else if (port == 3) return spwr_set_link3_txspeed(dd,txclk_div);
	else return E_PAR;
}


int spwr_set_read_timeout(int dd,UW time_out)
{
	ER er;
	UW data;
	int asize;

	data = time_out;
	er = tk_swri_dev(dd, DN_RXTMO, &data, sizeof(data), &asize);
	if (asize != 4) {
		return E_IO;
	}
	return er;
}

int spwr_set_write_timeout(int dd,UW time_out)
{
	ER er;
	UW data;
	int asize;

	data = time_out;
	er = tk_swri_dev(dd, DN_TXTMO, &data, sizeof(data), &asize);
	if (asize != 4) {
		return E_IO;
	}
	return er;
}


int spwr_read(int dd,UW *buf, UW *size)
{
	ER er;
	int asize;

	ID requestid;
	ER ioer;

	/*
	 * check link here ?
	 */

	loop:
	//printf("*1\n");
	requestid = tk_rea_dev(dd, 0, buf, SPWR_READ_MAXSIZE, TMO_FEVR);
	if (requestid < 0) {
		//printf("spwr_read() wait\n");
		goto loop;
	}
	//if (er == E_PAR) printf("E_PAR");
	if (requestid < E_OK) {printf("ERROR");}
	//printf("*2\n");
	er = tk_wai_dev(dd, requestid, &asize, &ioer, TMO_FEVR);
	if (er == E_PAR){
		//printf("E_PAR");
	}else if (ioer == E_TMOUT){
		//printf("E_TMOUT\n");
	}

	//printf("_%d",asize);

	//printf("*3\n");
	if (asize > SPWR_READ_MAXSIZE) {
		*size = asize;
		return -1;
	} else {
		*size = asize;
		return ioer;
	}
}


int spwr_write(int dd,UW *buf, UW size)
{
	ER er;
	int asize;

	/*
	 * check link here ?
	 */

//	printf("spwr_write start\n");

	er = tk_swri_dev(dd, 0, buf, size, &asize);

//	printf("spwr_write done\n");

	if (size == asize) {
		return 0;
	} else {
		return 1;
	}
}


int spwr_send_timecode(int dd,unsigned char flag_and_timecode){
	ER er;
	unsigned int data;
	int asize;
	int dd;

	data=0x00000000+flag_and_timecode;
	er = tk_swri_dev(dd, DN_TCOUT, &data, sizeof(data), &asize);

	return er;
}

int spwr_get_timecode(int dd,unsigned char* flag_and_timecode){
	ER er;
	unsigned int data;
	int asize;
	int dd;

	er = tk_srea_dev(dd, DN_TCIN, &data, 4, &asize);
	
	*flag_and_timecode=(unsigned char)(data & 0x000000ff);
	return er;
	
}



/* ????????????????? */

int spwr_table_write(int dd,UW index, UW data)
{
	ER er;
	int asize, data0;

	data0 = index;
	er = tk_swri_dev(dd, DN_TBLIDX, &data0, sizeof(data0), &asize);
	if (er != E_OK) return er;

	data0 = data;
	er = tk_swri_dev(dd, DN_TBLDAT, &data0, sizeof(data0), &asize);
	return er;
}

UW spwr_table_read(int dd,UW index)
{
	ER er;
	int asize, data0;

	data0 = index;
	er = tk_swri_dev(dd, DN_TBLIDX, &data0, sizeof(data0), &asize);
	if (er != E_OK) return 0xffffffff;

	er = tk_srea_dev(dd, DN_TBLDAT, &data0, sizeof(data0), &asize);
	if (er != E_OK) return 0xffffffff;
	return data0;
}


/** 2008-10-08 added
 */

/** reset port
 */
int spwr_reset_link(int dd,int portnum){
	ER er;
	UW data;
	int asize;

	if(portnum<1 || 6<portnum){
		return E_PAR;
	}

	int address;
	switch(portnum){
	case 1: address=DN_SPW1CSR;break;
	case 2: address=DN_SPW2CSR;break;
	case 3: address=DN_SPW3CSR;break;
	case 4: address=DN_SPW4CSR;break;
	case 5: address=DN_SPW5CSR;break;
	case 6: address=DN_SPW6CSR;break;
	default: return E_PAR;break;
	}

	er = tk_srea_dev(dd, address, &data, sizeof(data), &asize);
	if (asize != 4) {
		return E_IO;
	}
	data = data & 0x00ffffff;
	data = data | SPW_LINKRESET;


	er = tk_swri_dev(dd, address, &data, sizeof(data), &asize);
	if (asize != 4) {
		er = E_IO;
	}

	return er;
}


/** */
int spwr_get_linkstatus(int dd,int portnum,unsigned int* status){
	ER er;
	UW data;
	int asize;

	if(portnum<1 || 6<portnum){
		return E_PAR;
	}

	int address;
	switch(portnum){
	case 1: address=DN_SPW1CSR;break;
	case 2: address=DN_SPW2CSR;break;
	case 3: address=DN_SPW3CSR;break;
	case 4: address=DN_SPW4CSR;break;
	case 5: address=DN_SPW5CSR;break;
	case 6: address=DN_SPW6CSR;break;
	default: return E_PAR;break;
	}

	er = tk_srea_dev(dd, address, &data, sizeof(data), &asize);
	if (asize != 4) {
		tk_cls_dev(dd, 0);
		return E_IO;
	}

	*status=data;
	return er;
}
