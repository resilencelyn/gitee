//
// Created by Administrator on 2022/1/9.
//

#ifndef UNTITLED1_TASK_H
#define UNTITLED1_TASK_H
#include "type.h"
#include "arch/atomic.h"
#include "mkrtos/signal.h"
#include "ipc/spin_lock.h"
#include <mkrtos/fs.h>
#include <loader.h>
#define NR_FILE 8

#define CUR_TASK sysTasks.currentTask

//struct file;

/**
* @brief �����״̬
*/
typedef enum{

    TASK_RUNNING,//��������
    TASK_SUSPEND,//�������
    TASK_CLOSED,// �����Ѿ����ر���
    TASK_UNINTR,//���񲻿ɱ��ж�
}TaskStatus;

/**
* @brief ������
*/
typedef void (*TaskFun)(void *arg0,void *arg1);


/**
* @brief �������������б�
*/
typedef struct _TaskCreatePar{
    /**
    * @brief ������
    */
    TaskFun taskFun;
    /**
    * @brief ����0
    */
    void *arg0;
    /**
    * @brief ����1
    */
    void *arg1;
    /**
    * @brief ���ȼ�
    */
    uint8_t prio;
    /**
    * @brief �û�ջ��С
    */
    uint16_t userStackSize;
    /**
    * @brief �ں�ջ��С
    */
    uint16_t kernelStackSize;
    /**
    * @brief ��������
    */
    const char* taskName;
    /**
     * @brief ���صĴ���
     */
    int32_t err;
}*PTaskCreatePar,TaskCreatePar;


/**
* @brief ջ����
*/
struct _stackInfo{
    /**
    * @brief ��ջ��ջ��ָ��
    */
    void *pspStack;
    void *mspStack;
    /**
    * @brief ʹ�õ���MSP����PSP 0ʹ��msp 1ʹ��psp 2�ں��߳�ģʽ
    */
    uint16_t stackType;
    /**
    * @brief svc�ж��Ƿ����,pendsv����Ƕ��svc�ж�,Ϊ1��������svc����
    */
    uint16_t svcStatus;
};



struct _SysTaskBaseLinks;
struct sigaction;
struct mem_struct;
struct tty_struct;
/**
* @brief	������ƿ�
*/
typedef struct task{
    struct _SysTaskBaseLinks *parent;//���ڵ�
    struct task *parentTask;//������
    struct task *next;//ͬ���ȼ�����
    struct task *nextAll;//�������������
    struct task *del_wait;//ɾ���ȴ�����
    struct wait_queue *close_wait;//�رյȴ�����

    void *memLowStack;//�Ñ���ջ��ջ��ָ�룬�������ս�ʱ�����ڴ��ͷ�
    void *knl_low_stack;//�ں�ջ
    uint32_t userStackSize;//�û�ջ��С
    uint32_t kernelStackSize;//�ں�ջ��С
    struct _stackInfo skInfo;//�洢��ջ��Ϣ

    uint32_t runCount;//����ʱ�����
    TaskStatus status;//��ǰ״̬
    uint8_t prio;//�������ȼ�
    const char*	taskName;// ��������

    uint32_t sig_bmp[_NSIG_WORDS];//�źŵ�λͼ
    struct sigaction signals[_NSIG];//�źŴ���
    uint32_t sig_mask[_NSIG_WORDS];//�ź�mask

    uint32_t alarm;//��ʱ����ms


    int32_t exitCode;//�˳���
    struct mem_struct *mems;//��������ڴ��������������ر�ʱ������������������ڴ潫���ͷ�

    uint8_t is_s_user; //�Ƿ񳬼��û�

    pid_t PID;//����id
    pid_t tpid;//�̵߳�pid

    uid_t ruid;//��ʵuser id
    uid_t euid;//��Чuser id
    uid_t suid;//�����user id
    uid_t rgid;//��ʵgroup id
    int32_t PGID; //��ID
    uid_t egid;//��Ч��id
    uid_t sgid;//�������id


    int clone_flag;//��¡��־

    /**
     * �ڽ��̴���һ�����ļ�����Ŀ¼ʱ����һ����ʹ���ļ���ʽ���������� (����3 . 3��3 . 4�ڣ�
     * ����������˵���� o p e n��c r e a t��������������������һ������ m o d e����ָ�������ļ��Ĵ�ȡ
     * ���Ȩλ)�����ǽ���4 . 2 0��˵����δ���һ����Ŀ¼�����ļ���ʽ������������Ϊ 1��λ����
     * �ļ�m o d e�е���Ӧλ��һ����ת��0��
     */
    mode_t mask;//�����ļ�ʱ���õ�mask
    struct tty_struct *tty;//��ǰ����ʹ�õ�tty������Ǻ�̨���̣���ò���ΪNULL
    struct file files[NR_FILE];//�ļ�
    struct inode* root_inode; //��inode
    struct inode* pwd_inode;//��ǰĿ¼
    char pwd_path[128];//��ǰ�ľ���·��
    ELFExec_t *exec;//��ǰӦ�õ�������
}*PTaskBlock,TaskBlock;




