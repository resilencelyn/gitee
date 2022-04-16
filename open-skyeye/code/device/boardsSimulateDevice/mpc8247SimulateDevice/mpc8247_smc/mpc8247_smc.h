#ifndef __MPC8247_SMC_H__
#define __MPC8247_SMC_H__

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    "SMCMR",
    "SMCE",
    "SMCM",
    NULL
};

// 寄存器偏移地址
#define _SMCMR 0x2
#define _SMCE 0x6
#define _SMCM 0xa

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{
    _SMCMR,
    _SMCE,
    _SMCM,

};

// 设备寄存器结构体
struct registers
{
    uint16_t smcmr;
    uint8_t smce;
    uint8_t smcm;
};

// 中断管理器中断信号接口
struct general_signal_s
{
    conf_object_t* obj;
    general_signal_iface_t* iface;
};

// 串口设备设备双向传输接口
struct skyeye_uart_s
{
    conf_object_t* obj;
    skyeye_uart_intf* iface;
};

// 串口设备终端输出到数据文件接口
struct uart_file_s
{
    conf_object_t* obj;
    uart_file_intf* iface;
};

// 串口设备终端输出到虚拟串口接口
struct uart_com_s
{
    conf_object_t* obj;
    uart_com_intf* iface;
};

// 设备对象结构
struct mpc8247_smc_device_t
{
    conf_object_t* obj;
    // 接口对象和方法;
    struct general_signal_s general_signal;
    struct skyeye_uart_s skyeye_uart;
    struct uart_file_s uart_file;
    struct uart_com_s uart_com;
    struct registers regs;
    // 属性;
    uint32_t reverse;
    uint32_t int_number;
    // 定时器;
    time_handle_t timer_handle[1];
};

typedef struct mpc8247_smc_device_t mpc8247_smc_device;

/* bsd */
typedef unsigned char		u_char;
typedef unsigned short		u_short;
typedef unsigned int		u_int;
typedef unsigned long		u_long;

/* sysv */
typedef unsigned char		unchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;

/* The SMCs are relocated to any of the first eight DPRAM pages.
 * We will fix these at the first locations of DPRAM, until we
 * get some microcode patches :-).
 * The parameter ram space for the SMCs is fifty-some bytes, and
 * they are required to start on a 64 byte boundary.
 */
#define PROFF_SMC1	(0)
#define PROFF_SMC2	(64)
#define PROFF_SPI	((16*1024) - 128)

/* Define enough so I can at least use the serial port as a UART.
*/
typedef struct smc_uart {
    ushort	smc_rbase;	/* Rx Buffer descriptor base address */
    ushort	smc_tbase;	/* Tx Buffer descriptor base address */
    u_char	smc_rfcr;	/* Rx function code */
    u_char	smc_tfcr;	/* Tx function code */
    ushort	smc_mrblr;	/* Max receive buffer length */
    uint	smc_rstate;	/* Internal */
    uint	smc_idp;	/* Internal */
    ushort	smc_rbptr;	/* Internal */
    ushort	smc_ibc;	/* Internal */
    uint	smc_rxtmp;	/* Internal */
    uint	smc_tstate;	/* Internal */
    uint	smc_tdp;	/* Internal */
    ushort	smc_tbptr;	/* Internal */
    ushort	smc_tbc;	/* Internal */
    uint	smc_txtmp;	/* Internal */
    ushort	smc_maxidl;	/* Maximum idle characters */
    ushort	smc_tmpidl;	/* Temporary idle counter */
    ushort	smc_brklen;	/* Last received break length */
    ushort	smc_brkec;	/* rcv'd break condition counter */
    ushort	smc_brkcr;	/* xmt break count register */
    ushort	smc_rmask;	/* Temporary bit mask */
    uint	smc_stmp;	/* SDMA Temp */
} smc_uart_t;

/* SMC uart mode register (Internal memory map).
*/
#define	SMCMR_REN	((ushort)0x0001)
#define SMCMR_TEN	((ushort)0x0002)
#define SMCMR_DM	((ushort)0x000c)
#define SMCMR_SM_GCI	((ushort)0x0000)
#define SMCMR_SM_UART	((ushort)0x0020)
#define SMCMR_SM_TRANS	((ushort)0x0030)
#define SMCMR_SM_MASK	((ushort)0x0030)
#define SMCMR_PM_EVEN	((ushort)0x0100)	/* Even parity, else odd */
#define SMCMR_REVD	SMCMR_PM_EVEN
#define SMCMR_PEN	((ushort)0x0200)	/* Parity enable */
#define SMCMR_BS	SMCMR_PEN
#define SMCMR_SL	((ushort)0x0400)	/* Two stops, else one */
#define SMCR_CLEN_MASK	((ushort)0x7800)	/* Character length */
#define smcr_mk_clen(C)	(((C) << 11) & SMCR_CLEN_MASK)

/* SMC Event and Mask register.
*/
#define	SMCM_TXE	((ushort)0x10)
#define	SMCM_BSY	((ushort)0x04)
#define	SMCM_TX		((ushort)0x02)
#define	SMCM_RX		((ushort)0x01)
#define SMCM_TXorRX ((ushort)0x3)

/* Baud rate generators.
*/
#define CPM_BRG_RST		((uint)0x00020000)
#define CPM_BRG_EN		((uint)0x00010000)
#define CPM_BRG_EXTC_INT	((uint)0x00000000)
#define CPM_BRG_EXTC_CLK3_9	((uint)0x00004000)
#define CPM_BRG_EXTC_CLK5_15	((uint)0x00008000)
#define CPM_BRG_ATB		((uint)0x00002000)
#define CPM_BRG_CD_MASK		((uint)0x00001ffe)
#define CPM_BRG_DIV16		((uint)0x00000001)

#endif
