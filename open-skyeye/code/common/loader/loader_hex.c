/* Copyright (C) 
 * 2013 - Michael.Kang blackfin.kang@gmail.com
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
* @file loader_hex.c
* @brief load the hex executable file
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-07-31
*/

#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "portable/mman.h"
#include "bank_defs.h"
#include "skyeye_pref.h"
#include "skyeye_config.h"
#include "skyeye_ram.h"
#include "hex.h"

bool_t is_hex(const char *filename)
{
    filehex header;
    bool_t ret = False;
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        fprintf(stderr, "In %s, can not open file %s\n", __FUNCTION__, filename);
        exit(-1);
    }

    if (fread(&header, sizeof (filehex), 1, file) != 1)
    {
#if __WIN32__
        fprintf(stderr, "Workaround for windows fopen function in %s\n", __FUNCTION__);
#else
        goto out;
#endif
    }
    if (header.f_magic != 0x3a)
        ret = False;
    else
        ret = True;
  out:
    fclose(file);
    return ret;
}
