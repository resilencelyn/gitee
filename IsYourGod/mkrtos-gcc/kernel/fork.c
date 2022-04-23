//
// Created by Administrator on 2022/1/9.
//
#include <mkrtos/mem.h>
#include <type.h>
#include <mkrtos/task.h>
#include "arch/arch.h"
#include <string.h>
#include <sched.h>
extern PTaskBlock find_task(int32_t PID);
extern int lwip_fork(int s) ;
//pipe.c
void do_fork_pipe(struct inode *inode,struct task* new_task,int fd);

// fnΪ����ָ�룬��ָ��ָ��һ�������壬����Ҫ�������̵ľ�̬��������֪�����̵�4Ҫ�أ��������ָ������ָ�룬������ν�ġ��籾", ����
//     child_stackΪ���ӽ��̷���ϵͳ��ջ��ָ�루��linux��ϵͳ��ջ�ռ���2ҳ�棬����8K���ڴ棬����������ڴ��У��͵�ַ�Ϸ�����ֵ�����ֵ���ǽ��̿��ƿ�task_struct��ֵ����
//      arg���Ǵ����ӽ��̵Ĳ���һ��Ϊ��0����
//     flagsΪҪ������Դ�ı�־����������Ҫ�Ӹ����̼̳���Щ��Դ������Դ���ƻ��ǹ������������ò�����
//������flags����ȡ��ֵ
//  ��־                    ����
//  CLONE_PARENT   �������ӽ��̵ĸ������ǵ����ߵĸ����̣��½����봴�����Ľ��̳��ˡ��ֵܡ������ǡ����ӡ�
//  CLONE_FS           �ӽ����븸���̹�����ͬ���ļ�ϵͳ������root����ǰĿ¼��umask
//  CLONE_FILES      �ӽ����븸���̹�����ͬ���ļ���������file descriptor����
//  CLONE_NEWNS   ���µ�namespace�����ӽ��̣�namespace�����˽��̵��ļ�hierarchy
//  CLONE_SIGHAND   �ӽ����븸���̹�����ͬ���źŴ���signal handler����
//  CLONE_PTRACE   �������̱�trace���ӽ���Ҳ��trace ��֧��
//  CLONE_VFORK     �����̱�����ֱ���ӽ����ͷ������ڴ���Դ
//  CLONE_VM           �ӽ����븸������������ͬ���ڴ�ռ�
//  CLONE_PID          �ӽ����ڴ���ʱPID�븸����һ��
//  CLONE_THREAD    Linux 2.4��������֧��POSIX�̱߳�׼���ӽ����븸���̹�����ͬ���߳�Ⱥ

