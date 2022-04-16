/* Copyright (C) 
* 2014 - Michael.Kang blackfin.kang@gmail.com
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
* @file skyeye_sim.h
* @brief The class for golbal state of simulator
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2014-03-18
*/
#ifndef __SKYEYE_SIM_H__
#define __SKYEYE_SIM_H__
typedef struct skyeye_sim{
	char* checkpoint_path;
	int default_log_level;
	int gui_mode;
	char* host_arch;
	int host_num_cpus;
	char* host_os;
	int host_phys_mem;
}skyeye_sim_t;
#endif
