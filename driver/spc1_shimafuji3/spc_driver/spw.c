/*
 * SpaceWire I/F
 */

#include <tk/tkernel.h>
#include <tk/util.h>
#include <device/gdrvif.h>
#include <device/pci.h>
#include <string.h>

#include <stdio.h>

/* $BD/include/tk/vr/intc_vr5701.h */
#define INTNO_GPIO6		IV_GPIO(6)		/* int ch0 */
#define INTNO_GPIO7		IV_GPIO(7)		/* int ch1 */
#define INTNO_GPIO8		IV_GPIO(8)		/* int ch2 */
#define INTNO_GPIO9		IV_GPIO(9)		/* not-used */
#define INTNO_GPIO10	IV_GPIO(10)		/* not-used */
#define INTNO_GPIO11	IV_GPIO(11)		/* not-used */
#define INTNO_EPCIA		IV_EPCI(0)		/* inta# */

/* fpga registers */
/*#define FPGA_BASE			0xb0000000*/
#define FPGA_BASE		pci9054_space0_base

#define SPW_CH0_TXBASE		(FPGA_BASE + 0x0000000)
#define SPW_CH0_RXBASE		(FPGA_BASE + 0x0000800)
#define SPW_CH1_TXBASE		(FPGA_BASE + 0x0002000)
#define SPW_CH1_RXBASE		(FPGA_BASE + 0x0002800)
#define SPW_CH2_TXBASE		(FPGA_BASE + 0x0004000)
#define SPW_CH2_RXBASE		(FPGA_BASE + 0x0004800)
#define SPW_CH0_REGBASE		(FPGA_BASE + 0x0001f00)
#define SPW_CH1_REGBASE		(FPGA_BASE + 0x0003f00)
#define SPW_CH2_REGBASE		(FPGA_BASE + 0x0005f00)

#define LNKC_OFFSET			0x00
#define LNKS_OFFSET			0x04
#define TIMIN_OFFSET		0x08
#define TIMOUT_OFFSET		0x0c
#define INTC_OFFSET			0x10
#define INTS_OFFSET			0x14
#define TXCSR_OFFSET		0x18
#define RXCSR_OFFSET		0x1c
#define SIGN_OFFSET			0xfc

#define REG_OFFSET			0x1f00
#define TXBUF_OFFSET		0x0
#define RXBUF_OFFSET		0x800
#define PORT_OFFSET			0x2000

/* register address */
#define TIMOUT0_REG		(SPW_CH0_REGBASE + 0x0c)	/* r */
#define INTC0_REG		(SPW_CH0_REGBASE + 0x10)	/* rw */
#define INTS0_REG		(SPW_CH0_REGBASE + 0x14)	/* rw */

#define TIMOUT1_REG		(SPW_CH1_REGBASE + 0x0c)	/* r */
#define INTC1_REG		(SPW_CH1_REGBASE + 0x10)	/* rw */
#define INTS1_REG		(SPW_CH1_REGBASE + 0x14)	/* rw */

#define TIMOUT2_REG		(SPW_CH2_REGBASE + 0x0c)	/* r */
#define INTC2_REG		(SPW_CH2_REGBASE + 0x10)	/* rw */
#define INTS2_REG		(SPW_CH2_REGBASE + 0x14)	/* rw */

/* link interrupt flags */
#define TXBUFFER_RDY	0x00000001
#define RXBUFFER_DONE	0x00000002
#define TCODE_RECVED	0x00000004
#define LINK_CHANGE		0x00000008

#define FLG_ABORTED		0x80000000

#define TXBUF_EOP		0x40000000
#define TXBUF_EEP		0x20000000
#define TXBUF_START		0x80000000

#define RXBUF_EOP		0x40000000
#define RXBUF_EEP		0x20000000
#define RXBUF_DONE		0x80000000

/* link control flags */
#define SPW_LINKENABLE		0x00000001
#define SPW_LINKDISABLE		0x00000002
#define SPW_AUTOSTART		0x00000004

/* link status flags */
#define ENABLE_RX		0x00000001
#define ENABLE_TX		0x00000002
#define SEND_NULLS		0x00000004
#define SEND_FCTS		0x00000008
#define SEND_N_CHARS	0x00000010
#define SEND_TCODES		0x00000020
#define SEQUENCE_ERR	0x00000100
#define CREDIT_ERR		0x00000200
#define RX_ERR			0x00000400
#define PARITY_ERR		0x00001000
#define DISCONNECT_ERR	0x00002000
#define ESCAPE_ERR		0x00004000

/* plx registers */
/*#define PLX_REGBASE		0xb3dfff00*/
#define PLX_REGBASE		pci9054_config_base
#define PLX_LAS0BA		(PLX_REGBASE + 0x04)
#define PLX_LBRD0		(PLX_REGBASE + 0x18)

#define PLX_INTCSR			(PLX_REGBASE + 0x68)
#define PLX_DMAMODE0		(PLX_REGBASE + 0x80)
#define PLX_DMAPADR0		(PLX_REGBASE + 0x84)
#define PLX_DMALADR0		(PLX_REGBASE + 0x88)
#define PLX_DMASIZE0		(PLX_REGBASE + 0x8c)
#define PLX_DMADPR0			(PLX_REGBASE + 0x90)
#define PLX_DMAMODE1		(PLX_REGBASE + 0x94)
#define PLX_DMAPADR1		(PLX_REGBASE + 0x98)
#define PLX_DMALADR1		(PLX_REGBASE + 0x9c)
#define PLX_DMASIZE1		(PLX_REGBASE + 0xa0)
#define PLX_DMADPR1			(PLX_REGBASE + 0xa4)

#define PLX_DMA0CSR			(PLX_REGBASE + 0xa8)
#define PLX_DMA1CSR			(PLX_REGBASE + 0xa9)

#define DMA0_INT_ACTIVE		0x00200000
#define DMA1_INT_ACTIVE		0x00400000

UW pci9054_config_base;
UW pci9054_space0_base;

#define DMABUF_SIZE		2048

#define MAX_SPWPORT		3		/*8*/