//child_stack���û�ջ���ں�ջ������Ҫ����
int32_t sys_clone(int (*fn)(void*),void* child_stack,int flags,void* arg){

    if(!fn //|| !child_stack
    ) {
        return -EINVAL;
    }

    uint32_t t=DisCpuInter();
    PTaskBlock ptb=CUR_TASK;
    PTaskBlock newPtb=OSMalloc(sizeof(TaskBlock));
    if(newPtb==NULL){
        RestoreCpuInter(t);
        return -1;
    }

    memcpy(newPtb,ptb,sizeof(TaskBlock));
    newPtb->status=TASK_SUSPEND;
    newPtb->runCount=0;
    newPtb->next=NULL;
    newPtb->nextAll=NULL;

    newPtb->memLowStack=NULL;

    //�����ں�ջ�ռ�
    newPtb->knl_low_stack= (void *) OSMalloc(sizeof(uint32_t) * (newPtb->kernelStackSize));
    if (newPtb->knl_low_stack == NULL) {
        OSFree(newPtb);
        RestoreCpuInter(t);
        return -1;
    }

    if(!(flags&CLONE_PID)) {
        newPtb->PID = (pid_t) atomic_read(&sysTasks.pidTemp);
    }

    //clone�ڴ�ռ�
    if (ptb->exec) {
        void *exec_tmp = newPtb->exec;
        //���¸���Ӧ����Ϣ
        newPtb->exec = OSMalloc(sizeof(ELFExec_t));
        if (!newPtb->exec) {
            OSFree(newPtb);
            OSFree( newPtb->knl_low_stack);
            RestoreCpuInter(t);
            return -1;
        }
        memcpy(newPtb->exec, exec_tmp, sizeof(ELFExec_t));

        if(!(flags&CLONE_VM)) {
            //��ʹ����ͬ���ڴ�ռ䣬RAM���Ƕ�����
            newPtb->exec->data.data = OSMalloc(ptb->exec->data.sh_size);
            if (!newPtb->exec->data.data) {
                OSFree(newPtb);
                OSFree(ptb->exec);
                OSFree( newPtb->knl_low_stack);
                RestoreCpuInter(t);
                return -1;
            }
            memcpy(newPtb->exec->data.data, ptb->exec->data.data, ptb->exec->data.sh_size);
            newPtb->exec->bss.data = OSMalloc(ptb->exec->bss.sh_size);
            if (!newPtb->exec->bss.data) {
                OSFree(newPtb);
                OSFree(ptb->exec);
                OSFree(newPtb->exec->data.data);
                OSFree( newPtb->knl_low_stack);
                RestoreCpuInter(t);
                return -1;
            }
            memcpy(newPtb->exec->bss.data, ptb->exec->bss.data, ptb->exec->bss.sh_size);
//            newPtb->exec->clone_vm=TRUE;
        }
        //��־��exec��ʹ�ô���+1
        (*(ptb->exec->used_count))++;
    }

    int32_t err;
    /*ͨ�����ȼ��������*/
    err = add_task(newPtb,1);
    if(err != 0){
        //	RestoreCpuInter(t);
        /*�ͷ�������ڴ�*/
        OSFree(newPtb);
        OSFree(ptb->exec);
        OSFree( newPtb->knl_low_stack);
        OSFree(newPtb->exec->data.data);
        OSFree(newPtb->exec->bss.data);
        RestoreCpuInter(t);
        return -1;
    }

    atomic_inc(&sysTasks.pidTemp);

    //�����ں�ջλ��
    newPtb->skInfo.mspStack=(void*)(&(((uint32_t*)newPtb->knl_low_stack)[ptb->kernelStackSize-1]));
    newPtb->skInfo.mspStack = OSTaskSetReg(newPtb->skInfo.mspStack, fn, arg, 0, 0);
    //�����û�ջ
    newPtb->skInfo.pspStack=child_stack;
    if(newPtb->skInfo.pspStack) {
        //���¶�λ
        newPtb->skInfo.pspStack = OSTaskSetReg(newPtb->skInfo.pspStack, fn, arg, 0, 0);
    }else{
        newPtb->userStackSize=0;
        newPtb->skInfo.pspStack=(void*)(~(0L));
    }
    if(!newPtb->userStackSize){
        newPtb->skInfo.stackType=2;
    }else{
        newPtb->skInfo.stackType=1;
    }
    //����Ϊ�û�ģʽ
    newPtb->skInfo.svcStatus=0;
    if(flags&CLONE_PARENT){
        //CLONE_PARENT   �������ӽ��̵ĸ������ǵ����ߵĸ����̣��½����봴�����Ľ��̳��ˡ��ֵܡ������ǡ����ӡ�
        newPtb->parentTask=ptb->parentTask;
    }else{
        //���ø�����
        newPtb->parentTask=ptb;
    }

    if(!(flags&CLONE_FS)){
        //��ʹ�ø����̵�FS��Ϣ
        //��0�Ž��̵İ�
        newPtb->root_inode=sysTasks.init_task->root_inode;
        newPtb->pwd_inode=sysTasks.init_task->pwd_inode;
        //���ü�����+1
        atomic_inc(&newPtb->root_inode->i_used_count);
        atomic_inc(&newPtb->pwd_inode->i_used_count);
        newPtb->mask=0;
        newPtb->pwd_path[0]='\0';
    }

    if(flags&CLONE_FILES) {
        //�ӽ����븸���̹�����ͬ���ļ���������file descriptor����
        //���ڴ򿪵��ļ�������Ӧ������inode�����ý���+1�Ĳ�����
        for (int i = 0; i < NR_FILE; i++) {
            if (newPtb->files[i].used) {
                struct inode *tmp;
                if(newPtb->files[i].net_file){
                    lwip_fork(newPtb->files[i].net_sock);
                }else {
                    tmp = newPtb->files[i].f_inode;
                    if (tmp) {
                        if (i < 3) {
                            tmp->i_ops->default_file_ops->open(tmp, &newPtb->files[i]);
                        }
                        //��pipe����fork
                        do_fork_pipe(tmp, newPtb, i);
                        atomic_inc(&(tmp->i_used_count));
                    }
                }
            }
        }
    }else{
        //�����Ƹ����̵�file
        //ǰ�������ù�
        for (int i = 0; i < NR_FILE; i++) {
            if(i<3){
                if(newPtb->files[i].used) {
//                    if(newPtb->files[i].net_file){
//                        newPtb->files[i].sock_used_cn++;
//                    }else {
                        if (newPtb->files[i].f_inode
                                ) {
                            atomic_inc(&newPtb->files[i].f_inode->i_used_count);
                        }
//                    }
                }
            }else {
                if (newPtb->files[i].used) {
                    newPtb->files[i].used = 0;
                }
            }
        }
    }
    newPtb->del_wait=NULL;
    newPtb->close_wait=NULL;

    if(!(flags&CLONE_SIGHAND)) {
        //�������źŴ����
        newPtb->sig_bmp[0] = 0;
        newPtb->sig_bmp[1] = 0;
        for (int i = 0; i < _NSIG; i++) {
            newPtb->signals[i]._u._sa_handler = SIG_DFL;
        }
    }
    if(flags&CLONE_THREAD){
        newPtb->parentTask=ptb->parentTask;
        newPtb->tpid=ptb->PID;
    }

    //mem��Ϣ������fork
    newPtb->mems=NULL;
    newPtb->status=TASK_RUNNING;
    newPtb->clone_flag=flags;
    RestoreCpuInter(t);

    //����pid
    return newPtb->PID;
}

