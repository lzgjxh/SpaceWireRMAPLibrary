/*
 * SpaceWire I/F
 */

/* 2008-10-15
 *  2080-09-11版の田苗さんのルータドライバ(spwr.c)をもとに、湯浅が
 *  本郷で作業した結果と、2008-10-10の田苗さんの改修版をマージしたバージョン
 */

#include <tk/tkernel.h>
#include <tk/util.h>
#include <device/gdrvif.h>
#include <device/pci.h>
#include <string.h>

#include <stdio.h>

/* $BD/include/tk/vr/intc_vr5701.h */
#define INTNO_GPIO6		IV_GPIO(6)		/* int ch0 */
#define INTNO_GPIO7		IV_GPIO(7)		/* not-used */
#define INTNO_GPIO8		IV_GPIO(8)		/* not-used */
#define INTNO_GPIO9		IV_GPIO(9)		/* not-used */
#define INTNO_GPIO10	IV_GPIO(10)		/* not-used */
#define INTNO_GPIO11	IV_GPIO(11)		/* not-used */
#define INTNO_EPCIA		IV_EPCI(0)		/* inta# */


/* fpga registers */
#define FPGA_BASE		pci9054_space0_base

#define INTC_REG		(FPGA_BASE + 0xff00)
#define INTS_REG		(FPGA_BASE + 0xff04)
#define TCOUT_REG		(FPGA_BASE + 0xff08)
#define TCIN_REG		(FPGA_BASE + 0xff0c)
#define SIGN_REG		(FPGA_BASE + 0xfffc)

#define PORT0_CSR		(FPGA_BASE + 0x0000)
#define PORT1_CSR		(FPGA_BASE + 0x0004)
#define PORT2_CSR		(FPGA_BASE + 0x0008)
#define PORT3_CSR		(FPGA_BASE + 0x000c)
#define PORT4_CSR		(FPGA_BASE + 0x0010)
#define PORT5_CSR		(FPGA_BASE + 0x0014)
#define PORT6_CSR		(FPGA_BASE + 0x001c)

#define RT_CONFIG		(FPGA_BASE + 0x0400)
#define RT_SIGN			(FPGA_BASE + 0x0414)
#define TCODEEN_REG		(FPGA_BASE + 0x041c)
#define PORT0_KEY		(FPGA_BASE + 0x0424)

#define ADRTBL_BASE		FPGA_BASE + 0x0


/* flag */
#define TXBUF_EOP		0x40000000
#define TXBUF_EEP		0x20000000
#define TXBUF_START		0x80000000

#define RXBUF_EOP		0x40000000
#define RXBUF_EEP		0x20000000
#define RXBUF_DONE		0x80000000

#define BUFFER_OFFSET		0x8000
#define BUFFER_LENGTH		0x1000
#define RXBUF_OFFSET		0x800
#define BUFCSR_OFFSET		0xf000
#define BUFCSR_LENGTH		0x10

#define TXCSR_OFFSET		0x0
#define RXCSR_OFFSET		0x4

#define PORT0_STAT		(FPGA_BASE + 0x0000)
#define PORT0_CTRL		(FPGA_BASE + 0x0002)
#define PORT1_STAT		(FPGA_BASE + 0x0004)
#define PORT1_CTRL		(FPGA_BASE + 0x0006)
#define PORT2_STAT		(FPGA_BASE + 0x0008)
#define PORT2_CTRL		(FPGA_BASE + 0x000a)
#define PORT3_STAT		(FPGA_BASE + 0x000c)
#define PORT3_CTRL		(FPGA_BASE + 0x000e)


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


#define DMABUF_SIZE		2048

#define SPW_PORT_NUM		3		/*8*/

#define MAX_PORTNUM			3
#define MIN_PORTNUM			1


typedef struct {
	W port;				/* serial port number */
	PRI mypri;			/* task priority */
	GDI gdi;			/* general driver I/F handle */
	FastLock rdlock;	/* for read port exclusive access */
	FastLock wrlock;	/* for write port exclusive access */
	volatile ID mbf_rreq;
	volatile ID mbf_wreq;
	W open_num;
	/*BOOL opened:1;		/* TRUE if opened */
	/*BOOL suspended:1;	/* TRUE with suspend */
	/*BOOL connected:1;	/* TRUE if link connected */
	T_DEVREQ *devreq;	/* executing request (request to be aborted) */
	/* attribute information */
	TMO	snd_tmout;		/* send time out (ms) */
	TMO	rcv_tmout;		/* receive time out (ms) */
	volatile ID int_flag;	/* buffer interrupts are flagged for each driver */
	W tbl_index;		/* routing table index : addr=4*index */
	VP reg;				/* FPGA register base */
	VP buf_pa;			/* PLX local address of FPGA tx buffer (+ 0x800 if rx buffer) */
	VP txmem_la;		/* m2p dma buffer address (virtual, non cache) */
	UW txmem_pa;		/* m2p dma buffer address (physical, from pci) */
	VP rxmem_la;		/* p2m dma buffer address (virtual, non cache) */
	UW rxmem_pa;		/* p2m dma buffer address (physical, from pci) */
} SpWInfo;

SpWInfo spw_info[SPW_PORT_NUM];

typedef struct {
	W num;
	T_DEVREQ *devreq;
} msg_t;

#define DEFAULT_STACKSIZE		8192
#define DEFAULT_PRIORITY		130
#define HIGHER_PRIORITY			100

typedef enum {
	TXSPD_100MBPS =		0,
	TXSPD_50MBPS =		1,
	TXSPD_33MBPS =		2,
	TXSPD_25MBPS =		3,
	TXSPD_20MBPS =		4,
	TXSPD_10MBPS =		9,
	TXSPD_5MBPS =		19,
	TXSPD_2MBPS =		49
} spwr_txclk_div;


UB name[] = "spwar";	/* used for name of lock */

UW pci9054_config_base;
UW pci9054_space0_base;

/* prototype */
ER spwOpenFn(ID devid, UINT omode, GDI gdi);
ER spwCloseFn(ID devid, UINT option, GDI gdi);
ER spwAbortFn(T_DEVREQ *devreq, GDI gdi);
INT spwEventFn(INT evttyp, VP evtinf, GDI gdi);