typedef struct {
	W port; /* serial port number */
	PRI mypri; /* task priority */
	GDI gdi; /* general driver I/F handle */
	FastLock rdlock; /* for read port exclusive access */
	FastLock wrlock; /* for write port exclusive access */
	//	FastLock intc_lock;	/* for intc reg exclusive access */
	volatile ID mbf_rreq;
	volatile ID mbf_wreq;
	//	volatile ID req_flag;
	BOOL opened :1; /* TRUE if opened */
	BOOL suspended :1; /* TRUE with suspend */
	BOOL connected :1; /* TRUE if link connected */
	T_DEVREQ *devreq; /* executing request (request to be aborted) */
	/* attribute information */
	TMO snd_tmout; /* send time out (ms) */
	TMO rcv_tmout; /* receive time out (ms) */
	volatile ID int_flag;
	VP reg; /* FPGA register base */
	VP buf_pa; /* PLX local address of FPGA tx buffer (+ 0x800 if rx buffer) */
	VP txmem_la; /* m2p dma buffer address (virtual, non cache) */
	UW txmem_pa; /* m2p dma buffer address (physical, from pci) */
	VP rxmem_la; /* p2m dma buffer address (virtual, non cache) */
	UW rxmem_pa; /* p2m dma buffer address (physical, from pci) */
} SpWInfo;

SpWInfo spw_info[MAX_SPWPORT];

typedef struct {
	W num;
	T_DEVREQ *devreq;
} msg_t;

#define DEFAULT_STACKSIZE		8192
#define DEFAULT_PRIORITY		130
#define HIGHER_PRIORITY			100

UB name[] = "spwar"; /* used for name of lock */

/* prototype */
ER spwOpenFn(ID devid, UINT omode, GDI gdi);
ER spwCloseFn(ID devid, UINT option, GDI gdi);
ER spwAbortFn(T_DEVREQ *devreq, GDI gdi);
INT spwEventFn(INT evttyp, VP evtinf, GDI gdi);

const GDefDev gdef = { NULL, /* exinf */
"spwa", /* devnm */
3, /* maxreqq */
0, /* drvatr */
0, /* devatr */
0, /* nsub */
1, /* blksz */
spwOpenFn, /* open */
spwCloseFn, /* close */
spwAbortFn, /* abort */
spwEventFn, /* event */
};

/* �f�[�^�ԍ���` */
enum {
	DN_LINKC = -200, /* link control register �̑����f�[�^�ԍ� */
	DN_LINKS = -201, /* link status register �̑����f�[�^�ԍ� */
	DN_TIMIN = -202, /* timein(time-code���M) register �̑����f�[�^�ԍ�, �����݂̂� */
	DN_TIMOUT = -203, /* timeout(time-code��M) register �̑����f�[�^�ԍ�, �Ǐo���̂� */
	DN_INTC = -204, /* interrupt control register �̑����f�[�^�ԍ� */
	DN_SIGN = -205, /* fpga signature register �̑����f�[�^�ԍ�, �Ǐo���̂� */
	DN_TXTMO = -206, /* ���M�^�C���A�E�g(ms) */
	DN_RXTMO = -207
/* ��M�^�C���A�E�g(ms) */
};

UB ch0_time_code, ch1_time_code, ch2_time_code;
UB ch0_connected, ch1_connected, ch2_connected;

volatile ID fid_pci_int;
FastLock dma0_lock;
FastLock dma1_lock;

/* misc */
#define VR5701_PIO0		0xbe000940
#define CPU_LED0	0x00000001
#define CPU_LED1	0x00000002
#define CPU_LED2	0x00000004
#define CPU_LED3	0x00000008
#if 1
#define CPU_LED0_ON		out_w(VR5701_PIO0, in_w(VR5701_PIO0) & ~CPU_LED0)
#define CPU_LED1_ON		out_w(VR5701_PIO0, in_w(VR5701_PIO0) & ~CPU_LED1)
#define CPU_LED2_ON		out_w(VR5701_PIO0, in_w(VR5701_PIO0) & ~CPU_LED2)
#define CPU_LED3_ON		out_w(VR5701_PIO0, in_w(VR5701_PIO0) & ~CPU_LED3)
#define CPU_LED0_OFF	out_w(VR5701_PIO0, in_w(VR5701_PIO0) | CPU_LED0)
#define CPU_LED1_OFF	out_w(VR5701_PIO0, in_w(VR5701_PIO0) | CPU_LED1)
#define CPU_LED2_OFF	out_w(VR5701_PIO0, in_w(VR5701_PIO0) | CPU_LED2)
#define CPU_LED3_OFF	out_w(VR5701_PIO0, in_w(VR5701_PIO0) | CPU_LED3)
#else
#define CPU_LED0_ON
#define CPU_LED1_ON
#define CPU_LED2_ON
#define CPU_LED3_ON
#define CPU_LED0_OFF
#define CPU_LED1_OFF
#define CPU_LED2_OFF
#define CPU_LED3_OFF
#endif

Inline void spw_fpga_write(VP reg_base, UW reg_offs, UW data) {
	out_w((UW) reg_base + reg_offs, data);
}

Inline UW spw_fpga_read(VP reg_base, UW reg_offs) {
	return in_w((UW) reg_base + reg_offs);
}

