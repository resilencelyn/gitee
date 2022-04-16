#ifndef _RING_FIFO_H
#define _RING_FIFO_H

typedef struct Ring_fifo_s
{
    int head;
    int tail;
    int fill_cnt;
    int fifo_size;
    int fifo_item_len;
    char **data;
    //char data[20][1024];
} RING_FIFO;

RING_FIFO *Ring_FIFO_create(int array_size, int array_item_len);
int Ring_FIFO_length(RING_FIFO * FIFO);
int Ring_FIFO_read(RING_FIFO * FIFO, char **dst);
int Ring_FIFO_write(RING_FIFO * FIFO, char *src);

#endif
