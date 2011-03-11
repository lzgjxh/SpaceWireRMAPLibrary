#ifndef _SPW_IF_H_
#define _SPW_IF_H_


/* データ番号定義(全部32bitアクセス) */
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
#define SPW_LINKENABLE		0x00010000
#define SPW_LINKDISABLE		0x00020000
#define SPW_AUTOSTART		0x00040000
#define SPW_LINKRESET		0x00080000

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

/** 2008-10-09 added */
#define SPWR_READ_MAXSIZE 51200

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

extern int spwr_open(int portnum);
extern int spwr_close(int dd);
extern int spwr_set_txspeed(int dd,int port, spwr_txclk_div txclk_div);
extern int spwr_set_read_timeout(int dd,UW time_out);
extern int spwr_set_write_timeout(int dd,UW time_out);
extern int spwr_read(int dd,UW *buf, UW *size);
extern int spwr_write(int dd,UW *buf, UW size);
extern int spwr_send_timecode(int dd,unsigned char flag_and_timecode);
extern int spwr_get_timecode(int dd,unsigned char* flag_and_timecode);

extern int spwr_table_write(int dd,UW index, UW data);
extern UW spwr_table_read(int dd,UW index);

extern int spwr_reset_link(int dd,int port);
extern int spwr_get_linkstatus(int dd,int port,unsigned int* status);



#endif /*_SPW_IF_H_*/