int spw_send(SpWInfo *info, UW *buf, UW len, W *alen) {
	ER er = E_OK;
	UINT flg_pattern;
	UW intmask, status;
	W i, tx_bytes, rem_bytes;
	UW num, cmd;
	UW txbuf_pa;
	UW *p, *q;
	UINT intsts;

	txbuf_pa = (UW) info->buf_pa;
	rem_bytes = len;
	q = &buf[0];

	//printf("spw_send ch=%d\n", info->port);
	CPU_LED0_ON;

	while (rem_bytes > 0) {
		if (rem_bytes > DMABUF_SIZE) {
			tx_bytes = DMABUF_SIZE;
			rem_bytes = rem_bytes - DMABUF_SIZE;
			cmd = 0x00000fff & tx_bytes;
			cmd = cmd | 0x80000000;
		} else {
			tx_bytes = rem_bytes;
			rem_bytes = 0;
			cmd = 0x00000fff & tx_bytes;
			cmd = cmd | 0xc0000000;
		}
		//		printf("sending %d\n", tx_bytes);

		CPU_LED1_ON;
		status = spw_fpga_read(info->reg, TXCSR_OFFSET);
		//	printf("tx_csr status=%08x\n", status);
		if ((status & 0x80000000) == 0) {
			tk_clr_flg(info->int_flag, ~TXBUFFER_RDY);
			DI(intsts);
			intmask = spw_fpga_read(info->reg, INTC_OFFSET);
			intmask |= TXBUFFER_RDY;
			spw_fpga_write(info->reg, INTC_OFFSET, intmask); // enabel int, and wait...
			EI(intsts);
			//er = tk_wai_flg(info->int_flag, TXBUFFER_RDY, TWF_ORW | TWF_BITCLR, &flg_pattern, TMO_FEVR);
			er = tk_wai_flg(info->int_flag, TXBUFFER_RDY, TWF_ORW | TWF_BITCLR, &flg_pattern, info->snd_tmout);
			if (er != E_OK) {
				if (er != E_TMOUT)
					printf("spw_send : tk_wait_flg error\n");
				break;
			}
		}
		CPU_LED1_OFF;

		//Lock(&dma0_lock);

		p = (volatile UW *) info->txmem_la; // ���ł�cache off�ɂȂBĂ���͂�
		num = ((tx_bytes + 3) >> 2);
		for (i = 0; i < num; i++)
			*p++ = *q++;

		Lock(&dma0_lock);
		retry_tx_dma: num = ((tx_bytes + 3) & 0xfffffffc);
		out_w(PLX_DMADPR0, 0);
		out_w(PLX_DMAPADR0, info->txmem_pa);
		out_w(PLX_DMALADR0, txbuf_pa);
		out_w(PLX_DMASIZE0, num);
		out_b(PLX_DMA0CSR, 0x03);

		//er = tk_wai_flg(fid_pci_int, DMA0_INT_ACTIVE, TWF_ORW | TWF_BITCLR, &flg_pattern, TMO_FEVR);
		er = tk_wai_flg(fid_pci_int, DMA0_INT_ACTIVE, TWF_ORW | TWF_BITCLR, &flg_pattern, 100); // dma��1ms�ȓ�
		if (er != E_OK) {
			out_b(PLX_DMA0CSR, 0x04); // abort dma ?
			Unlock(&dma0_lock);
			printf("tx dma : tk_wait_flg error\n");
			goto retry_tx_dma;
			break;
		}
		//EnableInt(INTNO_EPCIA, 0);

		Unlock(&dma0_lock); //	printf("send done\n");

		CPU_LED3_ON;
		spw_fpga_write(info->reg, TXCSR_OFFSET, cmd);
		Asm("sync");
		CPU_LED3_OFF;
	}
	CPU_LED0_OFF;
	//CPU_LED1_ON;
	//printf("spw_send done ch=%d\n", info->port);

	return er;
}

// �߂�l:
// EOP�̂Ƃ�          T.B.D.
// EEP�̂Ƃ�          T.B.D.
// �p�P�b�g�r���̂Ƃ� T.B.D.
// �G���[��(DMA)      T.B.D.
int spw_receive(SpWInfo *info, UW *buf, UW len, W *alen) {
	ER er = E_OK;
	UINT flg_pattern;
	W i, num, rx_bytes, rem_bytes, recv_num;
	UW rxbuf_pa;
	UW *p, *q;
	UW status;
	UW intmask;
	UINT intsts;

	rxbuf_pa = (UW) info->buf_pa + RXBUF_OFFSET;
	recv_num = 0;
	p = buf;
	rem_bytes = len;

	for (;;) {
		//	printf("waiting... ch=%d\n", info->port);
		//	er = tk_wai_flg(info->int_flag, RXBUFFER_DONE, TWF_ORW | TWF_BITCLR, &flg_pattern, TMO_FEVR);
		er = tk_wai_flg(info->int_flag, RXBUFFER_DONE| FLG_ABORTED, TWF_ORW | TWF_BITCLR, &flg_pattern, info->rcv_tmout);
		if (er != E_OK) {
			// E_TMOUT���͊����݂��BĂ��Ȃ�
			break;
		}
		if (flg_pattern & FLG_ABORTED) {
			er = E_ABORT;
			EnableInt(INTNO_GPIO6, 0);
			break;
		}

		status = spw_fpga_read(info->reg, RXCSR_OFFSET);
		rx_bytes = status & 0x00000fff;
		if (rx_bytes > 2048) {
			// irrregal, broken
			*alen = 0;
			num = 0;
			er = -1;
			//return -1;
		} else {
			num = ((rx_bytes + 3) & 0xfffffffc);
		}

		if (num > 0) {
			Lock(&dma1_lock);

			out_w(PLX_DMADPR1, 0x00000008); // local=>pci
			out_w(PLX_DMAPADR1, info->rxmem_pa);
			out_w(PLX_DMALADR1, rxbuf_pa);
			out_w(PLX_DMASIZE1, num);
			out_b(PLX_DMA1CSR, 0x03);

			//er = tk_wai_flg(fid_pci_int, DMA1_INT_ACTIVE, TWF_ORW | TWF_BITCLR, &flg_pattern, TMO_FEVR);
			er = tk_wai_flg(fid_pci_int, DMA1_INT_ACTIVE, TWF_ORW | TWF_BITCLR, &flg_pattern, 100); // dma��1ms�ȓ�
			if (er != E_OK) {
				out_b(PLX_DMA1CSR, 0x04); // abort ?
				Unlock(&dma1_lock);
				printf("rx dma : tk_wait_flg error\n");
				break;
			}

			Unlock(&dma1_lock);

			if (rem_bytes < rx_bytes) {
				num = (rem_bytes >> 2);
				recv_num += (rem_bytes & 0xfffffffc);
				rem_bytes = 0;
			} else {
				num = ((rx_bytes + 3) >> 2);
				recv_num += rx_bytes;
				rem_bytes -= rx_bytes;
			}
			q = (volatile UW *) info->rxmem_la;
			for (i = 0; i < num; i++)
				*p++ = *q++;

			//Unlock(&dma1_lock);
		}
		// buffer change
		spw_fpga_write(info->reg, RXCSR_OFFSET, RXBUF_DONE);

		// enable rx int
		DI(intsts);
		intmask = spw_fpga_read(info->reg, INTC_OFFSET);
		intmask |= RXBUFFER_DONE;
		spw_fpga_write(info->reg, INTC_OFFSET, intmask);
		EI(intsts);

		CPU_LED2_OFF;

		if (status & RXBUF_EOP) {
			*alen = recv_num;
			break;
		} else if (status & RXBUF_EEP) {
			*alen = recv_num;
			er = E_MACV;
			break;
		} else if (rem_bytes == 0) {
			*alen = recv_num;
			er = E_MACV;
			break;
		}
		// buf������Ȃ��A�ǂݎc�����Ƃ��͂ǂ�����H


	}
	return er;
}

