/* Copyright (C)
 * 2011 - Michael.kang
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

/**
* @file breakpoint.c
* @brief The breakpoint management and implementation.
* @author Michael.kang
* @version
* @date 2011-04-28
*/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "skyeye_core.h"
#include "breakpoint.h"
#include "skyeye_types.h"
#include "skyeye_callback.h"
#include "skyeye_command.h"
#include "sim_control.h"
#include <skyeye_addr_space.h>
#include <skyeye_obj.h>
#include <skyeye_interface.h>
#include <skyeye_core_intf.h>

/**
* @brief Get a breakpoint by its id
*
* @param id
*
* @return a breakpoint instance
*/

int print_bp_info(breakpoint_mgt_t * break_manager)
{
    int i, num, id, hits;
    generic_address_t addr;
    char OutPutBuf[128] = { '\0' };
    breakpoint_t *bp;

    printf("=========================================\n");
    printf("%5s%20s%20s\n", "ID", "Address", "Hits");
    num = get_bp_numbers(break_manager);
    for (i = 0; i < num; i++)
    {
        bp = &break_manager->breakpoint[i];
        id = bp->id;
        addr = bp->address;
        hits = bp->hits;
        sprintf(OutPutBuf, "%5d%20x%20d\n", id, addr, hits);
        printf("%s", OutPutBuf);
    }
    printf("=========================================\n");
    return 0;
}

breakpoint_t *get_bp_by_id(breakpoint_mgt_t * break_manager, breakpoint_id_t id)
{
    /* scan the breakpoint if the breakpoint exists */
    int i;

    for (i = 0; i < break_manager->bp_number; i++)
    {
        if (break_manager->breakpoint[i].id == id)
            return &break_manager->breakpoint[i];
    }
    return NULL;
}

/**
* @brief if the breakpoint is valid
*
* @param bp
*
* @return
*/
static bool_t valid_bp(breakpoint_t * bp)
{
    if (bp->id != 0)
        return True;
    else
        return False;
}

/**
* @brief get a breakpoint by its address
*
* @param addr
*
* @return
*/
breakpoint_t *get_bp_by_addr(breakpoint_mgt_t * break_manager, generic_address_t addr)
{
    int i;

    /* scan the breakpoint if the breakpoint exists */
    for (i = 0; i < break_manager->bp_number; i++)
    {
        if (break_manager->breakpoint[i].address == addr)
            return &break_manager->breakpoint[i];
    }
    return NULL;
}

/**
* @brief insert a breakpoint at an address
*
* @param access_type
* @param address_type
* @param addr
*
* @return
*/
int insert_bp(breakpoint_mgt_t * break_manager, access_t access_type, breakpoint_kind_t address_type,
              generic_address_t addr)
{
    breakpoint_t *bp;

    if (break_manager->bp_number == MAX_BP_NUMBER)
        return -1;

    bp = get_bp_by_addr(break_manager, addr);
    if (bp != NULL)
        return 0; // When breakpoint already exists

    /* we insert the new bp to the last position. And increase the bp_number */
    bp = &break_manager->breakpoint[break_manager->bp_number];
    bp->address = addr;
    bp->stop_address = addr - 4;
    bp->stop_address = addr;
    bp->access_type = access_type;
    bp->address_type = address_type;
    bp->skiped = 0;
    bp->hits = 0;

    bp->id = break_manager->bp_number;
    break_manager->bp_number++;
    //bp->id = break_manager->bp_number;
    return 0;
}

/**
* @brief remove a breakpoint by id
*
* @param id
*
* @return
*/
int remove_bp(breakpoint_mgt_t * break_manager, breakpoint_id_t id)
{
    int i;
    breakpoint_t *bp;

    bp = get_bp_by_id(break_manager, id);
    uint32_t addr = bp->address;

    if (bp)
    {
        //memcpy(break_manager->breakpoint + id - 1, break_manager->breakpoint + id, sizeof(breakpoint_t) * (break_manager->bp_number - id-1));
        int i = 0;

        for (i = id; i < break_manager->bp_number - 1; i++)
        {
            memcpy(break_manager->breakpoint + i, break_manager->breakpoint + i + 1, sizeof (breakpoint_t));
            break_manager->breakpoint[i].id -= 1;
        }
        break_manager->bp_number--;
        printf("Remove breakpoint at address 0x%x successfully.\n", addr);
        fflush(stdout);
        return 0;
    }

    return -1;
}

