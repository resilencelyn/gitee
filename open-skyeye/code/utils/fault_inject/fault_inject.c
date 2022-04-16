/*
 * fault_inject.c - record the bus access
 * Copyright (C) 2003-2007 Skyeye Develop Group
 * for help please send mail to <skyeye-developer@lists.sf.linuxforum.net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */
/*
 * 01/02/2010   Michael.Kang  <blackfin.kang@gmail.com>
 */
#include <stdlib.h>
#include <stdio.h>
#include "skyeye_arch.h"
#include "skyeye_callback.h"
#include "skyeye_command.h"
#include "skyeye_cli.h"
#include "skyeye_bus.h"

typedef enum
{
    RANDOM = 0,
    FIXED
} fj_mode_t;

long int random(void);

/**
* @brief the mode of fault injection
*/
static fj_mode_t fj_mode = RANDOM;

/**
* @brief the address of fault injection
*/
static generic_address_t fj_addr;
static int fj_reverse_bit;
static int enable_fault_inject = 0;
int fj_bus_read(short size, generic_address_t addr, uint32_t * value)
{
    default_bus_read(size, addr, value);
    if (enable_fault_inject && addr == fj_addr)
    {
        uint32_t tmp = *value;

        if (fj_mode == FIXED)
        {
            if ((tmp >> fj_reverse_bit) & 0x1)
            {
                tmp &= ~(1 << fj_reverse_bit);
            } else
            {
                tmp |= (1 << fj_reverse_bit);
            }
        } else
        {
            /* random condition */
            if ((fj_mode == RANDOM) && (random() % 4))
            {
                if ((tmp >> fj_reverse_bit) & 0x1)
                {
                    tmp &= ~(1 << fj_reverse_bit);
                } else
                {
                    tmp |= (1 << fj_reverse_bit);
                }
            }
        }
        printf("In %s, fault inject enabled, old value=0x%x, and new value=0x%x\n", __FUNCTION__, *value, tmp);
        *value = tmp;
    }
    return 0;
}

int fj_bus_write(short size, generic_address_t addr, uint32_t value)
{
    default_bus_write(size, addr, value);
    return 0;
}

int com_fault_inject(char *arg)
{
    int flag = 0;
    char result[64];

    if (arg == NULL || *arg == '\0')
        return Invarg_exp;
    else
    {
        flag = get_parameter(result, arg, "addr");
        if (flag > 0)
            fj_addr = strtoul(result, NULL, 0);
        else
        {
            printf("Format error for addr\n");
            return Invarg_exp;
        }
        flag = get_parameter(result, arg, "mode");
        if (flag > 0)
            fj_mode = strtoul(result, NULL, 0);
        else
        {
            printf("Format error: x addr=xxxx size=xxxx\n");
            return Invarg_exp;
        }
        flag = get_parameter(result, arg, "reverse_bit");
        if (flag > 0)
            fj_reverse_bit = strtoul(result, NULL, 0);
        else
        {
            printf("Format error: x addr=xxxx size=xxxx\n");
            return Invarg_exp;
        }
        enable_fault_inject = 1;
        printf("In %s, fault_inject enabled, addr=0x%x, mode=%d, reserve_bit=%d\n", __FUNCTION__, fj_addr, fj_mode,
               fj_reverse_bit);
    }
    return No_exp;
}

/* some initialization for log functionality */
exception_t fault_inject_init()
{
    exception_t ret = No_exp;

    /* register callback function */
    /* add corresponding command */
    add_command("fault_inject", com_fault_inject, "Inject some fixed or random fault for bus_read.\n");

    return ret;
}

/* destruction function for log functionality */
exception_t fault_inject_fini()
{
    return No_exp;
}