//ER readData(SpWInfo *info, T_DEVREQ *devreq)
void read_task(INT stacd, VP exinf) {
	ER err = E_OK;
	W req_cnt;
	UW req_data;
	SpWInfo *info;
	T_DEVREQ *devreq;
	msg_t msg;

	tk_slp_tsk(500);

	info = (SpWInfo *) exinf;

	for (;;) {

		//info = (SpWInfo *)exinf;
		err = tk_rcv_mbf(info->mbf_rreq, &msg, TMO_FEVR);
		if (err != sizeof(msg)) {
			printf("read task : msg size error\n");
			break;
		}

		//	printf(" read_req msg=%08x\n", msg.num);

		Lock(&info->rdlock);

		devreq = msg.devreq;

		err = tk_set_tsp(TSK_SELF, &devreq->tskspc);

		/* parameter check */
		if (devreq->size < 0) {
			err = E_PAR;
			goto err_ret;
		}

		err = E_OK;

		switch (devreq->start) {
		case 0:
			err = spw_receive(info, devreq->buf, devreq->size, &devreq->asize);
			break;
		case DN_LINKC:
			req_cnt = devreq->size;
			if (req_cnt == 4) {
				req_data = spw_fpga_read(info->reg, LNKC_OFFSET);
				memcpy(devreq->buf, &req_data, 4);
				devreq->asize = 4;
			} else {
				err = E_PAR;
			}
			break;
		case DN_LINKS:
			req_cnt = devreq->size;
			if (req_cnt == 4) {
				req_data = spw_fpga_read(info->reg, LNKS_OFFSET);
				memcpy(devreq->buf, &req_data, 4);
				devreq->asize = 4;
			} else {
				err = E_PAR;
			}
			break;
		case DN_TIMOUT:
			req_cnt = devreq->size;
			if (req_cnt == 4) {
				req_data = spw_fpga_read(info->reg, TIMOUT_OFFSET);
				memcpy(devreq->buf, &req_data, 4);
				devreq->asize = 4;
			} else {
				err = E_PAR;
			}
			break;
		case DN_INTC:
			req_cnt = devreq->size;
			if (req_cnt == 4) {
				req_data = spw_fpga_read(info->reg, INTC_OFFSET);
				memcpy(devreq->buf, &req_data, 4);
				devreq->asize = 4;
			} else {
				err = E_PAR;
			}
			break;
		case DN_SIGN:
			req_cnt = devreq->size;
			if (req_cnt == 4) {
				req_data = spw_fpga_read(info->reg, SIGN_OFFSET);
				memcpy(devreq->buf, &req_data, 4);
				devreq->asize = 4;
			} else {
				err = E_PAR;
			}
			break;
		case DN_TXTMO:
			req_cnt = devreq->size;
			if (req_cnt == 4) {
				req_data = info->snd_tmout;
				memcpy(devreq->buf, &req_data, 4);
				devreq->asize = 4;
			} else {
				err = E_PAR;
			}
			break;
		case DN_RXTMO:
			req_cnt = devreq->size;
			if (req_cnt == 4) {
				req_data = info->rcv_tmout;
				memcpy(devreq->buf, &req_data, 4);
				devreq->asize = 4;
			} else {
				err = E_PAR;
			}
			break;
		default:
			err = E_PAR;
		}
		if (err < E_OK) {
			//printf("readData err = %d\n", err);
		}

		err_ret: devreq->error = err;
		GDI_Reply(devreq, info->gdi);

		Unlock(&info->rdlock);

	}
}

//ER writeData(SpWInfo *info, T_DEVREQ *devreq)
void write_task(INT stacd, VP exinf) {
	ER err = E_OK;
	W req_cnt;
	UW *req_data;
	SpWInfo *info;
	T_DEVREQ *devreq;
	msg_t msg;

	tk_slp_tsk(500);

	info = (SpWInfo *) exinf;

	for (;;) {

		//info = (SpWInfo *)exinf;

		err = tk_rcv_mbf(info->mbf_wreq, &msg, TMO_FEVR);
		if (err != sizeof(msg)) {
			printf("write task : msg size error\n");
			break;
		}
		//	printf(" write_req msg=%08x\n", msg.num);

		Lock(&info->wrlock);

		devreq = msg.devreq;

		err = tk_set_tsp(TSK_SELF, &devreq->tskspc);

		/* parameter check */
		if (devreq->size < 0) {
			err = E_PAR;
			goto err_ret;
		}

		err = E_OK;

		switch (devreq->start) {
		case 0:
			err = spw_send(info, devreq->buf, devreq->size, &devreq->asize);
			break;
		case DN_LINKC:
			req_cnt = devreq->size;
			req_data = devreq->buf;
			if (req_cnt == 4) {
				spw_fpga_write(info->reg, LNKC_OFFSET, *req_data);
				devreq->asize = 4;
			} else {
				err = E_PAR;
			}
			break;
		case DN_LINKS:
			req_cnt = devreq->size;
			req_data = devreq->buf;
			if (req_cnt == 4) {
				spw_fpga_write(info->reg, LNKS_OFFSET, *req_data);
				devreq->asize = 4;
			} else {
				err = E_PAR;
			}
			break;
		case DN_TIMIN:
			req_cnt = devreq->size;
			req_data = devreq->buf;
			if (req_cnt == 4) {
				spw_fpga_write(info->reg, TIMIN_OFFSET, *req_data);
				devreq->asize = 4;
			} else {
				err = E_PAR;
			}
			break;
		case DN_INTC:
			req_cnt = devreq->size;
			req_data = devreq->buf;
			if (req_cnt == 4) {
				spw_fpga_write(info->reg, INTC_OFFSET, *req_data);
				devreq->asize = 4;
			} else {
				err = E_PAR;
			}
			break;
		case DN_SIGN:
			req_cnt = devreq->size;
			req_data = devreq->buf;
			if (req_cnt == 4) {
				spw_fpga_write(info->reg, SIGN_OFFSET, *req_data);
				devreq->asize = 4;
			} else {
				err = E_PAR;
			}
			break;
		case DN_TXTMO:
			req_cnt = devreq->size;
			req_data = devreq->buf;
			if (req_cnt == 4) {
				info->snd_tmout = *req_data;
				devreq->asize = 4;
				printf("TX TimeOut : %d ms\n",info->snd_tmout);
			} else {
				err = E_PAR;
			}
			break;
		case DN_RXTMO:
			req_cnt = devreq->size;
			req_data = devreq->buf;
			if (req_cnt == 4) {
				info->rcv_tmout = *req_data;
				devreq->asize = 4;
				printf("RX TimeOut : %d ms\n",info->rcv_tmout);
			} else {
				err = E_PAR;
			}
			break;
		default:
			err = E_PAR;
		}

		if (err < E_OK)
			goto err_ret;

		err_ret: devreq->error = err;
		GDI_Reply(devreq, info->gdi);

		Unlock(&info->wrlock);

	}
}