const GDefDev gdef = {
	NULL,		/* exinf */
	"spwa",		/* devnm */
	3,			/* maxreqq */
	0,			/* drvatr */
	0,			/* devatr */
	0,			/* nsub */
	1,			/* blksz */
	spwOpenFn,	/* open */
	spwCloseFn,	/* close */
	spwAbortFn,	/* abort */
	spwEventFn,	/* event */
};


/* データ番号定義(全部32bitアクセス) */
enum {
	DN_SPW1CSR = -201,	/* SpW port1 link control/status register の属性データ番号 */
	DN_SPW2CSR = -202,	/* SpW port2 link control/status register の属性データ番号 */
	DN_SPW3CSR = -203,	/* SpW port3 link control/status register の属性データ番号 */
	DN_SPW4CSR = -204,	/* SpW port4 link control/status register の属性データ番号 */
	DN_SPW5CSR = -205,	/* SpW port5 link control/status register の属性データ番号 */
	DN_SPW6CSR = -206,	/* SpW port6 link control/status register の属性データ番号 */
	DN_TBLIDX  = -212,	/* アドレスレジスタのindex (UW index, UW data) */
	DN_TBLDAT  = -213,	/* SpWコントロールレジスタのデータ (UW index, UW data) */
	DN_INTC    = -220,	/* interrupt control register の属性データ番号 */
	DN_INTS    = -221,	/* interrupt status register の属性データ番号 */
	DN_TCOUT   = -222,	/* TimeCode出力レジスタ(w)の属性データ番号 */
	DN_TCIN    = -223,	/* TimeCode入力レジスタ(r)の属性データ番号 */
	DN_TCODEEN = -224,  /* TimeCode Enable Register (wr) 2008-12-17 Takayuki Yuasa added*/
	DN_TXTMO   = -253,	/* 送信タイムアウト */
	DN_RXTMO   = -254,	/* 受信タイムアウト */
	DN_SIGN    = -255	/* fpga signature register の属性データ番号, 読出しのみ */
};

/* link control flags */
#define SPW_LINKENABLE		0x00000001
#define SPW_LINKDISABLE		0x00000002
#define SPW_AUTOSTART		0x00000004

/* link status flags */
#define ENABLE_RX			0x00000001
#define ENABLE_TX			0x00000002
#define SEND_NULLS			0x00000004
#define SEND_FCTS			0x00000008
#define SEND_N_CHARS		0x00000010
#define SEND_TCODES			0x00000020
#define SEQUENCE_ERR		0x00000100
#define CREDIT_ERR			0x00000200
#define RX_ERR				0x00000400
#define PARITY_ERR			0x00001000
#define DISCONNECT_ERR		0x00002000
#define ESCAPE_ERR			0x00004000


/* interrupt flags */
#define TXBUFFER_RDY	0x00000001
#define RXBUFFER_DONE	0x00000002
#define FLG_ABORTED		0x80000000

#define LINK0_CHANGE	0x00010000
#define LINK1_CHANGE	0x00020000
#define LINK2_CHANGE	0x00040000

#define TIMECODE_RDY	0x01000000


/* interrupt status/control register bit */

/* IPコアのソースと、このドライバ内でのtxbuf,rxbufの0,1,2は、
 * 本当のポート番号の4,5,6に対応するもの (Takayuki Yuasa 2008-10-04)
 */

#define INT_TXBUF0_RDY		0x00000001
#define INT_RXBUF0_DONE		0x00000002
#define INT_TXBUF1_RDY		0x00000004
#define INT_RXBUF1_DONE		0x00000008
#define INT_TXBUF2_RDY		0x00000010
#define INT_RXBUF2_DONE		0x00000020
#define INT_LINK0_CHANGE	0x00010000
#define INT_LINK1_CHANGE	0x00020000
#define INT_LINK2_CHANGE	0x00040000
#define INT_TIMECODE_RDY	0x01000000


volatile ID fid_spw_int;
UB time_code;
UB port1_connected, port2_connected, port3_connected;

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

void link_mon_task(INT stacd, VP exinf)
{
	ER er = E_OK;
	UINT flg_pattern;
	UW link_status;
	UW intmask;
	UINT intsts;

	while (1) {
		er = tk_wai_flg(fid_spw_int, LINK0_CHANGE | LINK1_CHANGE | LINK2_CHANGE, TWF_ORW | TWF_BITCLR, &flg_pattern, TMO_FEVR);
		if (er != E_OK) {
			printf("ch0 main task : tk_wait_flg error\n");
		}

		tk_slp_tsk(50);


		if (flg_pattern & LINK0_CHANGE) {
			link_status = in_w(PORT1_CSR);
			if ((link_status & 0x0000003f) == 0x0000003f) {
				port1_connected = 1;	printf("link0 on (0x%08x)\n", link_status);
			} else {
				port1_connected = 0;	printf("link0 off (0x%08x)\n", link_status);
			}
			DI(intsts);
			intmask = in_w(INTC_REG);
			intmask |= INT_LINK0_CHANGE;
			out_w(INTC_REG, intmask);		// enable
			EI(intsts);
		}

		if (flg_pattern & LINK1_CHANGE) {
			link_status = in_w(PORT2_CSR);
			if ((link_status & 0x0000003f) == 0x0000003f) {
				port2_connected = 1;	printf("link1 on (0x%08x)\n", link_status);
			} else {
				port2_connected = 0;	printf("link1 off (0x%08x)\n", link_status);
			}
			DI(intsts);
			intmask = in_w(INTC_REG);
			intmask |= INT_LINK1_CHANGE;
			out_w(INTC_REG, intmask);		// enable
			EI(intsts);
		}

		if (flg_pattern & LINK2_CHANGE) {
			link_status = in_w(PORT3_CSR);
			if ((link_status & 0x0000003f) == 0x0000003f) {
				port3_connected = 1;	printf("link2 on (0x%08x)\n", link_status);
			} else {
				port3_connected = 0;	printf("link2 off (0x%08x)\n", link_status);
			}
			DI(intsts);
			intmask = in_w(INTC_REG);
			intmask |= INT_LINK2_CHANGE;
			out_w(INTC_REG, intmask);		// enable
			EI(intsts);
		}
	}
}


