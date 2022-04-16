/*
 * =====================================================================================
 *
 *       Filename:  tsc695f_uart.h
 *
 *    Description:  UART variables
 *
 *        Version:  1.0
 *        Created:  20/05/08 15:53:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aitor Viana Sanchez (avs), aitor.viana.sanchez@esa.int
 *        Company:  European Space Agency (ESA-ESTEC)
 *
 * =====================================================================================
 */

#ifndef _TSC695F_UART_H_
#define _TSC695F_UART_H_
/*-----------------------------------------------------------------------------
 *  UART variables
 *-----------------------------------------------------------------------------*/
typedef struct reg_status{
    uint8 dra                       :1;
    uint8 tsea                      :1;
    uint8 thea                      :1;
    uint8 reserved0                 :1;
    uint8 fea                       :1;
    uint8 pea                       :1;
    uint8 oea                       :1;
    uint8 cua                       :1;
    uint8 reserved1                 :8;
    uint8 drb                       :1;
    uint8 tseb                      :1;
    uint8 theb                      :1;
    uint8 reserved2                 :1;
    uint8 feb                       :1;
    uint8 peb                       :1;
    uint8 oeb                       :1;
    uint8 cub                       :1;
    uint8 reserved3                 :8;
}status_t;

#define FIFO_SIZE	1024
typedef struct _FIFO
{
    unsigned char     *pFirst;
    unsigned char     *pLast;
    unsigned char     *pIn;
    unsigned char     *pOut;
    unsigned int   Length;
    unsigned int   Enteres;

}FIFO;
typedef struct UARTState
{
    conf_object_t *obj;
    struct registers
    {
        uint32 uarta_data;
        uint32 uartb_data;

        union
        {
            status_t flag;
            uint32 value;
        }status;

    }regs;
    conf_object_t* term;
    skyeye_uart_intf* term_iface;

    FIFO *read_fifo;
    int fifo_length;
    conf_object_t *signal;
    general_signal_intf *signal_iface;
    conf_object_t *uart_file;
    uart_file_intf *uart_file_iface;
    int int_number;
    int timer_scheduler_id;
}tsc695f_uart_dev;

static char* regs_name[] =
{
    "uarta_Data_Reg",
    "uartb_Data_Reg",
    "Status_Reg",
    NULL
};

#define DBG_LEON2_U	0
#define DBG_LEON2_UART(fmt, ...)      if(DBG_LEON2_U){					\
    printf(fmt, ## __VA_ARGS__);	\
}
#define UARTA_DATA_REGISTER      0x0
#define UARTB_DATA_REGISTER      0x4
#define UART_STATUS_REGISTER    0x8

#define UART_NREGS  6

//**************Uart Status Register Bits************
#define  DRA  0x1
#define  DRB  0x10000

//************************Registers Define*****
#define  UDATA    dev->regs.data
#define  USTATUS  dev->regs.status.value


#endif