ER spwOpenFn(ID devid, UINT omode, GDI gdi) {
	ER err = E_OK;
	SpWInfo *info = GDI_exinf(gdi);
	volatile UW reg;

	if (info->opened)
		return err;

	printf("spw%d opened\n", info->port);
	reg = spw_fpga_read(info->reg, LNKS_OFFSET); /* read status(clear) */
	spw_fpga_write(info->reg, INTC_OFFSET, 0xf); /* enable all int */
	//	spw_fpga_write(info->port, LNKC_OFFSET, 0);

	info->opened = TRUE;

	return err;
}

ER spwCloseFn(ID devid, UINT option, GDI gdi) {
	ER err = E_OK;
	SpWInfo *info = GDI_exinf(gdi);
	volatile UW reg;

	if (info->opened) {
		//printf("spw%d closed\n", info->port);
		//reg = spw_fpga_read(info->reg, LNKS_OFFSET);	/* read status */
		//spw_fpga_write(info->reg, INTC_OFFSET, 0);
		info->opened = FALSE;
	}
	return err;
}

ER spwAbortFn(T_DEVREQ *devreq, GDI gdi) {
	ER err = E_OK;
	SpWInfo *info = GDI_exinf(gdi);

	//printf("spwAbortFn\n");

	tk_set_flg(info->int_flag, FLG_ABORTED);

	return err;
}

INT spwEventFn(INT evttyp, VP evtinf, GDI gdi) {
	SpWInfo *info = GDI_exinf(gdi);

	switch (evttyp) {
	case TDV_SUSPEND:
	case TDV_RESUME:
	default: /* undefined event */
		printf("spwEventFn evttyp=%d\n", evttyp);
		return E_PAR;
	}
}

ER spwAcceptDeviceRequest(SpWInfo *info) {
	T_DEVREQ *devreq;
	ER err;
	msg_t msg;

	for (;;) { /* never terminate by itself */

		/* accept request */
		err = GDI_Accept(&devreq, DRP_NORMREQ, TMO_FEVR, info->gdi);
		if (err < E_OK) {
			printf("GDI_Accept er = %d\n", err);
			continue;
		}
		/* task space setting */
		err = tk_set_tsp(TSK_SELF, &devreq->tskspc);
		if (err < E_OK) {
			printf("tk_set_tsp er = %d\n", err);
			continue;
		}

		/* record that handling to be aborted is in execution */
		info->devreq = devreq; //����͉��H


		/* execute handling */
		if (devreq->cmd == TDC_WRITE) {
			msg.num = info->port;
			msg.devreq = devreq;
			err = tk_snd_mbf(info->mbf_wreq, &msg, sizeof(msg_t), TMO_FEVR);
			if (err != E_OK) {
				printf("tk_snd_mbf er = %d\n", err);
				continue;
			}
		} else {
			msg.num = info->port;
			msg.devreq = devreq;
			err = tk_snd_mbf(info->mbf_rreq, &msg, sizeof(msg_t), TMO_FEVR);
			if (err != E_OK) {
				printf("tk_snd_mbf er = %d\n", err);
				continue;
			}
		}

		/* record that handling to be aborted is finished */
		//info->devreq = NULL;
	}

}

/* interrupt handler */

void gpio6_int(UINT dintno) {
	UH status;
	UH intmask;
	UW reg;

	DisableInt(INTNO_GPIO6);
	//ClearInt(dintno);

	status = in_w(INTS0_REG);

	if (status & TXBUFFER_RDY) {
		intmask = in_w(INTC0_REG);
		intmask &= ~TXBUFFER_RDY;
		out_w(INTC0_REG, intmask); // disable
		tk_set_flg(spw_info[0].int_flag, TXBUFFER_RDY);
	}

	if (status & RXBUFFER_DONE) {
		intmask = in_w(INTC0_REG);
		intmask &= ~RXBUFFER_DONE;
		out_w(INTC0_REG, intmask); // disable
		tk_set_flg(spw_info[0].int_flag, RXBUFFER_DONE);
		//CPU_LED1_OFF;
		CPU_LED2_ON;
	}

	if (status & TCODE_RECVED) {
		CPU_LED2_ON;
		//	intmask = in_w(INTC0_REG);
		//	intmask &= ~TCODE_RECVED;
		//	out_w(INTC0_REG, intmask);		// disable
		ch0_time_code = (UB) in_w(TIMOUT0_REG); // clear bit
		tk_set_flg(spw_info[0].int_flag, TCODE_RECVED);
	}

	if (status & LINK_CHANGE) {
		CPU_LED0_ON;
		intmask = in_w(INTC0_REG);
		intmask &= ~LINK_CHANGE;
		out_w(INTC0_REG, intmask); // disable
		tk_set_flg(spw_info[0].int_flag, LINK_CHANGE);
		out_w(INTS0_REG, LINK_CHANGE); // clear bit
	}

	EnableInt(INTNO_GPIO6, 0);
}

void gpio7_int(UINT dintno) {
	UW status;
	UW intmask;

	DisableInt(INTNO_GPIO7);

	status = in_w(INTS1_REG);

	if (status & TXBUFFER_RDY) {
		intmask = in_w(INTC1_REG);
		intmask &= ~TXBUFFER_RDY; // disable
		out_w(INTC1_REG, intmask);
		tk_set_flg(spw_info[1].int_flag, TXBUFFER_RDY);
	}

	if (status & RXBUFFER_DONE) {
		intmask = in_w(INTC1_REG);
		intmask &= ~RXBUFFER_DONE;
		out_w(INTC1_REG, intmask); // disable
		tk_set_flg(spw_info[1].int_flag, RXBUFFER_DONE);
	}

	if (status & TCODE_RECVED) {
		//	intmask = in_w(INTC1_REG);
		//	intmask &= ~TCODE_RECVED;
		//	out_w(INTC1_REG, intmask);		// disable
		ch1_time_code = (UB) in_w(TIMOUT1_REG); // clear bit
		tk_set_flg(spw_info[1].int_flag, TCODE_RECVED);
	}

	if (status & LINK_CHANGE) {
		intmask = in_w(INTC1_REG);
		intmask &= ~LINK_CHANGE;
		out_w(INTC1_REG, intmask); // disable
		tk_set_flg(spw_info[1].int_flag, LINK_CHANGE);
		out_w(INTS1_REG, LINK_CHANGE); // clear bit
	}

	EnableInt(INTNO_GPIO7, 0);
}

