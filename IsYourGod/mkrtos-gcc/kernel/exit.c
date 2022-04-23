//
// Created by Administrator on 2022/1/9.
//
#define __LIBRARY__
#include <unistd.h>
#include <mkrtos/task.h>
#include "arch/arch.h"
#include "stdlib.h"
#include "sched.h"
#include <mkrtos/mem.h>
#include <loader.h>
//signal.c
extern void sig_chld(void);
//shced.c
extern void update_cur_task(void);
extern void task_sche(void);
//fs.h
extern void sys_close(int fp);



//��ر�
void pre_exit(void){
//�ر����е��ļ�

    for( int i=0;i<NR_FILE;i++){
        if(CUR_TASK->files[i].used){
            sys_close(i);
        }
    }

    mem_clear();
    do_remove_sleep_tim(CUR_TASK);
}
/**
* @brief ��ϵͳ��ɾ����ǰִ�е����񣬸�ɾ��ֻ������Ϊ��ʬ����
*/
void do_exit(int32_t exitCode){
    //�������ж�
    uint32_t t;

    pre_exit();
    t=DisCpuInter();
    //��ǰ���̽����ˣ�Ӧ�ðɵ�ǰ���̵��ӽ���ȫ���ƽ�����ʼ������
    struct task* tmp=sysTasks.allTaskList;
    while(tmp){
        if(tmp->parentTask==CUR_TASK){
            tmp->parentTask=sysTasks.init_task;
        }
        tmp=tmp->nextAll;
    }
    //����chld��������
    sig_chld();
    //���ѵȴ�������йرյ�
    wake_up(CUR_TASK->close_wait);
    //���������Ѿ��ر���
    CUR_TASK->status=TASK_CLOSED;
    sysTasks.currentMaxTaskNode->taskReadyCount--;
    if(sysTasks.currentMaxTaskNode->taskReadyCount==0){
        //�������
        update_cur_task();
    }
    if(CUR_TASK->exec) {
        (*(CUR_TASK->exec->used_count))--;
        if(!((*CUR_TASK->exec->used_count))) {
            unload_elf(CUR_TASK->exec);
        }else{
//            if(!CUR_TASK->exec->clone_vm) {
                if(CUR_TASK->clone_flag&CLONE_VM) {
                    //û���븸����ʹ����ͬ���ڴ�ռ䣬����Ҫ�ͷ�����ռ�
                    OSFree(CUR_TASK->exec->data.data);
                    OSFree(CUR_TASK->exec->bss.data);
                }
                //fork���execҲ���µ�
                OSFree(CUR_TASK->exec);
//            }
        }
        CUR_TASK->exec=NULL;
    }
    if(CUR_TASK->memLowStack) {
        //�ͷ�ջ�ռ�
        OSFree(CUR_TASK->memLowStack);
    }
    OSFree(CUR_TASK->knl_low_stack);
    CUR_TASK->memLowStack=NULL;
    CUR_TASK->knl_low_stack=NULL;
    printk("sys remain mem %d\r\n",GetFreeMemory(1));
    task_sche();
    RestoreCpuInter(t);
}
/**
* @brief �������ʱ����øú���������ִ�н����������������������
*/
void TaskToEnd(int32_t exitCode){
//    printk("exit %d\n",exitCode);
    /*������Ҫͨ��ϵͳ���ã�����������û�����õ�*/
    _exit(exitCode);
    /*for(;;);*/
}

int sys_exit(int exitCode){
    /*ɾ����ǰ����*/
    inner_set_sig(SIGKILL);
    return 0;
}
