/*
 * loader_file.c - load a file to memory. 
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
 * 06/22/2009   Michael.Kang  <blackfin.kang@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "skyeye_log.h"
#include "skyeye_loader.h"
#include "bank_defs.h"
#include "skyeye_ram.h"
#include "skyeye_iface.h"

/**
* @brief load a block of data to the memory
*
* @param src
* @param len
* @param load_addr
*
* @return 
*/
exception_t load_data(void *src, size_t len, generic_address_t load_addr)
{
    char *p = src;
    generic_address_t addr = load_addr;
    int i = 0;

    while (i < len)
    {
        //skyeye_log(Debug_log, __FUNCTION__, "data=0x%x,addr=0x%x\n", *(p + i), addr);
        bus_write(8, addr, *(p + i));
        addr++;
        i++;
    }
    return No_exp;
}

/**
* @brief Load a file to guest memory
*
* @param filename
* @param load_addr
*
* @return 
*/
exception_t load_file(conf_object_t * obj, const char *filename, generic_address_t load_addr)
{
    int ret;
    FILE *f;
    uint8 data;
    int nread = 0;
    generic_address_t addr = load_addr;
    struct stat stat_buf;

    if (stat(filename, &stat_buf) != 0)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not stat file %s.\n", filename);
        return File_open_exp;
    }

    memory_space_intf *iface = (memory_space_intf *) SKY_get_iface(obj, MEMORY_SPACE_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get memory_space interface from %s error!!!\n", obj->objname);
        return Not_found_exp;
    }
    //addr_space_t* space = get_space_from_attr(obj);
    //if(space != NULL){
    //      skyeye_log(Error_log, __FUNCTION__, "Can not get space from core:%s\n", obj->objname);

    f = fopen(filename, "rb");
    if (f == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not open file %s.\n", filename);
        return File_open_exp;
    }

    /* read a char and write it to the memory */
    while (nread = fread(&data, 1, 1, f))
    {
        ret = iface->write(obj, addr, &data, 1);
        if (ret != No_exp)
        {
            /* error handler for address error */
            fclose(f);
            return Excess_range_exp;
        }
        addr++;
    }
    skyeye_log(Info_log, __FUNCTION__, "Load the file %s to the memory 0x%x\n", filename, load_addr);
    fclose(f);
    return No_exp;
}