int spw_send(SpWInfo *info, UW *buf, UW len, W *alen)
{
	//printf("spw_send ch=%d\n", info->port);
	ER er = E_OK;
	UINT flg_pattern;
	UW intmask, status;
	W i, tx_bytes, rem_bytes;
	UW num, cmd;
	UW txbuf_pa;
	UW *p, *q;
	UINT intsts;

	txbuf_pa = (UW)info->buf_pa;
	rem_bytes = len;
	q = &buf[0];

	//printf("spw_send ch=%d\n", info->port);
	CPU_LED0_ON;

	while (rem_bytes > 0) {
		//printf("+");
		//printf("#1 ch=%d\n", info->port);
		if (rem_bytes > DMABUF_SIZE) {
			tx_bytes = DMABUF_SIZE;
			rem_bytes = rem_bytes - DMABUF_SIZE;
			cmd = 0x00000fff & tx_bytes;
			cmd = cmd | 0x80000000;
		}
		else {
			tx_bytes = rem_bytes;
			rem_bytes = 0;
			cmd = 0x00000fff & tx_bytes;
			cmd = cmd | 0xc0000000;
		}
		//printf("#2 ch=%d\n", info->port);
		CPU_LED1_ON;
		status = in_w((UW)info->reg + TXCSR_OFFSET);

		if ((status & 0x80000000) == 0) {
			//printf("#3 ch=%d\n", info->port);
			tk_clr_flg(info->int_flag, ~TXBUFFER_RDY);
			DI(intsts);
			intmask = in_w(INTC_REG);
			intmask |= TXBUFFER_RDY << (2*info->port);
			out_w(INTC_REG, intmask);	// enabel int, and wait...
			EI(intsts);

			CPU_LED1_OFF;
			er = tk_wai_flg(info->int_flag, TXBUFFER_RDY, TWF_ORW | TWF_BITCLR, &flg_pattern, info->snd_tmout);
			CPU_LED1_ON;
			if (er != E_OK) {
				if (er != E_TMOUT) printf("spw_send : tk_wait_flg error\n");
				break;
			}
		}

		//printf("#5 ch=%d\n", info->port);

		//Lock(&dma0_lock);

		p = (volatile UW *)info->txmem_la;	// すでにcache offになっているはず
		num = ((tx_bytes + 3) >> 2);
		for (i = 0; i < num; i++) *p++ = *q++;

		//printf("#6 ch=%d\n", info->port);
		Lock(&dma0_lock);
		//printf("#7 ch=%d\n", info->port);

		num = ((tx_bytes + 3) & 0xfffffffc);
		out_w(PLX_DMADPR0, 0);
		out_w(PLX_DMAPADR0, info->txmem_pa);
		out_w(PLX_DMALADR0, txbuf_pa);
		out_w(PLX_DMASIZE0, num);
		out_b(PLX_DMA0CSR, 0x00000003);

		//printf("#8 ch=%d\n", info->port);
		er = tk_wai_flg(fid_pci_int, DMA0_INT_ACTIVE, TWF_ORW | TWF_BITCLR, &flg_pattern, TMO_FEVR);
		//printf("#9 ch=%d\n", info->port);
		if (er != E_OK) {
			out_b(PLX_DMA0CSR, 0x04);	// abort dma ?
			Unlock(&dma0_lock);
			printf("tx dma : tk_wait_flg error\n");
			break;
		}
		//EnableInt(INTNO_EPCIA, 0);

		Unlock(&dma0_lock);

		//cmd = cmd | (0x00ff0000 & (info->txport << 16));
		//printf("#10 ch=%d\n", info->port);
		out_w((UW)info->reg + TXCSR_OFFSET, cmd);
		//printf("#11 ch=%d\n", info->port);
	}
	CPU_LED0_OFF;

	//printf("spw_send done ch=%d\n", info->port);

	return er;
}


