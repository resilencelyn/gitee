//
// Created by Administrator on 2022/1/9.
//

#include <type.h>
#include "arch/arch.h"
#include <mkrtos/task.h>
#include <mkrtos/mem.h>
#include <errno.h>
#include <string.h>
/**
* @brief ϵͳ�����
*/
SysTasks sysTasks={0};

/**
* @brief ͨ��PID�ҵ��������
*/
struct task* find_task(int32_t PID){
    struct task* pstl;
    if(PID<0||PID==0){
        return sysTasks.currentTask;
    }
  //  uint32_t t=DisCpuInter();
    pstl=sysTasks.allTaskList;
    while(pstl){
        if(pstl->PID==PID){
         //   RestoreCpuInter(t);
            return pstl;
        }
        pstl=pstl->nextAll;
    }
  //  RestoreCpuInter(t);
    return NULL;
}
//�������񲻿ɴ��
void task_unintr(void){
    if(CUR_TASK->status!=TASK_UNINTR
       &&CUR_TASK->status!=TASK_CLOSED
            ) {
        CUR_TASK->status = TASK_UNINTR;
//        sysTasks.currentMaxTaskNode->taskReadyCount--;
//        if(sysTasks.currentMaxTaskNode->taskReadyCount==0){
//            //�������
//            update_cur_task();
//        }
    }
}
//���������
void task_suspend(void){
    if(CUR_TASK->status!=TASK_SUSPEND
     &&CUR_TASK->status!=TASK_CLOSED
     &&CUR_TASK->status!=TASK_UNINTR
    ) {
        sysTasks.currentMaxTaskNode->taskReadyCount--;
        if(sysTasks.currentMaxTaskNode->taskReadyCount==0){
            //�������
            update_cur_task();
        }
        CUR_TASK->status = TASK_SUSPEND;
    }
}
//����������
void task_run(void){
    if(CUR_TASK->status!=TASK_RUNNING
       &&CUR_TASK->status!=TASK_CLOSED
    ){
        if(CUR_TASK->status==TASK_SUSPEND) {
            sysTasks.currentMaxTaskNode->taskReadyCount++;
        }
        CUR_TASK->status = TASK_RUNNING;
    }
}
void task_run_1(struct task* tk){
    if(!tk){
        return ;
    }
    if(tk->status!=TASK_RUNNING
       &&tk->status!=TASK_CLOSED
            ){
        if(tk->status==TASK_SUSPEND) {
            tk->parent->taskReadyCount++;
            if (tk->prio > sysTasks.currentMaxTaskNode->taskPriority) {
                update_cur_task();
            }
        }
        tk->status = TASK_RUNNING;
    }
}

/**
* @brief ������ȣ����������ȹرգ��������Ч
*/
void task_sche(void){
    if(sysTasks.currentMaxTaskNode->taskReadyCount==0){
        return ;
    }
    //����Ƿ���Ե���
    if(atomic_test(&(sysTasks.isSch),TRUE)){
        _TaskSchedule();
    }
}
/**
* @brief ���¾�����������ȼ�������ڵ㣬���µ������о�������������0
* @param pSysTasks ����������
*/
void update_cur_task(void){

    PSysTaskBaseLinks ptl;
    uint32_t t=DisCpuInter();

    ptl=sysTasks.sysTaskBaseLinks.next;

    /*������ȼ�*/
    PSysTaskBaseLinks maxPrioTask=NULL;

    while(ptl){
        if(ptl->taskReadyCount>0){
            if(maxPrioTask==NULL
               && ptl->taskReadyCount>0
                    ){
                maxPrioTask=ptl;
            }else{
                if(ptl->taskPriority > maxPrioTask->taskPriority
                    //	&& ptl->taskReadyCount>0
                        ){
                    /*����*/
                    maxPrioTask=ptl;
                }
            }
        }
        ptl=ptl->next;
    }
    sysTasks.currentMaxTaskNode=maxPrioTask;

    /*��ǰ�����Ľڵ�*/
    //pSysTasks->currentTask=NULL;

    RestoreCpuInter(t);
}

