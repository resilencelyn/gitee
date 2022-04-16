#ifndef __LEON2_IO_H__
#define __LEON2_IO_H__

#ifndef __SPARC_TYPES_H_
#error "types.h header file must be included before including mach_leon2_regmap.h"
#endif
#include <stdio.h>

//#define leon2_declare_device(name) \
//void name##_write(uint32 size, uint32 addr, uint32 v); \
//uint32 name##_read(uint32 size, uint32 addr);    \
//void name##_cycle(void *arg);   \
//void name##_init(void *state)

#define TIMER1_ADDR     0x80000040
#define TIMER_IRQ       8

#define UART1_ADDR      0x80000070
#define UART1_IRQ        3

#define MCFG_ADDR       0x80000000
#define CFG_ADDR        0x80000024
#define IRQCTRL_ADDR    0x80000090

#ifdef __cplusplus
extern "C"
{
#endif

/*-----------------------------------------------------------------------------
 *  TIMERS REGISTERS
 *-----------------------------------------------------------------------------*/

    typedef uint32(*io_read_callback) (void *opaque, uint32 addr);
    typedef uint32(*io_write_callback) (void *opaque, uint32 addr, uint32 val);

    int leon2_register_io_memory(uint32 address,        /*  I/O address */
                                 uint32 size,   /*  I/O area size (bytes)   */
                                 io_read_callback fwrite,       /*  read functin callback   */
                                 io_write_callback fread,       /*  write function callback */
                                 void *opaque); /*  private data    */

#ifdef __cplusplus
}
#endif

#define DBG_LEON2  1
#define DBG_leon2(fmt, ...)      if(DBG_LEON2){                                                     \
					printf(fmt, ## __VA_ARGS__);   \
					}

#endif
