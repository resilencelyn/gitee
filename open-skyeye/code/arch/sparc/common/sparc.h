/*
 * =====================================================================================
 *
 *       Filename:  sparc.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  15/04/08 11:42:41
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aitor Viana Sanchez (avs), aitor.viana.sanchez@esa.int
 *        Company:  European Space Agency (ESA-ESTEC)
 *
 * =====================================================================================
 */

#ifndef __SPARC_H__
#define __SPARC_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "types.h"
#ifndef __SPARC_TYPES_H_
#error "arch/sparc/common/types.h shall be included before sparc.h header"
#endif
#include "traps.h"
#include <regformat/sparc_regformat.h>
#include "iu.h"

/**
 *  \class iu_config_t
 *  \brief  This class defines which functions shall be implemented by the Integer
 *  Unit module.
 *   
 *  \par    iu_init_state:  This function shall initialize the integer unit
 *  registering the ISA set, and initialize the traps
 *  \par    iu_cycle_step:  This function executes the next instruction
 *  \par    iu_error_state: This function implements the IU error state
 *  \par    iu_trap_cycle:  This function chekcs whether there is a trap pending
 *  and handles it
 *  \par    iu_set_pc:      This function set the program counter
 *  \par    iu_get_pc:      This function returns the program counter
 */
    typedef struct _iu_config
    {
        void (*iu_init_state) (sparc_state_t * sparc_state);
        void (*iu_reset_state) (conf_object_t * conf_obj);
        int (*iu_cycle_step) (conf_object_t * conf_obj);        /*  returns the number of cycles consumed   */
        void (*iu_error_state) (void);
        int (*iu_trap_cycle) (conf_object_t * conf_obj);
          uint32(*iu_get_pc) (conf_object_t * conf_obj);
        void (*iu_set_pc) (conf_object_t * conf_obj, uint32);
    } iu_config_t;

/** This is the instruction type, in SPARC 32 bits  */
    typedef uint32 sparc_instr;

/*-----------------------------------------------------------------------------
 *  PUBLIC INTERFACE
 *-----------------------------------------------------------------------------*/
    void sparc_register_iu(conf_object_t * conf_obj, iu_config_t * iu_);

#ifdef __cplusplus
}
#endif

#endif
