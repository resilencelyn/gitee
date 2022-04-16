/*
 * =====================================================================================
 *
 *       Filename:  traps.c
 *
 *    Description:  Traps SPARC implementation
 *
 *        Version:  1.0
 *        Created:  21/04/08 18:21:30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aitor Viana Sanchez (avs), aitor.viana.sanchez@esa.int
 *        Company:  European Space Agency (ESA-ESTEC)
 *
 * =====================================================================================
 */

#include "types.h"
#include "traps.h"
#include "sparc.h"
#include "iu.h"
#include "bits.h"
#include "skyeye.h"

#include <stdio.h>
#define ET 0x20

static int init(void *);
static int trigger(conf_object_t * conf_obj, int tt);
static int signal(conf_object_t * conf_obj, int trap);
static int irq_signal(conf_object_t * conf_obj, int trap);

/** this is the trap handle class which will be called by the IU to handle all
 * the trap situations  */
trap_handle_t trap_handle = {
    init,
    signal,
    irq_signal,
    trigger,
};

/*
 * ===  CLASS     ======================================================================
 *         Name:  tt_table
 *  Description:  This structure gets all the information regarding the
 *  interrupt/exceptions and the priority
 * =====================================================================================
 */

/** This structure stores all the handler functions for all the traps   */

/** This value stores the number of elements in the tt_table    */
static trap_t tt_table[] = {
    //tt  prio type
    {0x00, 1, NOTRAP},                  // reset
    {0x2B, 2, NOTRAP},                  // data store error
    {0x3C, 2, NOTRAP},                  // instruction access MMU miss
    {0x21, 3, NOTRAP},                  // instruction access error
    {0x20, 4, NOTRAP},                  // r register access error
    {0x01, 5, NOTRAP},                  //  instruction access exception
    {0x03, 6, NOTRAP},                  //  priviledge instruction
    {0x02, 7, NOTRAP},                  //  illegal instruction
    {0x04, 8, NOTRAP},                  //  fp disabled
    {0x24, 8, NOTRAP},                  //  cp disabled
    {0x25, 8, NOTRAP},                  //  unimplemented FLUSH
    {0x0B, 8, NOTRAP},                  //  watchpoint detected
    {0x05, 9, NOTRAP},                  //  window overflow
    {0x06, 9, NOTRAP},                  //  window underflow
    {0x07, 10, NOTRAP},                 //  mem address not aligned
    {0x08, 11, NOTRAP},                 //  fp exception
    {0x28, 11, NOTRAP},                 //  cp exception
    {0x29, 12, NOTRAP},                 //  data access error
    {0x2C, 12, NOTRAP},                 //  data access MMU miss
    {0x09, 13, NOTRAP},                 //  data access exception
    {0x0A, 14, NOTRAP},                 //  tag overflow
    {0x2A, 15, NOTRAP},                 //  division by zero
};

static int tt_table_elem = sizeof (tt_table) / sizeof (trap_t);

