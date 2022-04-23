//
// Created by zz on 2022/2/13.
//

#include <mkrtos/task.h>
#include <arch/arch.h>
#include "mkrtos/mem.h"
#include <sys/wait.h>
/**
 * �������
 * @param wake_tasks
 */
void clear_task_q(struct task **wake_tasks){
    uint32_t t;
    t=DisCpuInter();
    while(*wake_tasks){
        *wake_tasks=(*wake_tasks)->del_wait;
    }
    RestoreCpuInter(t);
}
void add_wait_task_q(struct task** queue,struct task* add_queue){
    uint32_t t;
    t=DisCpuInter();
    if(*queue==NULL){
        *queue=add_queue;
    }else{
        add_queue->del_wait = (*queue);
        *queue=add_queue;
    }
    RestoreCpuInter(t);
}
/**
 * �Ƴ��ȴ�������
 * @param queue ��Ҫ�Ƴ����������
 * @param tk �Ƴ�������
 * @param max_prio ���صȴ������������ȼ�������ΪNULL�򲻷���
 * @return
 */
//�Ƴ�һ���ȴ���
void remove_wait_task_q(struct task ** queue,struct task* add_queue){
    struct task *temp=*queue;
    struct task *prev=NULL;
    uint32_t t;
    if(!add_queue){
        return ;
    }
    t=DisCpuInter();
    while(temp){
        if(temp==add_queue) {
            if (prev==NULL) {
                //ɾ���ĵ�һ��
                *queue=temp->del_wait;
                break;
            }else{
                prev->del_wait=temp->del_wait;
                break;
            }
        }
        prev=temp;
        temp=temp->del_wait;
    }
    RestoreCpuInter(t);
}

