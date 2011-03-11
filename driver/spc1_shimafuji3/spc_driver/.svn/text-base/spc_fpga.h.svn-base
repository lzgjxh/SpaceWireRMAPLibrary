#ifndef _SPACECUBE_FPGA_H_
#define _SPACECUBE_FPGA_H_

#define FPGA_BASE			0xb0000000

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
#define TXBUF_OFFSET		0x18
#define RXBUF_OFFSET		0x1c
#define SIGN_OFFSET			0xfc

#define REG_OFFSET			0x00001f00
#define RXMEM_OFFSET		0x00000800
#define PORT_OFFSET			0x00002000

/* –³‚­‚µ‚½‚¢ */
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


#endif /*_SPACECUBE_FPGA_H_*/