void gpio8_int(UINT dintno) {
	UW status;
	UW intmask;

	DisableInt(INTNO_GPIO8);

	status = in_w(INTS2_REG);

	if (status & TXBUFFER_RDY) {
		intmask = in_w(INTC2_REG);
		intmask &= ~TXBUFFER_RDY; // disable
		out_w(INTC2_REG,intmask);
		tk_set_flg(spw_info[2].int_flag, TXBUFFER_RDY);
	}

	if (status & RXBUFFER_DONE) {
		intmask = in_w(INTC2_REG);
		intmask &= ~RXBUFFER_DONE;
		out_w(INTC2_REG, intmask); // disable
		tk_set_flg(spw_info[2].int_flag, RXBUFFER_DONE);
	}

	if (status & TCODE_RECVED) {
		//	intmask = in_w(INTC2_REG);
		//	intmask &= ~TCODE_RECVED;
		//	out_w(INTC2_REG, intmask);		// disable
		ch2_time_code = (UB) in_w(TIMOUT2_REG); // clear bit
		tk_set_flg(spw_info[2].int_flag, TCODE_RECVED);
	}

	if (status & LINK_CHANGE) {
		intmask = in_w(INTC2_REG);
		intmask &= ~LINK_CHANGE;
		out_w(INTC2_REG, intmask); // disable
		tk_set_flg(spw_info[2].int_flag, LINK_CHANGE);
		out_w(INTS2_REG, LINK_CHANGE); // clear bit
	}

	EnableInt(INTNO_GPIO8, 0);
}

void epcia_int(UINT dintno) {
	UW status;
	UB dma_status;

	DisableInt(INTNO_EPCIA);
	status = in_w(PLX_INTCSR);

	if (status & DMA0_INT_ACTIVE) {
		dma_status = in_b(PLX_DMA0CSR);
		if (dma_status & 0x10) {
			out_b(PLX_DMA0CSR, status | 0x08);
			tk_set_flg(fid_pci_int, DMA0_INT_ACTIVE);
		}
		//printf("ch0 int %08x\n", dma_status);
	}

	if (status & DMA1_INT_ACTIVE) {
		dma_status = in_b(PLX_DMA1CSR);
		if (dma_status & 0x10) {
			out_b(PLX_DMA1CSR, status | 0x08);
			tk_set_flg(fid_pci_int, DMA1_INT_ACTIVE);
		}
		//printf("ch0 int %08x\n", dma_status);
	}

	//printf("apci int %08x\n", status);

	EnableInt(INTNO_EPCIA, 0);
}

/*
 * register/delete device and PC card client
 *	register	reg = TRUE
 *	delete	reg = FALSE
 */
INT regist_device(SpWInfo *info, BOOL reg) {
	GDefDev devdef;
	ER err;

	/* register information */
	devdef = gdef;
	devdef.exinf = info;
	devdef.devnm[3] += info->port; /* 0:spwa, 1:spwb, 2:spwc ... */

	/* define/undefine device */
	if (reg) {
		err = GDefDevice(&devdef, NULL, &info->gdi);
	} else {
		err = GDelDevice(info->gdi);
	}
	if (err < E_OK)
		goto err_ret;

	return E_OK;

	err_ret: printf("regist_device err = %d(port = %d)\n", err, info->port);
	return err;
}

void spacewire_driver_main(INT stacd, VP exinf) {
	ER err = 0;
	SpWInfo *info;

	info = (SpWInfo *) exinf;
	//printf("port = %d\n", info->port);
	//printf("pri = %d\n", info->mypri);

	name[3] = '0' + info->port; /* name = "spw0r", "spw1r" ... */

	tk_slp_tsk(500);

	/* create lock */
	name[4] = 'r';
	err = CreateLock(&info->rdlock, name);
	if (err < E_OK)
		goto err_ret0;
	name[4] = 'w';
	err = CreateLock(&info->wrlock, name);
	if (err < E_OK)
		goto err_ret0;
	//	err = CreateLock(&info->intc_lock, "intcl");
	//	if (err < E_OK) goto err_ret0;

	/* define device */
	err = regist_device(info, TRUE);
	if (err < E_OK)
		goto err_ret1;

	/* start receiving device request */
	err = spwAcceptDeviceRequest(info);
	if (err < E_OK)
		goto err_ret2;

	/* There is no case to execute here. */

	/* termination handling */
	err_ret2: regist_device(info, FALSE);
	err_ret1: DeleteLock(&info->wrlock);
	DeleteLock(&info->rdlock);
	err_ret0: printf("spacewire_driver_main err = %d(port = %d)\n", err, info->port);
	tk_exd_tsk();
}

void spacewire_link_mon(INT stacd, VP exinf) {
	ER er = 0;
	UINT flg_pattern;
	SpWInfo *info;
	UW link_status;
	UW intmask;
	UINT intsts;

	info = (SpWInfo *) exinf;

	while (1) {
		er = tk_wai_flg(info->int_flag, LINK_CHANGE, TWF_ORW | TWF_BITCLR, &flg_pattern, TMO_FEVR);
		if (er != E_OK) {
			printf("link_mon task : tk_wait_flg error\n");
		}CPU_LED0_OFF;
		tk_slp_tsk(50);
		link_status = spw_fpga_read(info->reg, LNKS_OFFSET);

		if ((link_status & 0x0000003f) == 0x0000003f) {
			if (info->port == 0) {
				ch0_connected = 1;
				printf("link0 on (0x%08x)\n", link_status);
			} else if (info->port == 1) {
				ch1_connected = 1;
				printf("link1 on (0x%08x)\n", link_status);
			} else {
				ch2_connected = 1;
				printf("link2 on (0x%08x)\n", link_status);
			}
		} else {
			if (info->port == 0) {
				ch0_connected = 0;
				printf("link0 off (0x%08x)\n", link_status);
			} else if (info->port == 1) {
				ch1_connected = 0;
				printf("link1 off (0x%08x)\n", link_status);
			} else {
				ch2_connected = 0;
				printf("link2 off (0x%08x)\n", link_status);
			}
		}
		//	Lock(&info->intc_lock);
		DI(intsts);
		intmask = spw_fpga_read(info->reg, INTC_OFFSET);
		intmask |= LINK_CHANGE;
		spw_fpga_write(info->reg, INTC_OFFSET, intmask); // enable
		EI(intsts);
		//	Unlock(&info->intc_lock);
	}
}