/**
* @brief ϵͳ������������洢��ͬ���ȼ�������ͷ
*/
typedef struct _SysTaskBaseLinks{
    /**
    * @brief ͬ���ȼ���������
    */
    PTaskBlock pSysTaskLinks;
    /**
    * @brief ��������ȼ�
    */
    uint8_t taskPriority;
    /**
    * @brief �������
    */
    uint16_t taskCount;
    /**
    * @brief �����������
    */
    uint16_t taskReadyCount;
    /**
    * @brief ��һ������ͷ�ڵ�
    */
    struct _SysTaskBaseLinks *next;

}*PSysTaskBaseLinks,SysTaskBaseLinks;

/**
* @brief ϵͳ����
*/
typedef struct{

    /**
    * @brief ϵͳ����
    */
    SysTaskBaseLinks sysTaskBaseLinks;
    /**
    * @brief ���б���������������
    */
    struct task* pBlockedLinks;
    struct task* init_task;
    /**
    * @brief ����ʱ��
    */
    uint32_t sysRunCount;

    /**
    * @brief ��ǰ������ȼ�����ڵ�
    */
    PSysTaskBaseLinks currentMaxTaskNode;
    /**
    * @brief �Ƿ��״�,��Ϊ0������Ϊ1
    */
    uint8_t isFirst;
    /**
    * @brief ��ǰ����������ڵ�
    */
    struct task* currentTask;
    /**
    * @brief ϵͳ������
    */
    uint32_t tasksCount;

    /**
    * @brief �Ƿ�����ϵͳ�����������
    */
    Atomic_t isSch;
    /**
    * @brief �������������
    */
    PTaskBlock allTaskList;
    /**
    * @brief ���������������IDʹ��
    */
    Atomic_t pidTemp;
}*PSysTasks,SysTasks;

extern SysTasks sysTasks;

struct task* find_task(int32_t PID);
void    task_sche(void);
int32_t add_task(struct task *add,uint32_t into_all_ls);
void    del_task(struct task** task_ls, struct task* del,int flag);
int32_t task_create(PTaskCreatePar tcp);

//�ȴ�����
struct wait_queue{
    struct task* task;
    struct wait_queue *next;
};
//sched.c
void wake_up(struct wait_queue *queue);
void add_wait_queue(struct wait_queue ** queue,struct wait_queue* add_queue);
struct wait_queue * find_wait_queue(struct wait_queue ** queue, struct task* tk,uint32_t *max_prio);
void remove_wait_queue(struct wait_queue ** queue,struct wait_queue* add_queue);
int32_t task_change_prio(struct task *tk,int32_t new_prio);
void update_cur_task(void);
void task_suspend(void);
void task_unintr(void);
void task_run(void);
void task_run_1(struct task* tk);
int32_t sys_pause(void);

//printk.c
void printk(const char *fmt, ...);
void fatalk(const char* fmt, ...);

//signal.c
int32_t inner_set_sig(uint32_t signum);
int32_t inner_set_task_sig(struct task *tk,uint32_t signum);
int32_t inner_set_pid_sig(pid_t pid,uint32_t signum);

//exit.c
void pre_exit(void);

//sleep.c
struct sleep_time_queue{
    struct task* task;
    struct sleep_time_queue *next;
    uint32_t slp_ms;
    uint32_t cur_ms;
};
void _do_check_sleep_tim(struct sleep_time_queue **slp_ls);
void do_check_sleep_tim(void);
void do_remove_sleep_tim(struct task* tk) ;
void add_sleep(struct sleep_time_queue **queue,struct sleep_time_queue *add);
void remove_sleep(struct sleep_time_queue **queue,struct sleep_time_queue *rm);
int do_nanosleep(struct sleep_time_queue **slp_ls,const struct timespec *req,struct timespec *rem,Atomic_t *lock,int check_val
        );
int sys_nanosleep(const struct timespec *req, struct timespec *rem);
void wake_up_sleep(struct sleep_time_queue **queue);
#endif //UNTITLED1_TASK_H