// 戻り値:
// EOPのとき          T.B.D.
// EEPのとき          T.B.D.
// パケット途中のとき T.B.D.
// エラー時           T.B.D.
int spw_receive(SpWInfo *info, UW *buf, UW len, W *alen)
{
	//printf("spw_receive ch=%d\n", info->port);
	ER er = E_OK;
	UINT flg_pattern;
	W i, num, rx_bytes, rem_bytes, recv_num;
	UW rxbuf_pa;
	UW *p, *q;
	UW status;
	UW intmask;
	UINT intsts;

	//printf("*1 ch=%d\n", info->port);
	rxbuf_pa = (UW)info->buf_pa + RXBUF_OFFSET;
	recv_num = 0;
	p = buf;
	rem_bytes = len;

	CPU_LED2_ON;

	for (;;) {
		//printf("*");
		//printf("*2 ch=%d\n", info->port);
	//	printf("waiting... ch=%d\n", info->port);
	//	er = tk_wai_flg(info->int_flag, RXBUFFER_DONE, TWF_ORW | TWF_BITCLR, &flg_pattern, TMO_FEVR);
		CPU_LED2_OFF;
		//2008-10-07 Takayuki Yuasa modified
		//er = tk_wai_flg(info->int_flag, RXBUFFER_DONE, TWF_ORW | TWF_BITCLR, &flg_pattern, info->rcv_tmout);
		er = tk_wai_flg(info->int_flag, RXBUFFER_DONE | FLG_ABORTED, TWF_ORW | TWF_BITCLR, &flg_pattern, info->rcv_tmout);
		if (er != E_OK) {
			//time out
			//printf("time out");
			break;
		}

		//printf(")\n");
		CPU_LED2_ON;
		//printf("*3 ch=%d\n", info->port);

		if (flg_pattern & FLG_ABORTED) {
			er = E_ABORT;
			//EnableInt(INTNO_GPIO6, 0);
			break;
		}

		//printf("*4 ch=%d\n", info->port);
		status = in_w((UW)info->reg + RXCSR_OFFSET);
		rx_bytes = status & 0x00000fff;
		if (rx_bytes > 2048) {
			// irrregal, broken
			*alen = 0;
			//printf("@");
			return -1;
		}
		//printf("#%d",rx_bytes);
		num = ((rx_bytes + 3) & 0xfffffffc);

		//printf("*5 ch=%d\n", info->port);
		CPU_LED3_ON;
		if (num > 0) {
			//printf("*6 ch=%d\n", info->port);
			Lock(&dma1_lock);
			//printf("*7 ch=%d\n", info->port);

			out_w(PLX_DMADPR1, 0x00000008);		// local=>pci
			out_w(PLX_DMAPADR1, info->rxmem_pa);
			out_w(PLX_DMALADR1, rxbuf_pa);
			out_w(PLX_DMASIZE1, num);
			out_b(PLX_DMA1CSR, 0x03);

			//printf("*8 ch=%d\n", info->port);
			er = tk_wai_flg(fid_pci_int, DMA1_INT_ACTIVE, TWF_ORW | TWF_BITCLR, &flg_pattern, TMO_FEVR);
			//printf("*9 ch=%d\n", info->port);
			if (er != E_OK) {
				out_b(PLX_DMA1CSR, 0x04);	// abort ?
				Unlock(&dma1_lock);
				printf("rx dma : tk_wait_flg error\n");
				break;
			}

			//printf("*10 ch=%d\n", info->port);
			Unlock(&dma1_lock);
			//printf("*11 ch=%d\n", info->port);

			if (rem_bytes < rx_bytes) {
				num = (rem_bytes >> 2);
				recv_num += (rem_bytes & 0xfffffffc);
				rem_bytes = 0;
			}
			else {
				num = ((rx_bytes + 3) >> 2);
				recv_num += rx_bytes;
				rem_bytes -= rx_bytes;
			}
			q = (volatile UW *)info->rxmem_la;
			for (i = 0; i < num; i++) *p++ = *q++;		// 次のbufに続くときは32bit区切り

			//Unlock(&dma1_lock);
		}
		//printf("*12 ch=%d\n", info->port);
		// buffer change
		out_w((UW)info->reg + RXCSR_OFFSET, RXBUF_DONE);
		CPU_LED3_OFF;
		// enable rx int
	//	intmask = in_w(INTC_REG);
	//	intmask |= (RXBUFFER_DONE << (2*info->port));
	//	out_w(INTC_REG, intmask);



		if (status & RXBUF_EOP) {
			*alen = recv_num;
			//printf("^");
			break;
		}
		else if (status & RXBUF_EEP) {
			*alen = recv_num;
			er = E_MACV;
			//printf("-");
			break;
		}
		else if (rem_bytes == 0) {
			*alen = recv_num;
			er = E_OK;
			//printf("/");
			break;
		}
		// bufが足りなく、読み残したときはどうする？

		//printf("*13 ch=%d\n", info->port);
		// enable rx int
		DI(intsts);
		intmask = in_w(INTC_REG);
		intmask |= (RXBUFFER_DONE << (2*info->port));
		out_w(INTC_REG, intmask);
		EI(intsts);
		//printf("*14 ch=%d\n", info->port);
	}
	CPU_LED2_OFF;
	// enable rx int
	DI(intsts);
	intmask = in_w(INTC_REG);
	intmask |= (RXBUFFER_DONE << (2*info->port));
	out_w(INTC_REG, intmask);
	EI(intsts);

	return er;
}