void spacewire_time_mon(INT stacd, VP exinf) {
	ER er = 0;
	UINT flg_pattern;
	SpWInfo *info;
	UW intmask;

	info = (SpWInfo *) exinf;

	while (1) {
		er = tk_wai_flg(info->int_flag, TCODE_RECVED, TWF_ORW | TWF_BITCLR, &flg_pattern, TMO_FEVR);
		if (er != E_OK) {
			printf("time_mon task : tk_wait_flg error\n");
		}
		CPU_LED2_OFF;
		printf("timecode : 0x%02x, 0x%02x, 0x%02x\n", ch0_time_code, ch1_time_code, ch2_time_code);

		//		Lock(&info->intc_lock);
		//		intmask = spw_fpga_read(info->reg, INTC_OFFSET);
		//		intmask |= TCODE_RECVED;
		//		spw_fpga_write(info->reg, INTC_OFFSET, intmask);		// enable
		//		Unlock(&info->intc_lock);
	}
}

int start_spacewire_driver(W port, PRI pri) {
	T_CTSK ct_spw;
	T_CFLG cf_spw_int;
	T_CMBF cmbf;
	SpWInfo *p_spw;
	ID tid_main, tid_rd, tid_wr, tid_lm, tid_tm;
	ER err = E_OK;
	UW base;
	INT size;
	VP la, pa;
	UW pcia;

	if (port >= MAX_SPWPORT) {
		err = E_LIMIT;
		goto err_ret0;
	}
	p_spw = &spw_info[port];
	memset(p_spw, 0, sizeof(SpWInfo));

	/* setup tx dma buffer */
	la = Kmalloc(4096); // 2048�ő��v?
	//printf(" ch%d txdma_buf(cpu la)=0x%08x\n", port, (UW)la);
	if (la == 0)
		goto err_ret0;
	size = CnvPhysicalAddr(la, DMABUF_SIZE, &pa);
	if (size != DMABUF_SIZE) {
		printf("error : tx_buf_addr(cpu pa)=0x%08x, size=0x%08x\n", pa, size);
		goto err_ret1;
	}
	pcia = cpuAddrToPciMem((UW) pa); //printf(" ch%d txdma_buf(pci pa)=0x%08x\n", port, pcia);
	p_spw->txmem_la = la;
	p_spw->txmem_pa = pcia;

	/* setup tx dma buffer */
	la = Kmalloc(4096); //printf(" ch%d rxdma_buf(cpu la)=0x%08x\n", port, (UW)la);
	if (la == 0)
		goto err_ret1;
	size = CnvPhysicalAddr(la, DMABUF_SIZE, &pa);
	if (size != DMABUF_SIZE) {
		printf("error : rx_buf_addr(cpu pa)=0x%08x, size=0x%08x\n", pa, size);
		goto err_ret2;
	}
	pcia = cpuAddrToPciMem((UW) pa); //printf(" ch%d rxdma_buf(pci pa)=0x%08x\n", port, pcia);
	p_spw->rxmem_la = la;
	p_spw->rxmem_pa = pcia;

	p_spw->port = port;
	p_spw->mypri = pri;
	p_spw->snd_tmout = TMO_FEVR;
	p_spw->rcv_tmout = TMO_FEVR;
	p_spw->opened = FALSE;

	base = FPGA_BASE + REG_OFFSET + port * PORT_OFFSET;
	p_spw->reg = (VP) base;

	base = port * PORT_OFFSET;
	p_spw->buf_pa = (VP) base;

	cf_spw_int.exinf = NULL;
	cf_spw_int.flgatr = TA_TFIFO | TA_WMUL;
	cf_spw_int.iflgptn = 0;
	p_spw->int_flag = tk_cre_flg(&cf_spw_int); /* GPIO6-8 int flag */

	/* read req message box */
	cmbf.exinf = NULL;
	cmbf.mbfatr = TA_TFIFO;
	cmbf.bufsz = sizeof(msg_t) * 32;
	cmbf.maxmsz = sizeof(msg_t);
	err = tk_cre_mbf(&cmbf);
	if (err < E_OK)
		goto err_ret3;
	p_spw->mbf_rreq = err;

	/* write req message box */
	cmbf.exinf = NULL;
	cmbf.mbfatr = TA_TFIFO;
	cmbf.bufsz = sizeof(msg_t) * 32;
	cmbf.maxmsz = sizeof(msg_t);
	err = tk_cre_mbf(&cmbf);
	if (err < E_OK)
		goto err_ret4;
	p_spw->mbf_wreq = err;

	/* driver main task */
	ct_spw.exinf = p_spw;
	ct_spw.tskatr = TA_HLNG | TA_RNG0;
	ct_spw.task = spacewire_driver_main;
	ct_spw.itskpri = pri;
	ct_spw.stksz = DEFAULT_STACKSIZE;
	err = tk_cre_tsk(&ct_spw);
	if (err < E_OK)
		goto err_ret4;
	tid_main = (ID) err;
	err = tk_sta_tsk(tid_main, (INT) p_spw);
	if (err < E_OK)
		goto err_ret5;

	/* read task */
	ct_spw.exinf = p_spw;
	ct_spw.tskatr = TA_HLNG | TA_RNG0;
	ct_spw.task = read_task;
	ct_spw.itskpri = HIGHER_PRIORITY;
	ct_spw.stksz = DEFAULT_STACKSIZE;
	err = tk_cre_tsk(&ct_spw);
	if (err < E_OK)
		goto err_ret5;
	tid_rd = (ID) err;
	err = tk_sta_tsk(tid_rd, (INT) p_spw);
	if (err < E_OK)
		goto err_ret6;

	/* write task */
	ct_spw.exinf = p_spw;
	ct_spw.tskatr = TA_HLNG | TA_RNG0;
	ct_spw.task = write_task;
	ct_spw.itskpri = HIGHER_PRIORITY;
	ct_spw.stksz = DEFAULT_STACKSIZE;
	err = tk_cre_tsk(&ct_spw);
	if (err < E_OK)
		goto err_ret6;
	tid_wr = (ID) err;
	err = tk_sta_tsk(tid_wr, (INT) p_spw);
	if (err < E_OK)
		goto err_ret7;

	/* link monitor task */
	ct_spw.exinf = p_spw;
	ct_spw.tskatr = TA_HLNG | TA_RNG0;
	ct_spw.task = spacewire_link_mon;
	ct_spw.itskpri = pri;
	ct_spw.stksz = DEFAULT_STACKSIZE;
	err = tk_cre_tsk(&ct_spw);
	if (err < E_OK)
		goto err_ret7;
	tid_lm = (ID) err;
	err = tk_sta_tsk(tid_lm, 0);
	if (err < E_OK)
		goto err_ret8;

	/* time monitor task */
	ct_spw.exinf = p_spw;
	ct_spw.tskatr = TA_HLNG | TA_RNG0;
	ct_spw.task = spacewire_time_mon;
	ct_spw.itskpri = HIGHER_PRIORITY;
	ct_spw.stksz = DEFAULT_STACKSIZE;
	err = tk_cre_tsk(&ct_spw);
	if (err < E_OK)
		goto err_ret8;
	tid_tm = (ID) err;
	err = tk_sta_tsk(tid_tm, 0);
	if (err < E_OK)
		goto err_ret9;

	return err;

	err_ret9: tk_del_tsk(tid_tm);
	err_ret8: tk_del_tsk(tid_lm);
	err_ret7: tk_del_tsk(tid_wr);
	err_ret6: tk_del_tsk(tid_rd);
	err_ret5: tk_del_tsk(tid_main);
	err_ret4: tk_del_mbf(p_spw->mbf_wreq);
	err_ret3: tk_del_mbf(p_spw->mbf_rreq);
	err_ret2: Kfree(p_spw->rxmem_la);
	err_ret1: Kfree(p_spw->txmem_la);
	printf("start error2\n");
	err_ret0: printf("start error1\n");

	return err;
}

