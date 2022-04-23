//
// Created by Administrator on 2022/1/23.
//
#include <ipc/ipc.h>
#include <mkrtos/task.h>
#include <mkrtos/mem.h>
#include "arch/arch.h"

//����������
#define MUTEX_NUM 16
//��������
static struct mutex mutex_list[MUTEX_NUM]={0};

int32_t mutex_get(sem_t mid){
    struct sem* temp;

    if(mid<0||mid>=MUTEX_NUM){
        return -EINVAL;
    }
    struct wait_queue *pwait=NULL;
    pwait= OSMalloc(sizeof(struct wait_queue));
    if(pwait == NULL){
        return -ENOMEM;
    }
    pwait->task=CUR_TASK;
    pwait->next=NULL;

    atomic_inc(&mutex_list[mid].s_used_count);
    //��ӵ��ȴ�������
    add_wait_queue(&mutex_list[mid].m_wait, pwait);
    return mid;
}
int32_t mutex_put(sem_t mid) {
    struct sem *temp;
    struct wait_queue* wq;

    if (mid < 0 || mid >= MUTEX_NUM) {
        return -EINVAL;
    }
    if (atomic_test(&mutex_list[mid].s_used_count,0)){
        //������˵��û����
        return -EAGAIN;
    }
    //ֻ���Լ����ܸɵ��Լ�
    wq=find_wait_queue(&mutex_list[mid].m_wait,CUR_TASK,NULL);
    if(wq!=NULL) {
        atomic_dec(&mutex_list[mid].s_used_count);
        remove_wait_queue(&mutex_list[mid].m_wait, wq);
        OSFree(wq);
    }else{
        return -EACCES;
    }
    return 0;
}
//��mutex
int32_t lock_mutex(int32_t mt_l){
    struct wait_queue* wq;
    struct mutex *mx;
    uint32_t max_prio;
    if(mt_l<0||mt_l>=MUTEX_NUM){
        return -EINVAL;
    }
    mx=&mutex_list[mt_l];
    //�ȼ������Ƿ���������Դ
    wq=find_wait_queue(&mx->m_wait,CUR_TASK,&max_prio);
    if(wq == NULL){
        return -EACCES;
    }
    again:
    if(atomic_test_set((&mx->m_lock),1)){
        //���ɹ�
        mx->m_tk_prev_prio=CUR_TASK->prio;
        if(CUR_TASK->prio!=max_prio) {//���ȼ��仯�˲Ž��и���
            //�������ȼ�����ͦ�鷳���ø������ڵö��У�md
            uint32_t t;
            t = DisCpuInter();
            CUR_TASK->prio = max_prio;
            if (add_task(CUR_TASK,0) < 0) {
                //û���㹻���ڴ��ˣ��ָ�֮ǰ��
                CUR_TASK->prio = mx->m_tk_prev_prio;
                RestoreCpuInter(t);
                return -ENOMEM;
            }
            CUR_TASK->prio = mx->m_tk_prev_prio;
            del_task(NULL,CUR_TASK,0);
            CUR_TASK->prio = max_prio;
            RestoreCpuInter(t);
        }
        mx->m_who_lock=CUR_TASK;
    }else{
        //���Ϊ�������ǰ����
        task_suspend();
//        CUR_TASK->status=TASK_SUSPEND;
        task_sche();
        goto again;
    }
    return 0;
}
//����mutex
int32_t unlock_mutex(int32_t mt_l){
    struct wait_queue* wq;
    struct mutex *mx;
    uint32_t t;
    uint32_t old_prio;
    if(mt_l<0||mt_l>=MUTEX_NUM){
        return -EINVAL;
    }
    mx=&mutex_list[mt_l];
    //��ǰ���̱����������Ľ���
    if(mx->m_who_lock==NULL || mx->m_who_lock!=CUR_TASK){
        return -1;
    }
    //�ȼ������Ƿ���������Դ
    wq=find_wait_queue(&mx->m_wait,mx->m_who_lock,NULL);
    if(wq == NULL){
        return -EACCES;
    }
    if(mx->m_tk_prev_prio!=mx->m_who_lock->prio) {
        //���ȼ��������Ž��и���
        t = DisCpuInter();
        old_prio = mx->m_who_lock->prio;
        mx->m_who_lock->prio = mx->m_tk_prev_prio;
        if (add_task(mx->m_who_lock,0)) {
            CUR_TASK->prio = old_prio;
            RestoreCpuInter(t);
            goto next;
        }
        mx->m_who_lock->prio=old_prio;
        del_task(NULL,mx->m_who_lock,0);
        mx->m_who_lock->prio=mx->m_tk_prev_prio;
        RestoreCpuInter(t);
    }
    next:
    wake_up(mx->m_wait);
    atomic_set(&mx->m_lock,0);
    return 0;
}
