/* Copyright (C) 
 * 2011 - Michael.Kang blackfin.kang@gmail.com
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
* @file skyeye_mm.c
* @brief the memory management module of SkyEye
* @author Michael.Kang blackfin.kang@gmail.com
* @version 
* @date 2011-04-30
*/

#include <malloc.h>
#include <string.h>
#include "skyeye_mm.h"
#include "asnprintf.h"
#include "skyeye_log.h"

/**
 * this variable is used to test
 * mm_info_head and mm_info_end
 * isSaveMMLog
 */

#ifdef IS_MM_TEST
mm_info_t *mm_info_head = NULL, *mm_info_end = NULL;
int isSaveMMLog;
#endif

/**
* @brief memory allocation function for a given size
*
* @param sz
*
* @return 
*/
void *skyeye_mm_orig(size_t sz, const char *file, const char *func, int line)
{
    //printf("In %s, sz=0x%x\n", __FUNCTION__, sz);
    void *result = NULL;

    result = (unsigned long *) malloc(sz);
#ifdef IS_MM_TEST
    if (result != NULL)
    {
        mm_info_insert("mm", file, func, line, (unsigned long) result, sz);
    }
#endif
    if (result == NULL)
    {
        skyeye_log(Error_log, func, "[%s:%d]malloc faild, sz:0x%x\n", file, line, sz);
    }
    return result;
}

void *skyeye_realloc_orig(void *ptr, size_t size, const char *file, const char *func, int line)
{
    //print f("In %s, sz=0x%x\n", __FUNCTION__, sz);
    void *result = NULL;

    result = (unsigned long *) realloc(ptr, size);

#ifdef IS_MM_TEST
    if (result != NULL)
    {
        mm_info_t *p = mm_info_head;
        int k = 0;

        while (p != NULL)
        {
            if (p->mm == (unsigned long) ptr)
            {
                //Pointer addr no change.
                k = 1;
                break;
            }
            p = p->next;
        }

        if (k == 1)
        {
            if (ptr != result)
            {
                p->mm = (unsigned long) result;
                p->sz = size;
            }
        } else
        {
            mm_info_insert("realloc", file, func, line, (unsigned long) result, size);
        }
    }
#endif
    if (result == NULL)
    {
        skyeye_log(Error_log, func, "[%s:%d]malloc faild, sz:0x%x\n", file, line, size);
    }
    return result;
}

/**
* @brief allocate a zeroed memory
*
* @param sz
*
* @return 
*/
void *skyeye_mm_zero_orig(size_t sz, const char *file, const char *func, int line)
{
    void *result = NULL;

    result = malloc(sz);
    if (result != NULL)
    {
        memset(result, 0, sz);
#ifdef IS_MM_TEST
        mm_info_insert("mm_zero", file, func, line, (unsigned long) result, sz);
#endif
    }
    if (result == NULL)
    {
        skyeye_log(Error_log, func, "[%s:%d]malloc faild, sz:0x%x\n", file, line, sz);
    }
    return result;
}

void *skyeye_mm_align_orig(size_t align, size_t sz, const char *file, const char *func, int line)
{
    void *result = NULL;

#ifndef __MINGW32__
    result = memalign(align, sz);
#else
    result = __mingw_aligned_malloc(sz, align);
#endif
#ifdef IS_MM_TEST
    if (result != NULL)
    {
        mm_info_insert("mm_align", file, func, line, (unsigned long) result, sz);
    }
#endif
    if (result == NULL)
    {
        skyeye_log(Error_log, func, "[%s:%d]malloc faild, sz:0x%x\n", file, line, sz);
    }
    return result;
}

void skyeye_align_free_orig(void *p, const char *file, const char *func, int line)
{
    if (p != NULL)
    {
#ifndef __MINGW32__
        free(p);
#else
        __mingw_aligned_free(p);
#endif
#ifdef IS_MM_TEST
        mm_info_insert("align_free", file, func, line, (unsigned long) p, get_mm_size(p));
#endif
    }
    p = NULL;
}

/**
* @brief duplicate a string
*
* @param s
*
* @return 
*/
char *skyeye_strdup_orig(const char *s, const char *file, const char *func, int line)
{
    char *result = NULL;

    result = strdup(s);
#ifdef IS_MM_TEST
    if (result != NULL)
    {
        mm_info_insert("strdup", file, func, line, (unsigned long) result, strlen(s));
    }
#endif
    return result;
}

/**
* @brief free the allocated memory
*
* @param p
*/
void skyeye_free_orig(void *p, const char *file, const char *func, int line)
{
    if (p != NULL)
    {
#ifdef IS_MM_TEST
        mm_info_insert("free", file, func, line, (unsigned long) p, get_mm_size(p));
#endif
        free(p);
    }
    p = NULL;
}