/**
* @brief ͨ�����ȼ��ҵ�������
* @param prio �������ȼ�
* @return ������ȼ�����ı�ͷ
*/
static PSysTaskBaseLinks FindTaskLinks(uint8_t prio){
    PSysTaskBaseLinks ptl;
    ptl=sysTasks.sysTaskBaseLinks.next;
    while(ptl){
        if(ptl->taskPriority == prio){
            return ptl;
        }
        ptl=ptl->next;
    }
    return NULL;
}
/**
* @brief ͨ�����ȼ��������ͷ�����ָ�����ȼ������ڣ��򴴽�һ�����ȼ��Ľڵ�
* @param prio ��ӵ����ȼ�
 * @return ��ӳɹ�������ͷ
*/
static PSysTaskBaseLinks AddLinks(uint8_t prio){

    PSysTaskBaseLinks pSysTaskBaseLinks
            =(PSysTaskBaseLinks)OSMalloc(sizeof(SysTaskBaseLinks));
    if(pSysTaskBaseLinks == NULL){
        return NULL;
    }
    pSysTaskBaseLinks->next=NULL;
    pSysTaskBaseLinks->pSysTaskLinks=NULL;
    pSysTaskBaseLinks->taskCount=0;
    pSysTaskBaseLinks->taskPriority=prio;
    pSysTaskBaseLinks->taskReadyCount=0;

    /*�������*/
    pSysTaskBaseLinks->next=sysTasks.sysTaskBaseLinks.next;
    sysTasks.sysTaskBaseLinks.next=pSysTaskBaseLinks;

    return pSysTaskBaseLinks;
}
/**
 * ɾ������
 * @param del
 */
void del_task(struct task** task_ls, struct task* del,int flag){
    PSysTaskBaseLinks taskLinks;
    uint32_t t;
    t=DisCpuInter();
    if(!task_ls){
        taskLinks = FindTaskLinks(del->prio);
        if(taskLinks==NULL){
            RestoreCpuInter(t);
            return ;
        }
        task_ls=&(taskLinks->pSysTaskLinks);
    }
    PTaskBlock pTemp=*task_ls;//taskLinks->pSysTaskLinks;
    PTaskBlock lastP=NULL;
    while(pTemp){
        if(del==pTemp){
            if(lastP==NULL){
                if(!flag) {
                    *task_ls = pTemp->next;
                }else {
                    *task_ls = pTemp->nextAll;
                }
                break;
            }else{
                if(!flag) {
                    lastP->next = pTemp->next;
                }else{
                    lastP->nextAll = pTemp->nextAll;
                }
                break;
            }
        }
        lastP=pTemp;
        pTemp=pTemp->next;
    }
    RestoreCpuInter(t);
    update_cur_task();
}
/**
* @brief ͨ�����ȼ�����������������ȼ������ڣ��򴴽������ȼ�������ڵ�
* @param pSysTasks ����������
* @return ����Ƿ�ɹ�
*/
int32_t add_task(struct task* pTaskBlock,uint32_t into_all_ls){

    if(pTaskBlock==NULL){
        return -1;
    }
    /*�������ж�*/
    uint32_t t;
    t=DisCpuInter();
    PSysTaskBaseLinks taskLinks;

    taskLinks = FindTaskLinks(pTaskBlock->prio);
    if(taskLinks == NULL){
        taskLinks=AddLinks(pTaskBlock->prio);
        if(taskLinks==NULL){
            RestoreCpuInter(t);
            /*û���ڴ�*/
            return -ENOMEM;
        }
    }
    taskLinks->taskCount++;
    taskLinks->taskReadyCount++;

    //�ŵ�ͬ���ȼ�������������
    PTaskBlock pstl=taskLinks->pSysTaskLinks;
    if(pstl == NULL){
        pTaskBlock->next=NULL;
        taskLinks->pSysTaskLinks=pTaskBlock;
    }else{
        /*���������ͷ*/
        pTaskBlock->next=pstl;
        taskLinks->pSysTaskLinks=pTaskBlock;
    }
    pTaskBlock->parent=taskLinks;

    if(into_all_ls) {
        //�浽���������������
        pstl = sysTasks.allTaskList;
        if (pstl == NULL) {
            pTaskBlock->nextAll = NULL;
            sysTasks.allTaskList = pTaskBlock;
        } else {
            /*���������ͷ*/
            pTaskBlock->nextAll = pstl;
            sysTasks.allTaskList = pTaskBlock;
        }
    }
    RestoreCpuInter(t);

    //�������ȼ�
    if(
        sysTasks.currentMaxTaskNode!=NULL
        &&
        pTaskBlock->prio>sysTasks.currentMaxTaskNode->taskPriority
        ) {
        update_cur_task();
    }else if(sysTasks.currentMaxTaskNode==NULL){
        update_cur_task();
    }
    return 0;
}

