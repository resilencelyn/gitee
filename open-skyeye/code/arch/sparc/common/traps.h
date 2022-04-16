/*
 * =====================================================================================
 *
 *       Filename:  traps.h
 *
 *    Description:  Header file for the SPARC Traps implementation
 *
 *        Version:  1.0
 *        Created:  21/04/08 18:22:29
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aitor Viana Sanchez (avs), aitor.viana.sanchez@esa.int
 *        Company:  European Space Agency (ESA-ESTEC)
 *
 * =====================================================================================
 */

#ifndef _TRAPS_H_
#define _TRAPS_H_

#include "skyeye_types.h"

/* 
 * ===  CLASS     ======================================================================
 *         Name:  trap_handle_t
 *  Description:  This class defines what must be implemented by the trap
 *  implementation.
 * =====================================================================================
 */
typedef struct _trap_t
{
    int (*init) (void *);
    int (*signal) (conf_object_t * conf_obj, int tt);
    int (*irq_signal) (conf_object_t * conf_obj, int tt);
    int (*trigger) (conf_object_t * conf_obj, int tt);
} trap_handle_t;

int tt_is_exception(int tt);
void tbr_set_tt(void *sparc_state, uint16 tt);

/* 
 * ===  CLASS     ======================================================================
 *         Name:  trap_mode_t
 *  Description:  This class defines the trap mode type. A trap may be
 *  synchronous or asynchronous.
 * =====================================================================================
 */
typedef enum _trapmode
{
    NOTRAP = 0,
    SYNCH_TRAP,
    ASYNCH_TRAP,
} trap_mode_t;

/* 
 * ===  CLASS     ======================================================================
 *         Name:  trap_t
 *  Description:  This class defines the parameters related to the traps. The
 *  parameters are:
 *      - tt:       Trap Type
 *      - priority: Trap priority
 *      - mode:     Trap mode (sync or async)
 * =====================================================================================
 */
typedef struct _trap_s
{
    uint8 tt;
    uint8 priority;
    trap_mode_t mode;
} trap_t;

/** exceptions or interrupt request declarations    */
typedef enum
{
    RESET = 0x0,
    DATA_STORE_ERROR,
    INSTR_ACCESS_MMU_MISS,
    INSTR_ACCESS_ERR,
    R_REG_ACCESS_ERR,
    INSTR_ACCESS_EXCEPTION,
    PRIVILEGED_INSTR,
    ILLEGAL_INSTR,
    FP_DISABLED,
    CP_DISABLED,
    UNIMP,
    WATCHPOINT_DETECTED,
    WOF,
    WUF,
    MEM_ADDR_NOT_ALIGNED,
    FP_EXCEPTION,
    CP_EXCEPTION,
    DATA_ACCESS_ERR,
    DATA_ACCESS_MMU_MISS,
    DATA_ACCESS_EXCEPTION,
    TAG_OVERFLOW,
    DIV_BY_ZERO,
    AHB_ERROR,
    UART1,
    UART2,
    PIO_0,
    PIO_1,
    PIO_2,
    PIO_3,
    TIMER1,
    TIMER2,
    SECOND_INTE_CONTRL,
    DSU_TRACE_BUFFER,
    ETHER_MAC,
    USER_DEFINED1,
    PCI,
    USER_DEFINED2
} sparc_exp_t;

typedef enum
{
    UPDATE_CWP = 0x100,
    SUB_CWP,
    ADD_CWP,
    RETT_TRAP,
    TRAP_IMPL_MAX,
} sparc_exp_impl_t;

#define get_sparc_exp(sparc_state)		(sparc_state->sparc_exp)
#define set_sparc_exp(exp) 	(sparc_state->sparc_exp = exp);

/*-----------------------------------------------------------------------------
 *  PUBLIC INTERFACE
 *-----------------------------------------------------------------------------*/
extern trap_handle_t trap_handle;

#endif