//pid_t waitpid(pid_t pid,int *status,int options);
//�ɹ���������������ӽ���ID��ʧ�ܣ�-1�����ӽ��̣�
//��������ͷ��������
//����pid��
//       >0 ����ָ��ID���ӽ���
//       -1 ���������ӽ��̣��൱��wait��
//       0 ���պ͵�ǰ����waitpidһ����������ӽ���
//       < -1 ����ָ���������ڵ������ӽ���
//����0������3ΪWNOHANG�����ӽ�����������
static void wait_task(struct wait_queue **wait_c){
    uint32_t t;
    RestoreCpuInter(t);
    struct wait_queue wait={CUR_TASK,NULL};
    //���������̻�û�н���ر�״̬����ȴ��ر��������
    add_wait_queue(wait_c,&wait);
    task_suspend();
//    CUR_TASK->status=TASK_SUSPEND;
    task_sche();
    remove_wait_queue(wait_c,&wait);
    task_run();
//    CUR_TASK->status=TASK_RUNNING;
}
static pid_t shutdown_task(struct task* ls){
    int32_t res_pid;
    del_task(NULL, ls,0);
    del_task(&sysTasks.allTaskList, ls,1);
    res_pid = ls->PID;
    OSFree(ls);
    return res_pid;
}
pid_t do_sys_wait(pid_t pid,int32_t *statloc,int32_t options,struct rusage *rusage ){
    uint32_t t;
    struct task *ls;
    struct task *close_task=0;
    uint32_t child_run_cn=0;
    uint32_t child_all_cn=0;
    again:
    child_run_cn=0;
    child_all_cn=0;
    t=DisCpuInter();
    ls=sysTasks.allTaskList;
    while(ls){
        if(ls->parentTask== CUR_TASK) {//��֤���������ӽ���
            if (pid == -1) {//�ȴ���һ�ӽ���
                if (ls->status == TASK_CLOSED) {
                    int32_t res_pid;
                    res_pid=shutdown_task(ls);
                    RestoreCpuInter(t);
                    if(statloc) {
                        *statloc = ls->exitCode;
                    }
                    return res_pid;
                } else {
                    //����ӵ�һ�������У�Ȼ���ڽ�β�������ж�
                    add_wait_task_q(&close_task, ls);
                    //�����е��ӽ�������
                    child_run_cn++;
                }
                //���е��ӽ�������
                child_all_cn++;

            } else if (pid > 0) {//�ȴ������ID��pid��ȵ��ӽ���
                if (ls->PID == pid) {
                    if (ls->status == TASK_CLOSED) {
                        int32_t res_pid;
                        res_pid=shutdown_task(ls);
                        if(statloc) {
                            *statloc = ls->exitCode;
                        }
                        RestoreCpuInter(t);
                        return res_pid;
                    } else {
                        if(options&WNOHANG ){
                            //������
                            return 0;
                        }else {
                            wait_task(&ls->close_wait);
                            goto again;
                        }
                    }
                }
            } else if (pid == 0 ||pid < -1) {
                int wait_pid;
                if(pid==0){//�ȴ�����ID���ڵ��ý�����ID����һ�ӽ���
                    wait_pid=CUR_TASK->PGID;
                }else{//�ȴ�����ID����pid����ֵ����һ�ӽ���
                    wait_pid=-pid;
                }
                if (ls->PGID==wait_pid) {
                    if(ls->status == TASK_CLOSED) {
                        int32_t res_pid;
                        res_pid = shutdown_task(ls);
                        if(statloc) {
                            *statloc = ls->exitCode;
                        }
                        RestoreCpuInter(t);
                        return res_pid;
                    }else{
                        //����ӵ�һ�������У�Ȼ���ڽ�β�������ж�
                        add_wait_task_q(&close_task, ls);
                        //�����е��ӽ�������
                        child_run_cn++;
                    }
                }else{
                    //���е��ӽ�������
                    child_all_cn++;
                }
            }
        }
        ls=ls->nextAll;
    }
    RestoreCpuInter(t);
    if(pid==-1 || pid<-1){
        if(child_all_cn==0){
            //û���ӽ��̣�����-1
            return -ECHILD;
        }else {
            //û���ӽ��̱��ر�
            if (child_all_cn == child_run_cn) {
                if(options & WNOHANG ){
                    //�������������ֱ�ӷ���
                    clear_task_q(&close_task);
                    return 0;
                }
                pid_t pid=-1;
                //�ȴ�����
                struct wait_queue wait_c={CUR_TASK,NULL};
                struct task *tmp;
                tmp=close_task;
                while(tmp){
                    //��ӵ��ȴ�����
                    add_wait_queue(&tmp->close_wait,&wait_c);
                    tmp=tmp->del_wait;
                }

                //��ʼ�ȴ�
                task_suspend();
//                CUR_TASK->status=TASK_SUSPEND;
                //��ǰ�̹߳���
                task_sche();
                uint8_t find=FALSE;
//                t=DisCpuInter();
                //˵��ĳ�����̱��ر���
                tmp=close_task;
                while(tmp){//�Ƴ����еĵȴ����񣬲��ҵ��Ǹ�closed�Ľ���
                    struct task* del_wait_next;
                    struct wait_queue **close_wait;
                    del_wait_next=tmp->del_wait;
                    close_wait=&tmp->close_wait;
                    if(tmp->status==TASK_CLOSED && !find){
                        pid=tmp->PID;
                        if(statloc) {
                            *statloc = tmp->exitCode;
                        }
                        shutdown_task(tmp);
                        find=TRUE;
                    }
                    remove_wait_queue(close_wait,&wait_c);
                    tmp=del_wait_next;
                }
//                RestoreCpuInter(t);
                //�������
                clear_task_q(&close_task);
                task_run();
//                CUR_TASK->status=TASK_RUNNING;
                return pid;
            }
        }
    }

    return -1;
}
pid_t sys_wait4(pid_t pid,int32_t *statloc,int32_t options,struct rusage *rusage ) {
    return do_sys_wait(pid,statloc,options,rusage);
}
/**
 * �ȴ�pid
 * @param pid
 * @param statloc
 * @param options
 * @return
 */
pid_t sys_waitpid(pid_t pid,int32_t *statloc,int32_t options){
    return do_sys_wait(pid,statloc,options,NULL);
}
