/*
 * bus_recoder.c - record the activities of bus
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
 * 1/9/2014   zyuming<zyumingfit@gmail.com>
 */

/**
* @brief the handler of serial option
*
* @param this_option
* @param num_params
* @param params[]
*
* @return 
*/

#include "stdlib.h"
#include "skyeye_types.h"
#include "skyeye_class.h"
#include "skyeye_interface.h"
#include "skyeye_serial.h"
#include "skyeye_options.h"
#include "skyeye_config.h"

int do_serial_option(skyeye_option_t * this_option, int num_params, const char *params[])
{
    char name[MAX_PARAM_NAME], value[MAX_PARAM_NAME];
    int i;
    skyeye_config_t *config;

    config = get_current_config();
    for (i = 0; i < num_params; i++)
    {
        if (split_param(params[i], name, value) < 0)
        {
            SKYEYE_ERR("log_info: Error: log has wrong parameter \"%s\".\n", name);
            continue;
        }

        if (!strncmp("obj", name, strlen(name)))
        {
            if (strlen(value) + 1 > MAX_PORT_LEN)
            {
                printf("Obj Name Is Too Long\n");
                return -1;
            }
            memmove(config->serial_parameter[config->serial_count].objname, value, strlen(value) + 1);
        }
        if (!strncmp("brate", name, strlen(name)))
        {
            sscanf(value, "%d", &(config->serial_parameter[config->serial_count].baudrate));
        }
        if (!strncmp("fctrl", name, strlen(name)))
        {
            sscanf(value, "%d", &(config->serial_parameter[config->serial_count].flow_ctrl));
        }
        if (!strncmp("dbits", name, strlen(name)))
        {
            sscanf(value, "%d", &(config->serial_parameter[config->serial_count].data_bits));
        }
        if (!strncmp("sbits", name, strlen(name)))
        {
            sscanf(value, "%d", &(config->serial_parameter[config->serial_count].stop_bits));
        }
        if (!strncmp("parity", name, strlen(name)))
        {
            config->serial_parameter[config->serial_count].parity = *value;
        }
        if (!strncmp("port", name, strlen(name)))
        {
            if (strlen(value) + 1 > MAX_PORT_LEN)
            {
                printf("Port Name Is Too Long\n");
                return -1;
            }
            memmove(config->serial_parameter[config->serial_count].port, value, strlen(value) + 1);
        }
    }
    config->serial_count++;
    return 0;
}

serial_t *get_serial(char *objname)
{
    int i;
    skyeye_config_t *config;

    config = get_current_config();
    for (i = 0; i < config->serial_count; i++)
    {
        if (!strncmp(config->serial_parameter[i].objname, objname, strlen(objname)))
        {
            return &(config->serial_parameter[i]);
        }
    }
    return NULL;
}

int set_serial_connect_flag(char *objname, int flag)
{
    int i;
    skyeye_config_t *config;

    config = get_current_config();
    for (i = 0; i < config->serial_count; i++)
    {
        if (!strncmp(config->serial_parameter[i].objname, objname, strlen(objname)))
        {
            config->serial_parameter[i].connect_flag = flag;

        }
    }
    return config->serial_parameter[i].connect_flag;
}