//ER readData(SpWInfo *info, T_DEVREQ *devreq)
void read_task(INT stacd, VP exinf)
{
	ER err = E_OK;
	W req_cnt;
	UW req_data;
	SpWInfo *info;
	T_DEVREQ *devreq;
	msg_t msg;
	UW addr;

	tk_slp_tsk(500);

	info = (SpWInfo *)exinf;

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

		err=E_OK;

		switch (devreq->start) {
			case 0:
				err = spw_receive(info, devreq->buf, devreq->size, &devreq->asize);
				break;
			case DN_SPW1CSR:
				req_cnt = devreq->size;
				if (req_cnt == 4) {
					req_data = in_w(PORT1_CSR);
					memcpy(devreq->buf, &req_data, 4);
					devreq->asize = 4;
				} else {
					err = E_PAR;
				}
				break;
			case DN_SPW2CSR:
				req_cnt = devreq->size;
				if (req_cnt == 4) {
					req_data = in_w(PORT2_CSR);
					memcpy(devreq->buf, &req_data, 4);
					devreq->asize = 4;
				} else {
					err = E_PAR;
				}
				break;
			case DN_SPW3CSR:
				req_cnt = devreq->size;
				if (req_cnt == 4) {
					req_data = in_w(PORT3_CSR);
					memcpy(devreq->buf, &req_data, 4);
					devreq->asize = 4;
				} else {
					err = E_PAR;
				}
				break;
			case DN_SPW4CSR:
				req_cnt = devreq->size;
				if (req_cnt == 4) {
					req_data = in_w(PORT4_CSR);
					memcpy(devreq->buf, &req_data, 4);
					devreq->asize = 4;
				} else {
					err = E_PAR;
				}
				break;
			case DN_SPW5CSR:
				req_cnt = devreq->size;
				if (req_cnt == 4) {
					req_data = in_w(PORT5_CSR);
					memcpy(devreq->buf, &req_data, 4);
					devreq->asize = 4;
				} else {
					err = E_PAR;
				}
				break;
			case DN_SPW6CSR:
				req_cnt = devreq->size;
				if (req_cnt == 4) {
					req_data = in_w(PORT6_CSR);
					memcpy(devreq->buf, &req_data, 4);
					devreq->asize = 4;
				} else {
					err = E_PAR;
				}
				break;
			case DN_INTC:
				req_cnt = devreq->size;
				if (req_cnt == 4) {
					req_data = in_w(INTC_REG);
					memcpy(devreq->buf, &req_data, 4);
					devreq->asize = 4;
				} else {
					err = E_PAR;
				}
				break;
			case DN_INTS:
				req_cnt = devreq->size;
				if (req_cnt == 4) {
					req_data = in_w(INTS_REG);
					memcpy(devreq->buf, &req_data, 4);
					devreq->asize = 4;
				} else {
					err = E_PAR;
				}
				break;
			case DN_TCIN:
				req_cnt = devreq->size;
				if (req_cnt == 4) {
					req_data = in_w(TCIN_REG);
					memcpy(devreq->buf, &req_data, 4);
					devreq->asize = 4;
				} else {
					err = E_PAR;
				}
				break;
			case DN_TCODEEN: /* 2008-12-17 Takayuki Yuasa added */
				req_cnt = devreq->size;
				if (req_cnt == 4) {
					req_data = in_w(TCODEEN_REG);
					memcpy(devreq->buf, &req_data, 4);
					devreq->asize = 4;
				} else {
					err = E_PAR;
				}
				break;
			case DN_TBLIDX:
				req_cnt = devreq->size;
				if (req_cnt == 4) {
					req_data = info->tbl_index;
					memcpy(devreq->buf, &req_data, 4);
					devreq->asize = 4;
				} else {
					err = E_PAR;
				}
				break;
			case DN_TBLDAT:
				req_cnt = devreq->size;
				addr = ADRTBL_BASE + (info->tbl_index << 2);
				if (req_cnt == 4) {
					req_data = in_w(addr);
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
			case DN_SIGN:
				req_cnt = devreq->size;
				if (req_cnt == 4) {
					req_data = in_w(SIGN_REG);
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

err_ret:
		devreq->error = err;
		GDI_Reply(devreq, info->gdi);

		Unlock(&info->rdlock);

	}
}


//ER writeData(SpWInfo *info, T_DEVREQ *devreq)
void write_task(INT stacd, VP exinf)
{
	ER err = E_OK;
	W req_cnt;
	UW *req_data;
	SpWInfo *info;
	T_DEVREQ *devreq;
	msg_t msg;
	UW addr;

	tk_slp_tsk(500);

	info = (SpWInfo *)exinf;

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

		devreq->asize=0;

		switch (devreq->start) {
			case 0:
				err = spw_send(info, devreq->buf, devreq->size, &devreq->asize);
				break;
			case DN_SPW1CSR:
				req_cnt = devreq->size;
				req_data = devreq->buf;
				if (req_cnt == 4) {
					out_w(PORT1_CSR, *req_data);
					devreq->asize=4;
				} else {
					err = E_PAR;
				}
				break;
			case DN_SPW2CSR:
				req_cnt = devreq->size;
				req_data = devreq->buf;
				if (req_cnt == 4) {
					out_w(PORT2_CSR, *req_data);
					devreq->asize=4;
				} else {
					err = E_PAR;
				}
				break;
			case DN_SPW3CSR:
				req_cnt = devreq->size;
				req_data = devreq->buf;
				if (req_cnt == 4) {
					out_w(PORT3_CSR, *req_data);
					devreq->asize=4;
				} else {
					err = E_PAR;
				}
				break;
			case DN_SPW4CSR:
				req_cnt = devreq->size;
				req_data = devreq->buf;
				if (req_cnt == 4) {
					out_w(PORT4_CSR, *req_data);
					devreq->asize=4;
				} else {
					err = E_PAR;
				}
				break;
			case DN_SPW5CSR:
				req_cnt = devreq->size;
				req_data = devreq->buf;
				if (req_cnt == 4) {
					out_w(PORT5_CSR, *req_data);
					devreq->asize=4;
				} else {
					err = E_PAR;
				}
				break;
			case DN_SPW6CSR:
				req_cnt = devreq->size;
				req_data = devreq->buf;
				if (req_cnt == 4) {
					out_w(PORT6_CSR, *req_data);
					devreq->asize=4;
				} else {
					err = E_PAR;
				}
				break;
			case DN_INTC:
				req_cnt = devreq->size;
				req_data = devreq->buf;
				if (req_cnt == 4) {
					out_w(INTC_REG, *req_data);
					devreq->asize=4;
				} else {
					err = E_PAR;
				}
				break;
			case DN_INTS:
				req_cnt = devreq->size;
				req_data = devreq->buf;
				if (req_cnt == 4) {
					out_w(INTS_REG, *req_data);
					devreq->asize=4;
				} else {
					err = E_PAR;
				}
				break;
			case DN_TCOUT:
				req_cnt = devreq->size;
				req_data = devreq->buf;
				if (req_cnt == 4) {
					out_w(TCOUT_REG, *req_data);
					devreq->asize=4;
				} else {
					err = E_PAR;
				}
				break;
			case DN_TCODEEN: /* 2008-12-17 Takayuki Yuasa added */
				req_cnt = devreq->size;
				req_data = devreq->buf;
				if (req_cnt == 4) {
					out_w(TCODEEN_REG, *req_data);
					devreq->asize=4;
				} else {
					err = E_PAR;
				}
				break;
			case DN_TBLIDX:
				req_cnt = devreq->size;
				req_data = devreq->buf;
				if (req_cnt == 4) {
					info->tbl_index = *req_data;
					devreq->asize=4;
				} else {
					err = E_PAR;
				}
				break;
			case DN_TBLDAT:
				req_cnt = devreq->size;
				req_data = devreq->buf;
				addr = ADRTBL_BASE + (info->tbl_index << 2);
				if (req_cnt == 4) {
					out_w(addr, *req_data);
					devreq->asize=4;
				} else {
					err = E_PAR;
				}
				break;
			case DN_TXTMO:
				req_cnt = devreq->size;
				req_data = devreq->buf;
				if (req_cnt == 4) {
					info->snd_tmout = *req_data;
					devreq->asize=4;
				} else {
					err = E_PAR;
				}
				break;
			case DN_RXTMO:
				req_cnt = devreq->size;
				req_data = devreq->buf;
				if (req_cnt == 4) {
					info->rcv_tmout = *req_data;
					devreq->asize=4;
				} else {
					err = E_PAR;
				}
				break;
			default:
				err = E_PAR;
		}
		if (err < E_OK) goto err_ret;

err_ret:
		devreq->error = err;
		GDI_Reply(devreq, info->gdi);

		Unlock(&info->wrlock);

	}
}


ER spwOpenFn(ID devid, UINT omode, GDI gdi)
{
	ER err = E_OK;
	SpWInfo	*info = GDI_exinf(gdi);

	//if (info->open_num == 0) {
		if (info->port == 0) out_w(PORT4_CSR, 0x00000001);
		else if (info->port == 1) out_w(PORT5_CSR, 0x00000001);
		else if (info->port == 2) out_w(PORT6_CSR, 0x00000001);
	//}
	info->open_num++;

	return err;
}



ER spwCloseFn(ID devid, UINT option, GDI gdi)
{
	SpWInfo	*info = GDI_exinf(gdi);

	if (info->open_num == 0) return E_PAR;

	//if (info->open_num = 1) {
		if (info->port == 0) out_w(PORT4_CSR, 0x0);
		else if (info->port == 1) out_w(PORT5_CSR, 0x0);
		else if (info->port == 2) out_w(PORT6_CSR, 0x0);
	//}
	info->open_num--;

	return E_OK;
}



ER spwAbortFn(T_DEVREQ *devreq, GDI gdi)
{
	ER err = E_OK;
	SpWInfo	*info = GDI_exinf(gdi);

	//printf("spwAbortFn\n");

	tk_set_flg(info->int_flag, FLG_ABORTED);

	return err;
}



INT spwEventFn(INT evttyp, VP evtinf, GDI gdi)
{
	SpWInfo	*info = GDI_exinf(gdi);

	switch (evttyp) {
		case TDV_SUSPEND:
		case TDV_RESUME:
		default:	/* undefined event */
			printf("spwEventFn evttyp=%d\n", evttyp);
			return E_PAR;
	}
}



ER spwAcceptDeviceRequest(SpWInfo *info)
{
	T_DEVREQ *devreq;
	ER err;
	msg_t msg;

	for (;;) {	/* never terminate by itself */

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
		info->devreq = devreq;		//これは何？


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




void gpio6_int(UINT dintno)
{
	UW status;
	UW intmask;

	DisableInt(INTNO_GPIO6);
	//ClearInt(dintno);

	status = in_w(INTS_REG);

	if (status & INT_TXBUF0_RDY) {
		intmask = in_w(INTC_REG);
		intmask &= ~INT_TXBUF0_RDY;
		out_w(INTC_REG, intmask);	// disable
		tk_set_flg(spw_info[0].int_flag, TXBUFFER_RDY);
	}
	if (status & INT_TXBUF1_RDY) {
		intmask = in_w(INTC_REG);
		intmask &= ~INT_TXBUF1_RDY;
		out_w(INTC_REG, intmask);	// disable
		tk_set_flg(spw_info[1].int_flag, TXBUFFER_RDY);
	}
	if (status & INT_TXBUF2_RDY) {
		intmask = in_w(INTC_REG);
		intmask &= ~INT_TXBUF2_RDY;
		out_w(INTC_REG, intmask);	// disable
		tk_set_flg(spw_info[2].int_flag, TXBUFFER_RDY);
	}

	if (status & INT_RXBUF0_DONE) {
		intmask = in_w(INTC_REG);
		intmask &= ~INT_RXBUF0_DONE;
		out_w(INTC_REG, intmask);		// disable
		tk_set_flg(spw_info[0].int_flag, RXBUFFER_DONE);
	}
	if (status & INT_RXBUF1_DONE) {
		intmask = in_w(INTC_REG);
		intmask &= ~INT_RXBUF1_DONE;
		out_w(INTC_REG, intmask);		// disable
		tk_set_flg(spw_info[1].int_flag, RXBUFFER_DONE);
	}
	if (status & INT_RXBUF2_DONE) {
		intmask = in_w(INTC_REG);
		intmask &= ~INT_RXBUF2_DONE;
		out_w(INTC_REG, intmask);		// disable
		tk_set_flg(spw_info[2].int_flag, RXBUFFER_DONE);
	}

	if (status & INT_TIMECODE_RDY) {
		tk_set_flg(fid_spw_int, TIMECODE_RDY);
		time_code = (UB)in_w(TCIN_REG);	// clear bit
	}

	if (status & INT_LINK0_CHANGE) {
		intmask = in_w(INTC_REG);
		intmask &= ~INT_LINK0_CHANGE;
		out_w(INTC_REG, intmask);		// disable
		tk_set_flg(fid_spw_int, LINK0_CHANGE);
		out_w(INTS_REG, INT_LINK0_CHANGE);	// clear bit
	}
	if (status & INT_LINK1_CHANGE) {
		intmask = in_w(INTC_REG);
		intmask &= ~INT_LINK1_CHANGE;
		out_w(INTC_REG, intmask);		// disable
		tk_set_flg(fid_spw_int, LINK1_CHANGE);
		out_w(INTS_REG, INT_LINK1_CHANGE);	// clear bit
	}
	if (status & INT_LINK2_CHANGE) {
		intmask = in_w(INTC_REG);
		intmask &= ~INT_LINK2_CHANGE;
		out_w(INTC_REG, intmask);		// disable
		tk_set_flg(fid_spw_int, LINK2_CHANGE);
		out_w(INTS_REG, INT_LINK2_CHANGE);	// clear bit
	}
	EnableInt(INTNO_GPIO6, 0);
}


void epcia_int(UINT dintno)
{
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
INT regist_device(SpWInfo *info, BOOL reg)
{
	GDefDev	devdef;
	ER	err;

	/* register information */
	devdef = gdef;
	devdef.exinf = info;
	devdef.devnm[3] += info->port;	/* 0:spwa, 1:spwb, 2:spwc ... */

	/* define/undefine device */
	if (reg) {
		err = GDefDevice(&devdef, NULL, &info->gdi);
	} else {
		err = GDelDevice(info->gdi);
	}
	if (err < E_OK) goto err_ret;

	return E_OK;

err_ret:
	printf("regist_device err = %d(port = %d)\n", err, info->port);
	return err;
}



void spacewire_driver_main(INT stacd, VP exinf)
{
	ER err = 0;
	SpWInfo *info;

	info = (SpWInfo *)exinf;
	//printf("port = %d\n", info->port);
	//printf("pri = %d\n", info->mypri);

	name[3] = '0' + info->port;	/* name = "spw0r", "spw1r" ... */

	tk_slp_tsk(500);

	/* create lock */
	name[4] = 'r';
	err = CreateLock(&info->rdlock, name);
	if (err < E_OK) goto err_ret0;
	name[4] = 'w';
	err = CreateLock(&info->wrlock, name);
	if (err < E_OK) goto err_ret0;

	/* define device */
	err = regist_device(info, TRUE);
	if (err < E_OK) goto err_ret1;

	/* start receiving device request */
	err = spwAcceptDeviceRequest(info);
	if (err < E_OK) goto err_ret2;

	/* There is no case to execute here. */


	/* termination handling */
err_ret2:
	regist_device(info, FALSE);
err_ret1:
	DeleteLock(&info->wrlock);
	DeleteLock(&info->rdlock);
err_ret0:
	printf("spacewire_driver_main err = %d(port = %d)\n", err, info->port);
	tk_exd_tsk();
}



int start_spacewire_driver(W port, PRI pri)
{
	T_CTSK ct_spw;
	T_CFLG cf_spw_int;
	T_CMBF	cmbf;

	SpWInfo *p_spw;
	ID tskid;
	ER err;
	UW base;
	INT size;
	VP la, pa;
	UW pcia;

	if (port >= SPW_PORT_NUM) {
		err = E_LIMIT;
		goto err_ret1;
	}
	p_spw = &spw_info[port];
	memset(p_spw, 0, sizeof(SpWInfo));

	/* setup tx dma buffer */
	la = Kmalloc(4096);		// 2048で大丈夫?		//printf(" ch%d txdma_buf(cpu la)=0x%08x\n", port, (UW)la);
	//la = (VP)(((UW)la & 0xfffff800) + 0x00000800);
	//			printf(" ch%d txdma_buf(cpu la)=0x%08x\n", port, (UW)la);
	size = CnvPhysicalAddr(la, DMABUF_SIZE, &pa);
	if (size != DMABUF_SIZE) {
		printf("error : tx_buf_addr(cpu pa)=0x%08x, size=0x%08x\n", pa, size);
		return -1;
	}
	pcia = cpuAddrToPciMem((UW)pa);					printf(" ch%d txdma_buf(pci pa)=0x%08x\n", port, pcia);
	p_spw->txmem_la = la;
	p_spw->txmem_pa = pcia;

	/* setup tx dma buffer */
	la = Kmalloc(4096);								//printf(" ch%d rxdma_buf(cpu la)=0x%08x\n", port, (UW)la);
	//la = (VP)(((UW)la & 0xfffff800) + 0x00000800);
	//			printf(" ch%d rxdma_buf(cpu la)=0x%08x\n", port, (UW)la);
	size = CnvPhysicalAddr(la, DMABUF_SIZE, &pa);
	if (size != DMABUF_SIZE) {
		printf("error : rx_buf_addr(cpu pa)=0x%08x, size=0x%08x\n", pa, size);
		return -1;
	}
	pcia = cpuAddrToPciMem((UW)pa);					printf(" ch%d rxdma_buf(pci pa)=0x%08x\n", port, pcia);
	p_spw->rxmem_la = la;
	p_spw->rxmem_pa = pcia;

	/* physical address (CPUから見たFPGA) */
	base = FPGA_BASE + BUFCSR_OFFSET + port*BUFCSR_LENGTH;
	p_spw->reg = (VP)base;

	/* fpga tx-buffer physical address (PLXから見たLocal address) */
	base = port*BUFFER_LENGTH + BUFFER_OFFSET;
	p_spw->buf_pa = (VP)base;


	p_spw->port = port;
	p_spw->mypri = pri;
	p_spw->open_num = 0;
	p_spw->snd_tmout = TMO_FEVR;
	p_spw->rcv_tmout = TMO_FEVR;

	p_spw->tbl_index = 0;

	cf_spw_int.exinf = NULL;
	cf_spw_int.flgatr = TA_TFIFO | TA_WMUL;
	cf_spw_int.iflgptn = 0;
	p_spw->int_flag = tk_cre_flg(&cf_spw_int);	/* GPIO6 int flag */

	/* message buffer */
	cmbf.exinf = NULL;
	cmbf.mbfatr = TA_TFIFO;
	cmbf.bufsz  = sizeof(msg_t) * 32;
	cmbf.maxmsz = sizeof(msg_t);
	err = tk_cre_mbf(&cmbf);
	if (err < E_OK) goto err_ret1;
	p_spw->mbf_rreq = err;

	/* message buffer */
	cmbf.exinf = NULL;
	cmbf.mbfatr = TA_TFIFO;
	cmbf.bufsz  = sizeof(msg_t) * 32;
	cmbf.maxmsz = sizeof(msg_t);
	err = tk_cre_mbf(&cmbf);
	if (err < E_OK) goto err_ret1;
	p_spw->mbf_wreq = err;


	/* driver main */
	ct_spw.exinf = p_spw;
	ct_spw.tskatr = TA_HLNG | TA_RNG0;
	ct_spw.task = spacewire_driver_main;
	ct_spw.itskpri = pri;
	ct_spw.stksz = DEFAULT_STACKSIZE;

	err = tk_cre_tsk(&ct_spw);
	if (err < E_OK) goto err_ret1;
	tskid = (ID)err;

	err = tk_sta_tsk(tskid, (INT)p_spw);
	if (err < E_OK) goto err_ret2;

	/* read task */
	ct_spw.exinf = p_spw;
	ct_spw.tskatr = TA_HLNG | TA_RNG0;
	ct_spw.task = read_task;
	ct_spw.itskpri = HIGHER_PRIORITY;
	ct_spw.stksz = DEFAULT_STACKSIZE;

	err = tk_cre_tsk(&ct_spw);
	if (err < E_OK) goto err_ret1;
	tskid = (ID)err;

	err = tk_sta_tsk(tskid, (INT)p_spw);
	if (err < E_OK) goto err_ret2;

	/* write task */
	ct_spw.exinf = p_spw;
	ct_spw.tskatr = TA_HLNG | TA_RNG0;
	ct_spw.task = write_task;
	ct_spw.itskpri = HIGHER_PRIORITY;
	ct_spw.stksz = DEFAULT_STACKSIZE;

	err = tk_cre_tsk(&ct_spw);
	if (err < E_OK) goto err_ret2;
	tskid = (ID)err;

	err = tk_sta_tsk(tskid, (INT)p_spw);
	if (err < E_OK) goto err_ret2;

	return tskid;

err_ret2:
	tk_del_tsk(tskid);
	printf("start error2\n");
err_ret1:
	printf("start error1\n");
	return err;
}



ER main(INT ac, UB *av[])
{
	ER er = E_OK;
	W caddr;
	W size;
	VP addr;

	T_CFLG cf_int;
	T_DINT di_spw_int;
	T_CTSK ct_mon;

	if (ac < 0) {		/* アンロード時 */
		return E_NOSPT; /* デバイス抹消はサポートしない */
	}
	printf("spacecube test driver ver.0.0 build %s %s\n", __DATE__, __TIME__);

	caddr = searchPciDev(0x10b5, 0x9054);						//printf(" caddr=0x%08x\n", caddr);
	getPciBaseAddr(caddr, PCR_BASEADDR_0, &addr, &size);
	pci9054_config_base = pciMemToCpuAddr((unsigned long)addr);	//printf(" bar0=0x%08x, size=0x%08x\n", (unsigned long)addr, size);
	getPciBaseAddr(caddr, PCR_BASEADDR_1, &addr, &size);		//printf(" bar1=0x%08x, size=0x%08x\n", (unsigned long)addr, size);
	getPciBaseAddr(caddr, PCR_BASEADDR_2, &addr, &size);		//printf(" bar2=0x%08x, size=0x%08x\n", (unsigned long)addr, size);
	pci9054_space0_base = pciMemToCpuAddr((unsigned long)addr);
				//printf(" pci9054 config reg base=0x%08x\n", pci9054_config_base);
				//printf(" pci9054 local mem base=0x%08x\n", pci9054_space0_base);


	/* setup plx pci9054 */
	out_w(PLX_LBRD0, 0x41430043);
	out_w(PLX_LAS0BA, 0x00000001);
	out_w(PLX_INTCSR, 0x000d0100);

	out_w(PLX_INTCSR, 0x000c0100);		// PCI int enable, DMA CH0/1 int enable

	out_w(PLX_DMAMODE0, 0x00020543);
	out_w(PLX_DMAMODE1, 0x00020543);

	out_w(PLX_DMADPR0, 0);				// pci=>local (dma ch0 dedicated to tx)
	out_w(PLX_DMADPR1, 0x00000008);		// local=>pci (dma ch1 dedicated to rx)


	/* 全port共通 */

	/* event flag (SpaceWire Event) */
	cf_int.exinf = NULL;
	cf_int.flgatr = TA_TFIFO | TA_WMUL;
	cf_int.iflgptn = 0;
	fid_spw_int = tk_cre_flg(&cf_int);	/* SpW int flag */

	/* event flag (PCI DMA) */
	cf_int.exinf = NULL;
	cf_int.flgatr = TA_TFIFO | TA_WMUL;
	cf_int.iflgptn = 0;
	fid_pci_int = tk_cre_flg(&cf_int);	/* EPCIA int flag */

	er = CreateLock(&dma0_lock, "dma0");	// 各portが同時にdma0(tx)を使用しないように
	er = CreateLock(&dma1_lock, "dma1");	// 各portが同時にdma1(rx)を使用しないように

//	er = CreateLock(&intc_lock, "intc");	// intcのrmw用


	/* register interrupt handler */
	di_spw_int.intatr = TA_HLNG;
	di_spw_int.inthdr = gpio6_int;
	tk_def_int(INTNO_GPIO6, &di_spw_int);	/* GPIO6 int handler */

	di_spw_int.intatr = TA_HLNG;
	di_spw_int.inthdr = epcia_int;
	tk_def_int(INTNO_EPCIA, &di_spw_int);	/* EPCIA int handler */


	/* interrupt settings */
	SetIntMode(INTNO_GPIO6, IM_LEVEL | IM_LOW);
	SetIntMode(INTNO_EPCIA, IM_LEVEL | IM_LOW);

	ClearInt(INTNO_GPIO6);
	ClearInt(INTNO_EPCIA);

	EnableInt(INTNO_GPIO6, 0);
	EnableInt(INTNO_EPCIA, 0);

	/* initialize link */

	er = start_spacewire_driver(0, DEFAULT_PRIORITY); /* デバイス登録 */
	er = start_spacewire_driver(1, DEFAULT_PRIORITY); /* デバイス登録 */
	er = start_spacewire_driver(2, DEFAULT_PRIORITY); /* デバイス登録 */

	ct_mon.exinf = NULL;
	ct_mon.tskatr = TA_HLNG | TA_RNG0;
	ct_mon.task = link_mon_task;
	ct_mon.itskpri = DEFAULT_PRIORITY;
	ct_mon.stksz = DEFAULT_STACKSIZE;
	er = tk_cre_tsk(&ct_mon);
	if (er > E_OK) {
		er = tk_sta_tsk((ID)er, 0);
	}
	/* clear all fpga ints */
	out_w(INTS_REG, 0xffffffff);
	/* enable all fpga ints */
	out_w(INTC_REG, 0xffffffff);


	/* open port 1,2,3 at an initial link speed of 10MHz */
	int status;
	status=0x0005 | ((TXSPD_10MBPS << 24) & 0x3f000000);
	out_h(PORT1_CTRL, status);
	out_h(PORT2_CTRL, status);
	out_h(PORT3_CTRL, status);

	return ((er >= E_OK) ? E_OK : er);
}

/************************************************************************
制限事項:

既知不具合:

TODO項目:
  イベント処理&リンク状態,TimeCodeなどのイベントをプロセスに発行

*************************************************************************/