/**
* @brief ��������м��
*/
void tasks_check(void){
    struct task* ptb;
    sysTasks.sysRunCount++;
    //������еĶ�ʱ���ź�
    ptb=sysTasks.allTaskList;
    while(ptb){
        if(ptb->status==TASK_RUNNING
        && ptb->alarm){
            //ʱ�䵽��
            if(ptb->alarm<sysTasks.sysRunCount){
                //����ָ���ź�
                inner_set_sig(SIGALRM);
                ptb->alarm = 0;
            }
        }
        /*else if(ptb->status==TASK_SUSPEND){
            //SIGCONT�ź������ﴦ��
            if (ptb->signalBMap & (1 << (SIGCONT - 1))) {
                //����յ����ź���SIGCONT�����Ҵ�ʱ������ͣ�ˣ�������
                ptb->status=TASK_RUNNING;
            }
        }
         */
nextAll:
        ptb=ptb->nextAll;
    }
}

int32_t task_change_prio(struct task *tk,int32_t new_prio){
    uint32_t t;
    uint32_t old_prio;
    struct _SysTaskBaseLinks *base_links;
    if(!tk){
        return -1;
    }
    if(tk->prio==new_prio){
        return 0;
    }
    t = DisCpuInter();
    base_links=CUR_TASK->parent;
    old_prio=CUR_TASK->prio;
    CUR_TASK->prio = new_prio;
    if (add_task(CUR_TASK,0) < 0) {
        //û���㹻���ڴ��ˣ��ָ�֮ǰ��
        CUR_TASK->prio = old_prio;
        RestoreCpuInter(t);
        return -ENOMEM;
    }
    base_links->taskReadyCount--;
    base_links->taskCount--;
    CUR_TASK->prio = old_prio;
    del_task(NULL,CUR_TASK,0);
    CUR_TASK->prio = new_prio;
    RestoreCpuInter(t);
    return 0;
}

/**
* @brief ִ��������ȣ�������һ�������ջ��
* @brief SP ��ǰ�̵߳�ջ����ַ
* @brief SPType ʹ�õ���MSP����PSP 0ʹ��msp 1ʹ��psp
* @return ����ջ��
*/
struct _stackInfo* sys_task_sche(void* psp,void* msp,uint32_t spType){
    uint16_t svcStatus=FALSE;
    volatile uint32_t *svcPendReg=(uint32_t*)(0xE000ED24);
    //����svc״̬
    if((*svcPendReg)&0x80L){
        (*svcPendReg)&=~(0x80L);
        svcStatus=TRUE;
    }
    if(sysTasks.isFirst==FALSE){
        /*��һ��δ���䣬�����һ��*/
        PTaskBlock ptb=sysTasks.currentMaxTaskNode->pSysTaskLinks;
        while(ptb){
            if(ptb->status!=TASK_RUNNING
                //|| ptb->delayCount>0
                    ){
                ptb=ptb->next;
                continue;
            }

            sysTasks.currentTask=ptb;
            sysTasks.isFirst=TRUE;
            break;
        }
    }else{
        /*�޸ĵ�ǰ�̵߳�ջ����ַ*/
        sysTasks.currentTask->skInfo.pspStack = psp;
        sysTasks.currentTask->skInfo.mspStack = msp;
        if (sysTasks.currentTask->skInfo.stackType != 2) {
            sysTasks.currentTask->skInfo.stackType = spType;
        }
        sysTasks.currentTask->skInfo.svcStatus = svcStatus;
        /*��ǰ����û�д���TASK_UNINTR״̬�ſ��Խ����л�����Ȼ���ܹ���*/
        if(CUR_TASK->status!=TASK_UNINTR) {
            /*֮ǰ�������ֱ������һ����Ч��*/
            PTaskBlock ptb;
            ptb = sysTasks.currentTask->next;
            do {
                if (ptb == NULL) {
                    ptb = sysTasks.currentMaxTaskNode->pSysTaskLinks;
                }
                if (ptb->status == TASK_RUNNING
                    /*|| ptb->delayCount>0*/
                        ) {
                    sysTasks.currentTask = ptb;
                    break;
                }

                ptb = ptb->next;
            } while (1);
        }
    }
    //�ָ������svc״̬
    if(sysTasks.currentTask->skInfo.svcStatus==TRUE){
        (*svcPendReg)|=0x80L;
        (*svcPendReg)&=~(0x100L);
    }
    sysTasks.currentTask->runCount++;
    /*���ض�ջ�ĵ�ַ*/
    return &(sysTasks.currentTask->skInfo);
}

