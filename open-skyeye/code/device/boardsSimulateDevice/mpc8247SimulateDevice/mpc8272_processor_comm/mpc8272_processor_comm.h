#ifndef __MPC8272_PROCESSOR_COMM_H__
#define __MPC8272_PROCESSOR_COMM_H__

// 所有设备的寄存器名称、列表顺序和结构寄存器寄存器内顺序保持一致
static char* regs_name[] =
{
    "CPCR",
    "RCCR",
    "RTER",
    "RTMR",
    "RTSCR",
    "RTSR",
    NULL
};

#define CPCR_RST        (1 << 31)
#define CPCR_FLG        (1 << 16)

#define CPCR_PAGE_MASK          (0x1f << 26)
#define CPCR_SBC_CODE_MASK      (0x1f << 21)
#define CPCR_OPCODE_MASK        (0xf << 0)

/*page*/
#define FCC1_SBC_PAGE   (0x4)
#define FCC2_SBC_PAGE   (0x5)
#define SCC1_SBC_PAGE   (0x0)
#define SCC3_SBC_PAGE   (0x2)
#define SCC4_SBC_PAGE   (0x3)
#define SMC1_SBC_PAGE   (0x7)
#define SMC2_SBC_PAGE   (0x8)
#define RAND_SBC_PAGE   (0xa)
#define SPI_SBC_PAGE    (0x9)
#define I2C_SBC_PAGE    (0xa)
#define TIMER_SBC_PAGE  (0xa)
#define IDMA2_SBC_PAGE  (0x8)
#define IDMA3_SBC_PAGE  (0x9)
#define USB_SBC_PAGE    (0xb)

/*sub block code*/
#define FCC1_SBC_CODE   (0x10)
#define FCC1_ATM_Transmit_SBC_CODE (0xe)
#define FCC2_SBC_CODE   (0x11)
#define FCC2_ATM_Transmit_SBC_CODE (0xe)
#define SCC1_SBC_CODE   (0x4)
#define SCC3_SBC_CODE   (0x6)
#define SCC4_SBC_CODE   (0x7)
#define SMC1_SBC_CODE   (0x8)
#define SMC2_SBC_CODE   (0x9)
#define SPI_SBC_CODE    (0xa)
#define I2C_SBC_CODE    (0xb)
#define TIMER_SBC_CODE  (0xf)
#define IDMA2_SBC_CODE  (0x15)
#define IDMA3_SBC_CODE  (0x16)
#define USB_SBC_CODE    (0x13)

/* Some opcodes (there are more...later)
*/
#define CPM_CR_INIT_TRX		((ushort)0x0000)
#define CPM_CR_INIT_RX		((ushort)0x0001)
#define CPM_CR_INIT_TX		((ushort)0x0002)
#define CPM_CR_HUNT_MODE	((ushort)0x0003)
#define CPM_CR_STOP_TX		((ushort)0x0004)
#define CPM_CR_RESTART_TX	((ushort)0x0006)
#define CPM_CR_SET_GADDR	((ushort)0x0008)

// 寄存器偏移地址,基地址0x119c0
#define _CPCR 0x0
#define _RCCR 0x4
#define _RTER 0x16
#define _RTMR 0x1a
#define _RTSCR 0x1c
#define _RTSR 0x20

// 寄存器偏移地址数组，列表顺序和寄存器结构体定义的顺序保持一致
static uint32_t regs_offset[] =
{
    _CPCR,
    _RCCR,
    _RTER,
    _RTMR,
    _RTSCR,
    _RTSR,

};

// 设备寄存器结构体
struct registers
{
    uint32_t CPCR;
    uint32_t RCCR;
    uint16_t RTER;
    uint16_t RTMR;
    uint16_t RTSCR;
    uint32_t RTSR;
};

// 设备对象结构
struct mpc8272_processor_comm_device_t
{
    conf_object_t* obj;
    // 接口对象和方法;
    struct registers regs;
};

typedef struct mpc8272_processor_comm_device_t mpc8272_processor_comm_device;
typedef void (*pc_fun_ptr_t) (mpc8272_processor_comm_device *dev, uint8_t, uint8_t);

#endif
