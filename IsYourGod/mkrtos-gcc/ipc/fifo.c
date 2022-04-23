//
// Created by Administrator on 2022/1/23.
//

#include <type.h>
#include <errno.h>
#include <arch/atomic.h>
#include <mkrtos/task.h>
#include <mkrtos/mem.h>
#include <string.h>

//fifoʵ��
struct fifo{
    uint32_t fifo_len;
    uint32_t data_size;
    uint32_t front;
    uint32_t rear;
    uint8_t *data;
    Atomic_t used_cnt;
    struct wait_queue *f_put_wait;
    struct wait_queue *f_get_wait;
};

#define FIFO_NUM 16
struct fifo fifo_list[FIFO_NUM]={0};

//�Ƿ�����
#define is_full(f) ((f)->front == ((f)->rear + 1)%(f)->fifo_len)
//�Ƿ��
#define is_empty(f) ((f)->front == (f)->rear)

int32_t fifo_get(int32_t key){
    struct wait_queue* wq;
    struct wait_queue *pwait=NULL;

    if(key<0 || key> FIFO_NUM){
        return -1;
    }

    wq=find_wait_queue(&fifo_list[key].f_put_wait,CUR_TASK,NULL);
    if(wq){
        //�Ѿ���������
        return key;
    }
    wq=find_wait_queue(&fifo_list[key].f_get_wait,CUR_TASK,NULL);
    if(wq){
        //�Ѿ���������
        return key;
    }

    pwait= OSMalloc(sizeof(struct wait_queue));
    if(pwait == NULL){
        return -ENOMEM;
    }
    pwait->task=CUR_TASK;
    pwait->next=NULL;

    atomic_inc(&fifo_list[key].used_cnt);
    //��ӵ��ȴ�������
    add_wait_queue(&fifo_list[key].f_put_wait, pwait);
    add_wait_queue(&fifo_list[key].f_get_wait, pwait);

    return key;
}
int32_t fifo_put(int32_t key){
    struct sem *temp;
    struct wait_queue* wq;
    struct wait_queue* wq1;

    if (key < 0 || key >= FIFO_NUM) {
        return -EINVAL;
    }
    if (atomic_test(&fifo_list[key].used_cnt,0)){
        return -EAGAIN;
    }
    //ֻ���Լ����ܸɵ��Լ�
    wq=find_wait_queue(&fifo_list[key].f_put_wait,CUR_TASK,NULL);
    wq1=find_wait_queue(&fifo_list[key].f_get_wait,CUR_TASK,NULL);

    if( (!wq) && (!wq1) ){
        return -EACCES;
    }

    remove_wait_queue(&fifo_list[key].f_get_wait, wq);
    remove_wait_queue(&fifo_list[key].f_get_wait, wq1);

    OSFree(wq);
    OSFree(wq1);

    atomic_dec(&fifo_list[key].used_cnt);
    return 0;
}
int32_t fifo_put_data(int32_t key,uint8_t *data){
    struct fifo *f;
    if (key < 0 || key >= FIFO_NUM) {
        return -EINVAL;
    }
    f=&(fifo_list[key]);
    if ((f->rear + 1) % f->fifo_len == f->front) {
        return -1; //��������ʱ����ִ����Ӳ���
    }
    //��������
    memcpy(f->data + (f->rear * f->data_size),data,f->data_size);
    //β��Ԫ��ָ����һ���ռ�λ��,ȡģ���㱣֤��������Խ�磨����һ��С�ڳ�����
    f->rear = (f->rear + 1) % f->fifo_len;
    return 0;
}
int32_t fifo_get_data(int32_t key,uint8_t *data) {
    struct fifo *f;
    if (key < 0 || key >= FIFO_NUM) {
        return -EINVAL;
    }
    f=&(fifo_list[key]);
    //�ǲ���Ϊ��
    if (f->front == f->rear)  {
        return -1;
    }
    //��������
    memcpy(data,f->data+f->front*f->data_size,f->data_size);
    f->front = (f->front + 1) % f->fifo_len;
    return 0;
}