#ifdef IS_MM_TEST

/**
* @insert a content
*/
void mm_info_insert(const char *type, const char *file, const char *func, int line, unsigned long mm, size_t sz)
{
    mm_info_t *p = NULL;

    p = (mm_info_t *) malloc(sizeof (mm_info_t));

    p->type = strdup(type);
    p->file = strdup(file);
    p->func = strdup(func);
    p->line = line;
    p->mm = mm;
    p->sz = sz;
    p->next = NULL;

    if (mm_info_head == NULL)
    {
        p->id = 1;
        mm_info_head = p;
    } else
    {
        p->id = mm_info_end->id + 1;
        mm_info_end->next = p;
    }
    mm_info_end = p;
}

/**
* @to find whether have this memory address.
*
* @param d
*
* @return
*/
int mm_info_search(mm_info_t * d)
{
    mm_info_t *p = mm_info_head;

    while (p != NULL)
    {
        if (p->mm == d->mm && p->id != d->id)
        {
            return 1;
        }
        p = p->next;
    }
    return 0;
}

/**
* @to find the memory address the size_t
*
* @param p
*
* @return
*/
size_t get_mm_size(void *p)
{
    mm_info_t *ptr = mm_info_head;

    while (ptr != NULL)
    {
        if (ptr->mm == (unsigned long) p)
        {
            return ptr->sz;
        }
        ptr = ptr->next;
    }
    // not fonud return -1
    return -1;
}

/**
* @print the memory information
*
* @param args
*/
void mm_info_print(char *args)
{
    mm_info_t *p = mm_info_head;
    int mm_n = 0, mm_zero_n = 0, strdup_n = 0;
    int mm_align_n = 0, align_free_n = 0, free_n = 0;
    int realloc_n = 0;
    int mm_sz = 0, mm_zero_sz = 0, strdup_sz = 0;
    int mm_align_sz = 0, align_free_sz = 0, free_sz = 0;
    int realloc_sz = 0;
    int mm_total = 0, mm_free = 0;
    int argslen = 0;

    argslen = strlen(args);
    while (p != NULL)
    {
        if (!strcmp(p->type, "mm"))
        {
            mm_n++;
            mm_sz += p->sz;
        }
        if (!strcmp(p->type, "mm_zero"))
        {
            mm_zero_n++;
            mm_zero_sz += p->sz;
        }
        if (!strcmp(p->type, "strdup"))
        {
            strdup_n++;
            strdup_sz += p->sz;
        }
        if (!strcmp(p->type, "mm_align"))
        {
            mm_align_n++;
            mm_align_sz += p->sz;
        }
        if (!strcmp(p->type, "align_free"))
        {
            align_free_n++;
            align_free_sz += p->sz;
        }
        if (!strcmp(p->type, "free"))
        {
            free_n++;
            free_sz += p->sz;
        }
        if (!strcmp(p->type, "realloc"))
        {
            realloc_n++;
            realloc_sz += p->sz;
        }

        if (argslen == 0)
        {
            printf("ID:%-4ld Type: %-10s %-35s %-25s L %-5ld %#-14lx sz: %ld\n",
                   p->id, p->type, p->file, p->func, p->line, p->mm, p->sz);
        } else if (!strcmp(args, "mm") || !strcmp(args, "mm_zero") || !strcmp(args, "strdup") ||
                   !strcmp(args, "mm_align") || !strcmp(args, "align_free") || !strcmp(args, "free")
                   || !strcmp(args, "realloc"))
        {
            if (!strcmp(args, p->type))
            {
                printf("ID:%-4ld Type: %-10s %-35s %-25s L %-5ld %#-14lx sz: %ld\n",
                       p->id, p->type, p->file, p->func, p->line, p->mm, p->sz);
            }
        } else
        {
            printf("Do not have this function.\n");
            return;
        }
        p = p->next;
    }
    mm_total = mm_sz + mm_zero_sz + strdup_sz + mm_align_sz + realloc_sz;
    mm_free = free_sz + align_free_sz;
    printf("==============================================================\n"
           "mm         %-4d sz:%d\n"
           "mm_zero    %-4d sz:%d\n"
           "strdup     %-4d sz:%d\n"
           "mm_align   %-4d sz:%d\n"
           "align_free %-4d sz:%d\n"
           "free       %-4d sz:%d\n"
           "realloc    %-4d sz:%d\n\n"
           "mm_total:  %d\n"
           "mm_free:   %d\n"
           "==============================================================\n",
           mm_n, mm_sz, mm_zero_n, mm_zero_sz, strdup_n, strdup_sz,
           mm_align_n, mm_align_sz, align_free_n, align_free_sz, free_n,
           free_sz, realloc_n, realloc_sz, mm_total, mm_free);
}

