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
* @file stat.c
* @brief 
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2015-02-11
*/

#if HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#else
#include <time.h>
#endif

#include <unistd.h>
#include <stdio.h>
#include "skyeye_stat.h"

static uint64_t prof_timer_start[TOTAL_PROF_TIMER];
static uint64_t prof_timer_total[TOTAL_PROF_TIMER];
void update_timing(int index, bool_t start)
{
    uint64_t usec;

#ifndef __WIN32__
    //#if HAVE_GETRUSAGE
#if 0
    struct rusage r_usage;

    getrusage(RUSAGE_SELF, &r_usage);
    usec = ((uint64_t) r_usage.ru_utime.tv_sec * 1000000) + r_usage.ru_utime.tv_usec;
#else
    //usec = (uint64_t)clock()/CLOCKS_PER_SEC;
    struct timespec ts;

    if (clock_gettime(CLOCK_MONOTONIC, &ts) < 0)
        return 0;
    usec = (uint64_t) ts.tv_sec * 1000000000 + (uint64_t) ts.tv_nsec;

#endif
    printf("in %s, nsec=%lld\n", __FUNCTION__, usec);
    if (True)
        prof_timer_start[index] = usec;
    else
        prof_timer_total[index] += usec - prof_timer_start[index];
#endif
}

uint64_t get_timing(int index)
{
    return prof_timer_total[index];
}
