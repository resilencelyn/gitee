/*
	skyeye.h - necessary definition for skyeye
	Copyright (C) 2003 Skyeye Develop Group
	for help please send mail to <skyeye-developer@lists.sf.linuxforum.net> 
	
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 
*/
/*
 * 1/9/2014	
 * 	   Parameters of serial to connect to PC serial port
 *	
 * */

#ifndef __SERIAL__
#define __SERIAL__

#include <skyeye_options.h>
#define MAX_PORT_LEN 128
#define MAX_OBJ_NAME 128

typedef struct serial_parameters{
	char objname[MAX_OBJ_NAME];
	int baudrate;
	int flow_ctrl;
	int data_bits;
	int stop_bits;
	char parity;
	char port[MAX_PORT_LEN]; 
	int connect_flag;
}serial_t;


int do_serial_option (skyeye_option_t * this_option, int num_params, const char *params[]);

serial_t *get_serial(char *objname);
int set_serial_connect_flag(char *objname, int flag);
#endif
