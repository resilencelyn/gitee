#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEGUB_RING_FIFO
typedef struct Ring_fifo_s
{
    int head;
    int tail;
    int fill_cnt;
    int fifo_size;
    int fifo_item_len;
    char **data;
} RING_FIFO;
#else
#include "Ring_fifo.h"
#endif

/*
 * ┌───┬───┬───┐
 * │ 7 │ 0 │ 1 │
 * ├───┼───┼───┤
 * │ 6 │   │ 2 │
 * ├───┼───┼───┤
 * │ 5 │ 4 │ 3 │
 * └───┴───┴───┘
 */

RING_FIFO *Ring_FIFO_create(int array_size, int array_item_len)
{
    RING_FIFO *FIFO = (RING_FIFO *) malloc(sizeof (RING_FIFO));

    FIFO->head = 0;
    FIFO->tail = 0;
    FIFO->fill_cnt = 0;
    FIFO->fifo_size = array_size;
    FIFO->fifo_item_len = array_item_len;

#if 1
    FIFO->data = (char **) malloc(array_size * sizeof (char **));
    memset(FIFO->data, 0, array_size * sizeof (char **));
    int i;
    char *p;

    for (i = 0; i < array_size; i++)
    {
        *(FIFO->data + i) = (char *) malloc(array_item_len);
        memset(*(FIFO->data + i), 0, array_item_len);
    }
#endif

    return FIFO;
}

int Ring_FIFO_length(RING_FIFO * FIFO)
{
    return FIFO->fill_cnt;
}

int Ring_FIFO_read(RING_FIFO * FIFO, char **dst)
{
    if (FIFO->fill_cnt > 0)
    {
        *dst = *(FIFO->data + FIFO->head);

        //memset(FIFO->data[FIFO->head], 0, FIFO->l);

        FIFO->head++;
        FIFO->head %= FIFO->fifo_size;

        FIFO->fill_cnt--;
    } else
    {
        return 0;
    }
    return 1;
}

int Ring_FIFO_write(RING_FIFO * FIFO, char *src)
{
    if (FIFO->fill_cnt < FIFO->fifo_size)
    {
        strncpy(*(FIFO->data + FIFO->tail), src, FIFO->fifo_item_len);  // '\0';
        //memcpy(*(FIFO->data + FIFO->tail), src, FIFO->fifo_item_len); // copy n size

        FIFO->tail++;
        FIFO->tail %= FIFO->fifo_size;

        FIFO->fill_cnt++;

        return 1;
    } else
    {
        return 0;
    }
}

/*
 * This is test Function.
 * gcc -nostartfiles -e fifo_test -o fifo_test Ring_fifo.c -DDEGUB_RING_FIFO
 *
 */

#ifdef DEGUB_RING_FIFO
int fifo_test()
{
    int i;
    int ret;
    RING_FIFO *FIFO;

    FIFO = Ring_FIFO_create(20, 1024);

    printf("fifo_length(): %d\n\n", Ring_FIFO_length(FIFO));

    Ring_FIFO_write(FIFO, "helllo\n");
    Ring_FIFO_write(FIFO, "helllo2\n");

    printf("fifo_length(): %d\n\n", Ring_FIFO_length(FIFO));

    char *str;

    Ring_FIFO_read(FIFO, &str);
    printf("%s", str);
    Ring_FIFO_read(FIFO, &str);
    printf("%s", str);

    printf("fifo_length(): %d\n\n", Ring_FIFO_length(FIFO));
    return 0;
}
#endif
