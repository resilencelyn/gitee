#ifndef __MPC8247_DPRAM_H__
#define __MPC8247_DPRAM_H__

// 串口设备终端输出到数据文件接口
struct uart_file_s
{
    conf_object_t* obj;
    uart_file_intf* iface;
};

// 串口设备设备双向传输接口
struct skyeye_uart_s
{
    conf_object_t* obj;
    skyeye_uart_intf* iface;
};

struct ram_space_s
{
    conf_object_t* obj;
    memory_space_iface_t* iface;
};

struct smc_dev_s
{
    conf_object_t* obj;
    dpram_get_smc_reg_intf* iface;
};

struct skyeye_reg_s
{
    skyeye_reg_intf* iface;
    conf_object_t* obj;
};

typedef struct bufdesc {
	uint16_t cbd_sc;
	uint16_t cbd_datlen;
	uint32_t cbd_bufaddr;
} cbd_t;

// 中断管理器中断信号接口
struct general_signal_s
{
    conf_object_t* obj;
    general_signal_iface_t* iface;
};

// 设备对象结构
struct mpc8247_dpram_device_t
{
    conf_object_t* obj;
    // 接口对象和方法;
    struct uart_file_s uart_file;
    struct skyeye_uart_s skyeye_uart;
    struct ram_space_s ram_space;
    struct smc_dev_s smc_dev;
    struct general_signal_s general_signal;

    /*reg ----- smc1
    * reg ----- smc2
    */
    struct skyeye_reg_s smc_reg;
    struct smc_parameter_ram {
		uint16_t rbase;
		uint16_t tbase;
		uint16_t rx_fifosize;
        uint16_t rbptr;
    } smc_ram_parameter[2];

    uint16_t smc1_rambase_by_ram;
    uint16_t smc2_rambase_by_ram;
    uint8_t receiver_bd_char_count;
    cbd_t *pcbd;
    uint32_t baseAddr;
};

typedef struct mpc8247_dpram_device_t mpc8247_dpram_device;

/*SMC1 BASE ADDRESS: 0x87fc, 2 bytes
*/
#define DBRAM_BASE 0xf0000000

#define SMC1_RAM_BASE 0x87fc
#define SMC2_RAM_BASE 0x88fc

#define RBASE 0x00
#define TBASE 0x02
#define MRBLR 0x06
#define RBPTR 0x10

typedef uint16_t ushort;
#define BD_SC_EMPTY     ((ushort)0x8000)        /* Recieve is empty */
#define BD_SC_READY     ((ushort)0x8000)        /* Transmit is ready */
#define BD_SC_WRAP      ((ushort)0x2000)        /* Last buffer descriptor */
#define BD_SC_INTRPT    ((ushort)0x1000)        /* Interrupt on change */
#define BD_SC_CM        ((ushort)0x0200)        /* Continous mode */
#define BD_SC_ID        ((ushort)0x0100)        /* Rec'd too many idles */
#define BD_SC_P         ((ushort)0x0100)        /* xmt preamble */
#define BD_SC_BR        ((ushort)0x0020)        /* Break received */
#define BD_SC_FR        ((ushort)0x0010)        /* Framing error */
#define BD_SC_PR        ((ushort)0x0008)        /* Parity error */
#define BD_SC_OV        ((ushort)0x0002)        /* Overrun */
#define BD_SC_CD        ((ushort)0x0001)        /* ?? */

#define SMCMR_SM_GCIorSCIT 0x00
#define SMCMR_SM_RES 0x1
#define SMCMR_SM_UART 0x2
#define SMCMR_SM_TRANSPARENT 0x3

#endif
