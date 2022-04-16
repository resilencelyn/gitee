/**
* Copyright (c), 2015-2025
* @file fifo.c
* @brief fifo主文件
* @author jdh
* @verbatim
* Change Logs:
* Date           Author       Notes
* 2018-01-24     jdh          新建
* 2018-11-06     jdh          修改成通用格式
* @endverbatim
*/

#ifndef _MY_FIFO_H_
#define _MY_FIFO_H_

#include "math.h"
#include "stdarg.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

/**
* @brief fifo结构
*/

typedef struct
{
    int ptr_write;
    int ptr_read;
    bool is_full;

    // fifo中存储的元素数,不是字节大小
    int item_sum;
    // 元素大小.单位: 字节
    int item_size;
    void *fifo_ptr;
} my_fifo;

/**
* @brief 创建fifo
* @param item_sum：fifo中元素数.注意不是字节数
* @param item_size: 元素大小.单位: 字节
* @return fifo索引
*/

int fifo_create(int item_sum, int item_size)
{
    my_fifo *fifo = (my_fifo *)malloc(sizeof(my_fifo));
    fifo->item_sum = item_sum;
    fifo->item_size = item_size;
    fifo->ptr_write = 0;
    fifo->ptr_read = 0;
    fifo->is_full = false;
    fifo->fifo_ptr = (void *)malloc(item_sum * item_size);
    return (int)fifo;
}

/**
* @brief 删除fifo
* @param fifo_index: fifo索引
*/

void fifo_delete(int fifo_index)
{
    my_fifo *fifo = (my_fifo *)fifo_index;
    free(fifo->fifo_ptr);
    fifo->fifo_ptr = NULL;
    free(fifo);
    fifo = NULL;
}

/**
* @brief fifo可读的元素数
* @param fifo_index: fifo索引
* @return 元素数
*/

int fifo_readable_item_count(int fifo_index)
{
    my_fifo *fifo = (my_fifo *)fifo_index;
    if (fifo->is_full)
    {
        return fifo->item_sum;
    }
    else
    {
        return (fifo->item_sum + fifo->ptr_write - fifo->ptr_read) % fifo->item_sum;
    }
}

/**
* @brief fifo可写的元素数
* @param fifo_index: fifo索引
* @return 元素数
*/

int fifo_writeable_item_count(int fifo_index)
{
    my_fifo *fifo = (my_fifo *)fifo_index;
    if (fifo->is_full)
    {
        return 0;
    }
    else
    {
        if (fifo->ptr_write == fifo->ptr_read)
        {
            return fifo->item_sum;
        }
        else
        {
            return (fifo->item_sum + fifo->ptr_read - fifo->ptr_write) % fifo->item_sum;
        }
    }
}

/**
* @brief fifo检查是否可以写入
* @param fifo_index: fifo索引
* @retval false:不可以写入.true:可以写入
*/

bool fifo_writeable(int fifo_index)
{
    my_fifo *fifo = (my_fifo *)fifo_index;
    return !fifo->is_full;
}

/**
* @brief fifo写入
* @param fifo_index: fifo索引
* @param frame:写入元素指针
* @return false:失败.true:成功
*/

bool fifo_write(int fifo_index, void *data)
{
    my_fifo *fifo = (my_fifo *)fifo_index;
    if (fifo->is_full)
    {
        return false;
    }

    memcpy((char *)(fifo->fifo_ptr) + fifo->ptr_write * fifo->item_size, data, fifo->item_size);
    fifo->ptr_write++;

    if (fifo->ptr_write >= fifo->item_sum)
    {
        fifo->ptr_write = 0;
    }
    if (fifo->ptr_write == fifo->ptr_read)
    {
        fifo->is_full = true;
    }

    return true;
}

/**
* @brief fifo批量写入
* @param fifo_index: fifo索引
* @param data: 写入元素指针
* @param item_num：写入的元素数目
* @return false:失败.true:成功
*/