// ϵͳ���ù��� - ���ñ�����ʱʱ��ֵ(��)��
// ����Ѿ����ù�alarm ֵ���򷵻ؾ�ֵ�����򷵻�0��
int32_t sys_alarm (uint32_t seconds){
    uint32_t old = 0;
    if(sysTasks.currentTask->alarm){
        old=sysTasks.currentTask->alarm;
    }
    sysTasks.currentTask->alarm = sysTasks.sysRunCount+(seconds*1000)/(1000/OS_WORK_HZ);
    return old;
}
// ������̵ȴ��ź�
int32_t sys_pause(void){
    task_suspend();
//    CUR_TASK->status=TASK_SUSPEND;
    task_sche();
    return -1;
}

//���Ѷ��������е�����
void wake_up(struct wait_queue *queue){
    uint32_t t;
    t=DisCpuInter();
    while(queue){
        if(queue->task){
            if(queue->task->status==TASK_SUSPEND
            ||queue->task->status==TASK_UNINTR
            ){
                task_run_1(queue->task);
//                queue->task->status=TASK_RUNNING;
            }
        }
        queue=queue->next;
    }
    RestoreCpuInter(t);
}

//���һ�����ȴ�������
void add_wait_queue(struct wait_queue ** queue,struct wait_queue* add_queue){
    uint32_t t;
    t=DisCpuInter();
    if(*queue==NULL){
        *queue=add_queue;
    }else{
       add_queue->next = (*queue);
       *queue=add_queue;
    }
    RestoreCpuInter(t);
}
//�ҵ�һ��queue������ȡ������ȼ�
struct wait_queue * find_wait_queue(struct wait_queue ** queue, struct task* tk,uint32_t *max_prio){
    struct wait_queue *temp=*queue;
    struct wait_queue *res=NULL;
    uint32_t t;
    t=DisCpuInter();
    if(max_prio) {
        *max_prio = 0;
    }
    while(temp){
        if ( temp->task->status != TASK_CLOSED
                ) {
            if(temp->task==tk){
                res=temp;
            }
            if (max_prio) {
                if (temp->task->prio > *max_prio) {
                    *max_prio = temp->task->prio;
                }
            }
        }
        temp=temp->next;
    }
    RestoreCpuInter(t);
    return res;
}
//�Ƴ�һ���ȴ���
void remove_wait_queue(struct wait_queue ** queue,struct wait_queue* add_queue){
    struct wait_queue *temp=*queue;
    struct wait_queue *prev=NULL;
    uint32_t t;
    if(!add_queue){
        return ;
    }
    t=DisCpuInter();
    while(temp){
        if(temp==add_queue) {
            if (prev==NULL) {
                //ɾ���ĵ�һ��
                *queue=temp->next;
                break;
            }else{
                prev->next=temp->next;
                break;
            }
        }
        prev=temp;
        temp=temp->next;
    }
    RestoreCpuInter(t);
}

/**
 * @brief ������ǰ�������ȼ�
 * @param increment
 * @return
 */
int32_t sys_nice (int32_t increment)
{
    int new_prio;
    new_prio=CUR_TASK->prio;
    if (new_prio- increment > 0)
        new_prio -= increment;

    if(task_change_prio(CUR_TASK,new_prio)<=0){
        return -ESRCH;
    }
    return 0;
}
extern void KernelTaskInit(void);
// ���ȳ���ĳ�ʼ���ӳ���
void SchedInit(void)
{
    InitMem();
    /*OS�Ƿ���ȳ�ʼ��*/
    atomic_set(&sysTasks.isSch,1);
    /*����pid�������*/
    atomic_set(&sysTasks.pidTemp,1);
    sysTasks.currentMaxTaskNode=NULL;
    sysTasks.tasksCount=0;
    KernelTaskInit();
}