/**
* @brief remove a breakpoint at the address
*
* @param addr
*
* @return
*/
int remove_bp_by_addr(breakpoint_mgt_t * break_manager, generic_address_t addr)
{
    breakpoint_t *bp = get_bp_by_addr(break_manager, addr);

    if (bp != NULL)
        return remove_bp(break_manager, bp->id);
    else
        return -1;
}

int init_breakpoint_manager(breakpoint_mgt_t * break_manager)
{
    break_manager->bp_number = 0;
    break_manager->breakpoint_hit = NULL;
    break_manager->last_bp_step = 0;
    return 0;
}

int create_a_breakpoint(breakpoint_mgt_t * break_manager, generic_address_t addr)
{
    int ret = insert_bp(break_manager, SIM_access_execute, SIM_Break_Virtual, addr);

    if (ret != 0)
    {
        printf("Can not insert breakpoint at address 0x%x\n", addr);
        return -1;
    }
    printf("Insert breakpoint at address 0x%x successfully.\n", addr);
    fflush(stdout);
    return 0;
}

int get_bp_numbers(breakpoint_mgt_t * break_manager)
{
    return break_manager->bp_number;
}

breakpoint_t *get_first_bp()
{
    return NULL;
}

breakpoint_t *get_next_bp(breakpoint_t * bp)
{
    return NULL;
}

int clear_breakpoint_hit_flag(breakpoint_mgt_t * break_manager)
{
    break_manager->breakpoint_hit = NULL;
    return 0;
}

int check_breakpoint_hit_flag(breakpoint_mgt_t * break_manager)
{
    if (break_manager->breakpoint_hit == NULL)
        return -1;
    return 0;
}

int get_breakpoint_hit_id(breakpoint_mgt_t * break_manager)
{
    int id;

    if (break_manager->breakpoint_hit != NULL)
    {
        id = break_manager->breakpoint_hit->id;
        return id;
    }
    return -1;
}

int check_breakpoint(breakpoint_mgt_t * break_manager, generic_address_t pc, uint64 steps)
{
    int i;

    for (i = 0; i < break_manager->bp_number; i++)
    {
        breakpoint_t *bp = &break_manager->breakpoint[i];

        if ((bp->stop_address == pc && steps > break_manager->last_bp_step))
        {
            bp->hits++;
            bp->skiped = 1;             // align with GDB-style breakpoint
            break_manager->breakpoint_hit = bp;
            break_manager->last_bp_step = steps;
            printf("The %d# breakpoint at address 0x%x is hit, %d.\n", bp->id, bp->address, bp->hits);
            fflush(stdout);
            SIM_stop(NULL);
            return 1;
        }
    }
    return 0;
}

//only do sploop breakpoint check
int check_pc_hit(breakpoint_mgt_t * break_manager, generic_address_t pc, uint64 steps)
{
    int i;

    for (i = 0; i < break_manager->bp_number; i++)
    {
        breakpoint_t *bp = &break_manager->breakpoint[i];

        if ((bp->stop_address == pc && steps > break_manager->last_bp_step))
        {
            bp->hits++;
            bp->skiped = 1;             // align with GDB-style breakpoint
            break_manager->breakpoint_hit = bp;
            return 1;
        }
    }
    return 0;
}

void iterate_breakpoints(breakpoint_mgt_t * break_manager, breakpoint_iterator_fn fn, void *arg)
{
    int i;

    for (i = 0; i < break_manager->bp_number; i++)
    {
        breakpoint_t *bp = &break_manager->breakpoint[i];

        fn(bp, arg);

    }
}

/**
* @brief Initilization of breakpoint
*
* @return
*/
int init_bp()
{
    exception_t exp;

    //register_callback(check_breakpoint_callback, Step_callback);
    //add_command("break", com_break, "set breakpoint for an address.\n");
    //add_command("list-bp", com_list_bp, "List all the breakpoint.\n");
    //add_command("delete-bp", com_delete_bp, "List all the breakpoint.\n");
    //register_info_command("breakpoint", com_list_bp, "List all the breakpoint.\n");
    return No_exp;
}
