/* Copyright (C)
* 
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
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.  * 
*/
/**
* @file timer_6713.h
* @brief The definition of system controller 
* @author 
* @version 78.77
*/

/* Autogenerated by SkyEye script */
#ifndef __timer_s3c6410_H__
#define __timer_s3c6410_H__

#include <skyeye_lock.h>
#define PWM_CTL_BASE    (0x51000000)
#define PWM_CTL_SIZE    (0x44)

#define TCFG0           (0x0)
#define TCFG1           (0x4)
#define TCON            (0x8)
#define TCNTB0          (0xc)
#define TCMPB0          (0x10)
#define TCNTO0          (0x14)
#define TCNTB1          (0x18)
#define TCMPB1          (0x1c)
#define TCNTO1          (0x20)
#define TCNTB2          (0x24)
#define TCMPB2          (0x28)
#define TCNTO2          (0x2c)
#define TCNTB3          (0x30)
#define TCMPB3          (0x34)
#define TCNTO3          (0x38)
#define TCNTB4          (0x3c)
#define TCNTO4          (0x40)
#define TINT_CSTAT      (0x44) /*Timer interrupt Control and Status Register*/
#define S3C6410_TIMER_NUM 5

typedef struct s3c6410x_timer{
	conf_object_t *obj;
	struct registers{
		uint32_t tcfg0;
		uint32_t tcfg1;
		uint32_t tcon;
		int tcnt[S3C6410_TIMER_NUM];
		uint32_t tcmp[S3C6410_TIMER_NUM];
		int tcntb[S3C6410_TIMER_NUM];
		uint32_t tcmpb[S3C6410_TIMER_NUM];
		uint32_t tcnto[S3C6410_TIMER_NUM];
		/* shenoubang 2012-4-19 */
		uint32_t tint_cstat;
	}regs;
	int scheduler_id;
	general_signal_intf *signal_iface;
	conf_object_t *signal;
	int interrupt_number;
	RWLOCK_T lock;
}timer_s3c6410_device;

static char** regs_name[] = {
	"tcfg0",
	"tcfg1",
	"tcon",
	"tcnt[0]",
	"tcnt[1]",
	"tcnt[2]",
	"tcnt[3]",
	"tcnt[4]",
	"tcmp[0]",
	"tcmp[1]",
	"tcmp[2]",
	"tcmp[3]",
	"tcmp[4]",
	"tcntb[0]",
	"tcntb[1]",
	"tcntb[2]",
	"tcntb[3]",
	"tcntb[4]",
	"tcmpb[0]",
	"tcmpb[1]",
	"tcmpb[2]",
	"tcmpb[3]",
	"tcmpb[4]",
	"tcnto[0]",
	"tcnto[1]",
	"tcnto[2]",
	"tcnto[3]",
	"tcnto[4]",
	"tint_cstat",
	 NULL
};
#endif