ER main(INT ac, UB *av[]) {
	ER er = E_OK;
	W caddr;
	W size;
	VP addr;

	T_CFLG cf_pci_int;
	T_DINT di_spw_int;

	if (ac < 0) { /* �A�����[�h�� */
		return E_NOSPT; /* �f�o�C�X���B̓T�|�[�g���Ȃ� */
	}
	printf("spw test driver ver.0.0 build %s %s\n", __DATE__, __TIME__);

	caddr = searchPciDev(0x10b5, 0x9054); //printf(" caddr=0x%08x\n", caddr);
			getPciBaseAddr(caddr, PCR_BASEADDR_0, &addr, &size);
			pci9054_config_base = pciMemToCpuAddr((unsigned long)addr); //printf(" bar0=0x%08x, size=0x%08x\n", (unsigned long)addr, size);
			getPciBaseAddr(caddr, PCR_BASEADDR_1, &addr, &size); //printf(" bar1=0x%08x, size=0x%08x\n", (unsigned long)addr, size);
			getPciBaseAddr(caddr, PCR_BASEADDR_2, &addr, &size); //printf(" bar2=0x%08x, size=0x%08x\n", (unsigned long)addr, size);
			pci9054_space0_base = pciMemToCpuAddr((unsigned long)addr);
			//printf(" pci9054 config reg base=0x%08x\n", pci9054_config_base);
			//printf(" pci9054 local mem base=0x%08x\n", pci9054_space0_base);


			out_w(0xbe000600,0x01000000);
			out_w(0xbe000710,0xa0000000);

			/* setup plx pci9054 */
			out_w(PLX_LBRD0, 0x41430043);
			out_w(PLX_LAS0BA, 0x00000001);
			out_w(PLX_INTCSR, 0x000d0100);

			out_w(PLX_INTCSR, 0x000c0100); // PCI int enable, DMA CH0/1 int enable

			out_w(PLX_DMAMODE0, 0x00020543);
			out_w(PLX_DMAMODE1, 0x00020543);

			out_w(PLX_DMADPR0, 0); // pci=>local (dma ch0 dedicated to tx)
			out_w(PLX_DMADPR1, 0x00000008); // local=>pci (dma ch1 dedicated to rx)


			/* event flag */
			cf_pci_int.exinf = NULL;
			cf_pci_int.flgatr = TA_TFIFO | TA_WMUL;
			cf_pci_int.iflgptn = 0;
			fid_pci_int = tk_cre_flg(&cf_pci_int); /* EPCIA int flag */

			er = CreateLock(&dma0_lock, "dma0"); // �eport��������dma0(tx)��g�p���Ȃ��悤��
			er = CreateLock(&dma1_lock, "dma1"); // �eport��������dma1(rx)��g�p���Ȃ��悤��


			/* register interrupt handler */
			di_spw_int.intatr = TA_HLNG;
			di_spw_int.inthdr = gpio6_int;
			tk_def_int(INTNO_GPIO6, &di_spw_int); /* GPIO6 int handler */

			di_spw_int.intatr = TA_HLNG;
			di_spw_int.inthdr = gpio7_int;
			tk_def_int(INTNO_GPIO7, &di_spw_int); /* GPIO7 int handler */

			di_spw_int.intatr = TA_HLNG;
			di_spw_int.inthdr = gpio8_int;
			tk_def_int(INTNO_GPIO8, &di_spw_int); /* GPIO8 int handler */

			di_spw_int.intatr = TA_HLNG;
			di_spw_int.inthdr = epcia_int;
			tk_def_int(INTNO_EPCIA, &di_spw_int); /* EPCIA int handler */

			SetIntMode(INTNO_GPIO6, IM_LEVEL | IM_LOW);
			SetIntMode(INTNO_GPIO7, IM_LEVEL | IM_LOW);
			SetIntMode(INTNO_GPIO8, IM_LEVEL | IM_LOW);
			SetIntMode(INTNO_EPCIA, IM_LEVEL | IM_LOW);

			ClearInt(INTNO_GPIO6);
			ClearInt(INTNO_GPIO7);
			ClearInt(INTNO_GPIO8);
			ClearInt(INTNO_EPCIA);

			EnableInt(INTNO_GPIO6, 0);
			EnableInt(INTNO_GPIO7, 0);
			EnableInt(INTNO_GPIO8, 0);
			EnableInt(INTNO_EPCIA, 0);

			er = start_spacewire_driver(0, DEFAULT_PRIORITY); /* �f�o�C�X�o�^ */
			er = start_spacewire_driver(1, DEFAULT_PRIORITY); /* �f�o�C�X�o�^ */
			er = start_spacewire_driver(2, DEFAULT_PRIORITY); /* �f�o�C�X�o�^ */

			return ((er >= E_OK) ? E_OK : er);
		}

		/************************************************************************
		 ������:

		 ��m�s�:

		 TODO����:
		 �C�x���g����&�����N���,TimeCode�Ȃǂ̃C�x���g��v���Z�X�ɔ��s

		 *************************************************************************/

		//�ύX2008/8/18
		//  ���W�X�^�A�h���X��PnP���𗘗p���Čv�Z����悤�ɕύX
		//  INTC���W�X�^�A�N�Z�X��Lock/Unlock�ŕی�
		//  �p�P�b�g(�o�b�t�@)��READ/WRITE�Ƀ^�C���A�E�g�ݒ��ǉ�
		//  TimeCode�A�����N�C�x���g���o�^�X�N��ǉ�(���g�p)
		//  �R�[�h����

