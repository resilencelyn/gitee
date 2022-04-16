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
* @file hex.h
* @brief The header of hex format
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-07-31
*/

#ifndef __hex_H__
#define __hex_H__

#include <skyeye_types.h>

// TI's hex v0 file header
typedef struct __attribute__ ((packed))
{
    uint8_t f_magic;                    //magic 
    uint8_t f_datalen;                  //data lenth 
    uint16_t f_addr;                    // address 
    uint8_t f_datatype;                 //data type  
    uint16_t f_data;                    // data 
    uint8_t f_checkflags;               // check flags
} filehex;

bool_t is_hex(const char *filename);
#endif