//����һ���ӽ���
int32_t sys_fork(uint32_t *psp){
//    uint32_t t=DisCpuInter();
    PTaskBlock ptb=CUR_TASK;
    PTaskBlock newPtb=OSMalloc(sizeof(TaskBlock));
    if(newPtb==NULL){
//        RestoreCpuInter(t);
        return -1;
    }
    memcpy(newPtb,ptb,sizeof(TaskBlock));
    newPtb->status=TASK_SUSPEND;
    newPtb->runCount=0;
    newPtb->next=NULL;
    newPtb->nextAll=NULL;
    newPtb->memLowStack=(void *)OSMalloc(sizeof(uint32_t)*(newPtb->userStackSize));
    if(newPtb->memLowStack==NULL){
        OSFree(newPtb);
//        RestoreCpuInter(t);
        return -1;
    }
    newPtb->knl_low_stack=(void *)OSMalloc(sizeof(uint32_t)*(newPtb->kernelStackSize));
    if(newPtb->knl_low_stack==NULL){
        OSFree(newPtb->memLowStack);
        OSFree(newPtb);
//        RestoreCpuInter(t);
        return -1;
    }
    newPtb->PID = (pid_t)atomic_read(&sysTasks.pidTemp);
    uint32_t t=DisCpuInter();
    if(ptb->exec){
        void* exec_tmp=newPtb->exec;
        //���¸���Ӧ����Ϣ
        newPtb->exec=OSMalloc(sizeof(ELFExec_t));
        if(!newPtb->exec){
            OSFree(newPtb);
            OSFree(newPtb->memLowStack);
            OSFree(newPtb->knl_low_stack);
            RestoreCpuInter(t);
            return -1;
        }
        memcpy(newPtb->exec,exec_tmp,sizeof(ELFExec_t));
        newPtb->exec->data.data = OSMalloc(ptb->exec->data.sh_size);
        if(!newPtb->exec->data.data){
            OSFree(newPtb);
            OSFree(ptb->exec);
            OSFree(newPtb->memLowStack);
            OSFree(newPtb->knl_low_stack);
            RestoreCpuInter(t);
            return -1;
        }
        memcpy(newPtb->exec->data.data,ptb->exec->data.data,ptb->exec->data.sh_size);
        newPtb->exec->bss.data = OSMalloc(ptb->exec->bss.sh_size);
        if(!newPtb->exec->bss.data){
            OSFree(newPtb);
            OSFree(ptb->exec);
            OSFree(newPtb->memLowStack);
            OSFree(newPtb->knl_low_stack);
            OSFree(newPtb->exec->data.data);
            RestoreCpuInter(t);
            return -1;
        }
        memcpy(newPtb->exec->bss.data,ptb->exec->bss.data,ptb->exec->bss.sh_size);
        (*(ptb->exec->used_count))++;
    }

    int32_t err;
    newPtb->parent=NULL;
    /*ͨ�����ȼ��������*/
    err = add_task(newPtb,1);
    if(err != 0){
        //	RestoreCpuInter(t);
        /*�ͷ�������ڴ�*/
        OSFree(newPtb);
        OSFree(ptb->exec);
        OSFree(newPtb->memLowStack);
        OSFree(newPtb->knl_low_stack);
        OSFree(newPtb->exec->data.data);
        OSFree(newPtb->exec->bss.data);
        RestoreCpuInter(t);
        return -1;
    }

    atomic_inc(&sysTasks.pidTemp);
    //���ø�����
    newPtb->parentTask=ptb;
    //����ջ
    memcpy(newPtb->memLowStack,ptb->memLowStack,sizeof(uint32_t)*(newPtb->userStackSize));
    memcpy(newPtb->knl_low_stack,ptb->knl_low_stack,sizeof(uint32_t)*(newPtb->kernelStackSize));

    //����ջλ��
    newPtb->skInfo.mspStack=(void*)(&(((uint32_t*)newPtb->knl_low_stack)[ptb->kernelStackSize-1]));
    if(newPtb->userStackSize!=0){
        newPtb->skInfo.pspStack=(void*)((uint32_t)(newPtb->memLowStack)+((uint32_t)(psp)- (uint32_t)(ptb->memLowStack)));
        ((uint32_t*)(newPtb->skInfo.pspStack))[8]=0;
    }else{
        newPtb->skInfo.pspStack=(void*)(~(0L));
    }
    RestoreCpuInter(t);
    //����Ϊ�û�ģʽ
    newPtb->skInfo.svcStatus=0;
    newPtb->skInfo.stackType=1;

    //���ڴ򿪵��ļ�������Ӧ������inode�����ý���+1�Ĳ�����
    for(int i=0;i<NR_FILE;i++){
        if(newPtb->files[i].used){
            struct inode *tmp;
            if(newPtb->files[i].net_file){
                lwip_fork(newPtb->files[i].net_sock);
            }else {
                tmp = newPtb->files[i].f_inode;
                if (tmp) {
                    if (i < 3) {
                        tmp->i_ops->default_file_ops->open(tmp, &newPtb->files[i]);
                    }
                    //��pipe����fork
                    do_fork_pipe(tmp, newPtb, i);
                    atomic_inc(&(tmp->i_used_count));
                }
            }
        }
    }
    //���ü�����+1
    if(newPtb->root_inode) {
        atomic_inc(&newPtb->root_inode->i_used_count);
    }
    if(newPtb->pwd_inode) {
        atomic_inc(&newPtb->pwd_inode->i_used_count);
    }
    newPtb->del_wait=NULL;
    newPtb->close_wait=NULL;
    newPtb->sig_bmp[0]=0;
    newPtb->sig_bmp[1]=0;
    newPtb->sig_mask[0]=0;
    newPtb->sig_mask[1]=0;
    for(int i=0;i<_NSIG;i++) {
        newPtb->signals[i]._u._sa_handler=SIG_DFL;
    }
    //mem��Ϣ������fork
    newPtb->mems=NULL;
    newPtb->status=TASK_RUNNING;


    //����pid
    return newPtb->PID;
}