int tt_is_exception(int tt)
{
    int i;

    if (tt >= 0x80 && tt <= 0xff)
        return 1;

    for (i = 0; i < tt_table_elem; i++)
    {
        if (tt == tt_table[i].tt)
            return 1;
    }

    return 0;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  init
 *  Description:  This function initializes the trap module. The function store
 *  a local copy of the processor state 'sparc_state'.
 * =====================================================================================
 */
static int init(void *state_)
{
    /*  FIXME!: to be implemented   */

    if (!state_)
        return SPARC_ERROR;
    /*  this is the processor state */

    DBG("TRAPS initialized\n");
    return SPARC_SUCCESS;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  signal
 *  Description:  This function signals a trap. That means that the irq_pending
 *  flag will be flagged, and the TT field of the TBR register will set to the
 *  correct value
 * =====================================================================================
 */
static int signal(conf_object_t * conf_obj, int trap)
{
    sparc_state_t *sparc_state = (sparc_state_t *) conf_obj->obj;

    if ((trap < 0) || sparc_state->trap_impl != TRAP_IMPL_MAX)
        return SPARC_ERROR;
    else if (trap >= tt_table_elem)
    {
        /*  SW trap, the 'trap' value is the tt */
        sparc_state->trap = trap;
    } else if ((trap > 0) || (trap < tt_table_elem))
    {
        /*  Hw trap, the 'trap' value is the index in the table */
        sparc_state->trap = tt_table[trap].tt;
    }
#ifdef SPARC_ENABLE_STAT
    /*  Do statistics   */
    switch (trap)
    {
        case WUF:
            statistics.nunderflow++;
            break;
        case WOF:
            statistics.noverflow++;
            break;
    }
#endif

    return SPARC_SUCCESS;
}

/**Tsc695f soc Interrupts :
 *******      Trap                          tt
 *******      Watchdog time-out             0x1F
 *******      INT4                          0x1E
 *******      RTCT                          0x1D
 *******      GPT 		            0x1C
 *******      INT3 			    0x1B
 *******      INT2 			    0x1A
 *******      DMA time-out 		    0x19
 *******      DMA access error	            0x18
 *******      UART Error 	            0x17
 *******      Correctable error in memory   0x16
 *******      UARTB 		            0x15
 *******      UARTA			    0x14
 *******      INT1 		            0x13
 *******      INT0 			    0x12
 *******      Masked hardware Error 	    0x11
****************************/
static int irq_signal(conf_object_t * conf_obj, int trap)
{
    sparc_state_t *sparc_state = (sparc_state_t *) conf_obj->obj;
    int bp_IRL;
    int PIL;

    if ((trap < 0x1) && (trap > 0xf))
        return SPARC_ERROR;

    bp_IRL = trap;
    PIL = (sparc_state->psr >> 8) & 0xf;

    // bp_IRL = PIL is not consist the sparcv8 manual, but can run vxWorks correctly
    if ((sparc_state->psr & ET) && ((bp_IRL >= PIL) || (bp_IRL == 15)))
    {
        sparc_state->trap_impl = trap + 0x10;
        sparc_state->irq_income = 1;
        return SPARC_SUCCESS;
    }

    return SPARC_ERROR;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  trigger
 *  Description:  This function triggers a trap. A trap causes the following to
 *  occur, if ET = 1:
 *  - Traps are disabled, ET = 0
 *  - The existing user supervisor mode is preserved, PS <- S
 *  - The user/supervisor mode is changed to supervisor, S = 1
 *  - The register window is advanced to a new window, CWP = (CWP - 1) % NWINDOWS
 *  - The trapped program counters are saved in local registers 1 and 2 of the
 *  new window, r[17] = PC, r[18] = NPC
 *  - The tt field is written to the particular value that identifies the
 *  exception of interrupt request, except as defined for 'Reset Trap' of 'Error
 *  Mode'
 *  - If the trap is 'Reset Trap' control is transfered to 0 address, PC = 0,
 *  NPC = 4
 *  - if the trap is not a 'Reset Trap' control is transferred into the trap
 *  table, PC=TBR, NPC = TBR + 4
 *
 *  If ET = 0 and a precise trap occurs, the processor enters in error_mode
 *  state and halts execution. If ET = 0 and an interrupt request or an
 *  interrupting or deferred exception occurs, it is ignored.
 * =====================================================================================
 */
static int trigger(conf_object_t * conf_obj, int tt)
{
    sparc_state_t *sparc_state = (sparc_state_t *) conf_obj->obj;
    int et = bit(PSRREG, PSR_ET);

    if (et)
    {
        int S;

        // disable traps
        clear_bit(PSRREG, PSR_ET);

        tbr_set_tt(sparc_state, tt);

        // supervisor mode is preserved
        S = bit(PSRREG, PSR_S);
        if (S)
            set_bit(PSRREG, PSR_PS);
        else
            clear_bit(PSRREG, PSR_PS);

        /*  put the processor in supervisor mode    */
        set_bit(PSRREG, PSR_S);

        /*  adavance the register window    */
        iu_sub_cwp(sparc_state);

        /*  save trapped program counters   */
        REG(L1) = PCREG;
        REG(L2) = NPCREG;

        PCREG = TBRREG;
        NPCREG = TBRREG + 4;
    } else
    {
        if (tt_is_exception(tt))
        {
            /*window overflow or window underflow */
            if (tt == 5 || tt == 6)
            {
                int S;

                tbr_set_tt(sparc_state, tt);

                // supervisor mode is preserved
                S = bit(PSRREG, PSR_S);
                if (S)
                    set_bit(PSRREG, PSR_PS);
                else
                    clear_bit(PSRREG, PSR_PS);

                /*  put the processor in supervisor mode    */
                set_bit(PSRREG, PSR_S);

                /*  adavance the register window    */
                iu_sub_cwp(sparc_state);

                /*  save trapped program counters   */
                REG(L1) = PCREG;
                REG(L2) = NPCREG;

                PCREG = TBRREG;
                NPCREG = TBRREG + 4;
                sparc_state->is_irq = 1;
            }
            //printf("Interrupt while ET disabled, processor halt (PC = 0x%x, NPC = 0x%x)\n", sparc_state->pc, sparc_state->npc);
        } else if (tt != TRAP_IMPL_MAX)
        {
            // it's irq, just ignore it
        }
    }

    if (!tt_is_exception(tt))
        sparc_state->irq_income = 0;

    return SPARC_SUCCESS;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  tbr_set_tt
 *  Description:  This function sets the TT field of the TBR register to the
 *  given value 'tt'
 * =====================================================================================
 */
void tbr_set_tt(void *sparc_state, uint16 tt)
{
    sparc_state_t *state = (sparc_state_t *) sparc_state;
    uint32 tbr = (tt << 4) & TBR_TT_MASK;

    /*  we nned to clear the TT field to ensure that not overlaping is produced
     *  in the OR operation bellow  */
    state->tbr &= TBR_BASE_MASK;
    /*  Update the TBR  */
    state->tbr |= tbr;
    return;
}
