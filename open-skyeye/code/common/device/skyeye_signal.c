/*
 * skyeye_signal.c - implement the signal framework for skyeye
 * Copyright (C) 2010 Skyeye Develop Group
 * for help please send mail to <skyeye-developer@lists.gro.clinux.org>
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
 *      2010-08-15 Michael.Kang<blackfin.kang@gmail.com>
 */
#include "skyeye_types.h"
#include "skyeye_core_intf.h"
#include "skyeye_signal.h"
#include "skyeye_interface.h"
#include "skyeye_iface.h"

/**
* @brief send interrupt signal to the processor
*
* @param signal
*
* @return 
*/
exception_t send_signal(conf_object_t * conf_obj, interrupt_signal_t * signal)
{
    //generic_arch_t* arch_instance = get_arch_instance("");
    //arch_instance->signal(signal);
    core_signal_intf *core_signal = (core_signal_intf *) SKY_get_iface(conf_obj, CORE_SIGNAL_INTF_NAME);

    core_signal->signal(conf_obj, signal);
    return No_exp;
}