bool fifo_write_batch(int fifo_index, void *data, int item_num)
{
    my_fifo *fifo = (my_fifo *)fifo_index;
    if (fifo_writeable_item_count((int)fifo) < item_num)
    {
        return false;
    }

    memcpy((char *)(fifo->fifo_ptr) + fifo->ptr_write * fifo->item_size, data, fifo->item_size * item_num);
    fifo->ptr_write += item_num;

    if (fifo->ptr_write >= fifo->item_sum)
    {
        fifo->ptr_write = 0;
    }
    if (fifo->ptr_write == fifo->ptr_read)
    {
        fifo->is_full = true;
    }

    return true;
}

/**
* @brief fifo检查是否可以读取
* @param fifo_index: fifo索引
* @return false:不可以读取.true:可以读取
*/

bool fifo_readable(int fifo_index)
{
    my_fifo *fifo = (my_fifo *)fifo_index;
    if (fifo->ptr_write == fifo->ptr_read && !fifo->is_full)
    {
        return false;
    }
    return true;
}

/**
* @brief fifo读取
* @param fifo_index: fifo索引
* @param data: 读取的数据
* @return false: 失败.true: 成功
*/

bool fifo_read(int fifo_index, void *data)
{
    my_fifo *fifo = (my_fifo *)fifo_index;
    if (fifo->ptr_write == fifo->ptr_read && !fifo->is_full)
    {
        return false;
    }

    memcpy(data, (char *)(fifo->fifo_ptr) + fifo->ptr_read * fifo->item_size, fifo->item_size);
    fifo->ptr_read++;
    if (fifo->ptr_read >= fifo->item_sum)
    {
        fifo->ptr_read = 0;
    }
    fifo->is_full = false;
    return true;
}

/**
* @brief fifo批量读取
* @param fifo_index: fifo索引
* @param data: 读取的数据
* @param item_num：读取的元素数
* @return false: 失败.true: 成功
*/

bool fifo_read_batch(int fifo_index, void *data, int item_num)
{
    my_fifo *fifo = (my_fifo *)fifo_index;
    if (fifo_readable_item_count((int)fifo) < item_num)
    {
        return false;
    }

    memcpy(data, (char *)(fifo->fifo_ptr) + fifo->ptr_read * fifo->item_size, fifo->item_size * item_num);
    fifo->ptr_read += item_num;
    if (fifo->ptr_read >= fifo->item_sum)
    {
        fifo->ptr_read = 0;
    }
    fifo->is_full = false;
    return true;
}

#endif

#if 0
#include "fifo.h"
#include <stdio.h>

#define uint8_t  unsigned char
#define uint16_t unsigned short
#define uint32_t unsigned long

struct _Test
{
    uint8_t a;
    uint16_t b;
};

int main()
{
    // fifo1操作
    struct _Test arr;
    int fifo1 = fifo_create(10, sizeof(struct _Test));
    for (uint8_t i = 0; i < 10; i++)
    {
        arr.a = i;
        arr.b = 100 + i;
        if (fifo_writeable(fifo1))
        {
            fifo_write(fifo1, (void *)&arr);
            printf("可写：%d 可读：%d\n", fifo_writeable_item_count(fifo1), fifo_readable_item_count(fifo1));
        }
    }

    while (1)
    {
        if (fifo_readable(fifo1) == false)
        {
            break;
        }
        fifo_read(fifo1, (void *)&arr);
        printf("read:%d %d\n", arr.a, arr.b);
    }

    // fifo2操作
    int fifo2 = fifo_create(100, 1);
    char str_arr[100] = {0};
    memcpy(str_arr, "jdh", 3);
    fifo_write_batch(fifo2, str_arr, 3);

    printf("fifo2可写：%d 可读：%d\n", fifo_writeable_item_count(fifo2), fifo_readable_item_count(fifo2));

    str_arr[0] = 0;
    fifo_read_batch(fifo2, str_arr, fifo_readable_item_count(fifo2));
    printf("read:%s\n", str_arr);

    getchar();
    return 0;
}
#endif