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
* @file file_copy.cpp
* @brief 
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2015-07-01
*/

#include <stdio.h>
#include <stdlib.h>
#include <skyeye.h>

exception_t file_copy(const char *filename_src, const char *filename_dst)
{

    FILE *fp_src = 0;
    FILE *fp_dst = 0;
    char buffer[BUFSIZ];
    size_t numread = 0;
    size_t numwritten = 0;

    if ((fp_src = fopen(filename_src, "r")) == NULL)
    {
        //printf( "The file %s was not opened\n", filename_src );
        //return File_open_exp;
    } else
    {
        //printf( "The file %s was opened\n", filename_src );
    }

    if ((fp_dst = fopen(filename_dst, "w+")) == NULL)
    {
        //printf( "The file %s was not opened\n", filename_dst );
        return File_open_exp;
    } else
    {
        //printf( "The file %s was opened\n", filename_dst );
    }

    while ((numread = fread(buffer, sizeof (char), BUFSIZ, fp_src)))
    {
        if (numread == 0)
            break;

        if ((numwritten = fwrite(buffer, sizeof (char), numread, fp_dst)) != numread)
            return Excess_range_exp;
    }

    fclose(fp_src);
    fclose(fp_dst);
    return No_exp;
}
