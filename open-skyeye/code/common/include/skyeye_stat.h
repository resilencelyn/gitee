/* Copyright (C) 
* 2015 - Michael.Kang blackfin.kang@gmail.com
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
* @file skyeye_stat.h
* @brief 
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2015-02-11
*/
#ifndef __SKYEYE_STAT_H__
#define __SKYEYE_STAT_H__
#include <skyeye_types.h>
typedef enum{
	MODULE_LD_PROF = 0,
	SIM_INIT_PROF,
	SIM_RUN_PROF,
	PARSE_PREP_PROF,
	TOTAL_PROF_TIMER,
}prof_timer_t;

#ifdef __cplusplus
 extern "C" {
#endif

void update_timing(int index, bool_t start);
uint64_t get_timing(int index);

#ifdef __cplusplus
}
#endif

#endif