/**
* @Save memory operation log[result]
*/
void skyeye_save_mm_result()
{
    int n = 0;
    mm_info_t *p = mm_info_head;
    FILE *fp;
    char *str = NULL;
    int ret;

    if (isSaveMMLog)
    {
        if ((fp = fopen(MM_TEST_LOG_RESULT, "w+")) != NULL)
        {
            while (p != NULL)
            {
                if (!mm_info_search(p))
                {
                    //Save mm result log
                    ret = asnprintf(&str, 1024, "ID:%-4ld Type: %-10s %-30s %-25s L %-5ld %#-14lx sz: %ld\n",
                                    p->id, p->type, p->file, p->func, p->line, p->mm, p->sz);
                    if (ret == -1)
                    {
                        skyeye_log(Error_log, __FUNCTION__, "asnprintf() source string beyond the specified size");
                    }
                    fseek(fp, 0, SEEK_END);
                    fwrite(str, strlen(str), 1, fp);
                }
                p = p->next;
            }
        }
        fclose(fp);
    }
}

/**
* @Save memory operation log[All]
*/
void skyeye_save_mm_info()
{
    mm_info_t *p = mm_info_head;
    int mm_n = 0, mm_zero_n = 0, strdup_n = 0;
    int mm_align_n = 0, align_free_n = 0, free_n = 0;
    int realloc_n = 0;
    int mm_sz = 0, mm_zero_sz = 0, strdup_sz = 0;
    int mm_align_sz = 0, align_free_sz = 0, free_sz = 0;
    int realloc_sz = 0;
    int mm_total = 0, mm_free = 0;
    FILE *fp;
    char *str = NULL;
    int ret;

    if (isSaveMMLog)
    {
        if ((fp = fopen(MM_TEST_LOG_INFO, "w+")) != NULL)
        {
            while (p != NULL)
            {
                if (!strcmp(p->type, "mm"))
                {
                    mm_n++;
                    mm_sz += p->sz;
                }
                if (!strcmp(p->type, "mm_zero"))
                {
                    mm_zero_n++;
                    mm_zero_sz += p->sz;
                }
                if (!strcmp(p->type, "strdup"))
                {
                    strdup_n++;
                    strdup_sz += p->sz;
                }
                if (!strcmp(p->type, "mm_align"))
                {
                    mm_align_n++;
                    mm_align_sz += p->sz;
                }
                if (!strcmp(p->type, "align_free"))
                {
                    align_free_n++;
                    align_free_sz += p->sz;
                }
                if (!strcmp(p->type, "free"))
                {
                    free_n++;
                    free_sz += p->sz;
                }
                if (!strcmp(p->type, "realloc"))
                {
                    realloc_n++;
                    realloc_sz += p->sz;
                }
                //Save mm info log
                ret = asnprintf(&str, 1024, "ID:%-4ld Type: %-10s %-35s %-25s L %-5ld %#-14lx sz: %ld\n",
                                p->id, p->type, p->file, p->func, p->line, p->mm, p->sz);
                if (ret == -1)
                {
                    skyeye_log(Error_log, __FUNCTION__, "asnprintf() source string beyond the specified size");
                }
                fseek(fp, 0, SEEK_END);
                fwrite(str, strlen(str), 1, fp);
                p = p->next;
            }
            mm_total = mm_sz + mm_zero_sz + strdup_sz + mm_align_sz + realloc_sz;
            mm_free = free_sz + align_free_sz;
            ret = asnprintf(&str, 1024,
                            "==============================================================\n"
                            "mm         %-4d sz:%d\n"
                            "mm_zero    %-4d sz:%d\n"
                            "strdup     %-4d sz:%d\n"
                            "mm_align   %-4d sz:%d\n"
                            "align_free %-4d sz:%d\n"
                            "free       %-4d sz:%d\n"
                            "realloc    %-4d sz:%d\n\n"
                            "mm_total:  %d\n"
                            "mm_free:   %d\n"
                            "leak:      %d\n"
                            "==============================================================\n",
                            mm_n, mm_sz, mm_zero_n, mm_zero_sz, strdup_n, strdup_sz,
                            mm_align_n, mm_align_sz, align_free_n, align_free_sz, free_n,
                            free_sz, realloc_n, realloc_sz, mm_total, mm_free, mm_total - mm_free);
            if (ret == -1)
            {
                skyeye_log(Error_log, __FUNCTION__, "asnprintf() source string beyond the specified size");
            }
            fseek(fp, 0, SEEK_END);
            fwrite(str, strlen(str), 1, fp);
        }
        fclose(fp);
    }
}

/**
* @set whether save the log
*
* @param k
*/
void set_mm_save_log(int k)
{
    if (k == 1)
    {
        isSaveMMLog = 1;
    } else if (k == 0)
    {
        isSaveMMLog = 0;
    }
}

#endif //IS_MM_TEST
