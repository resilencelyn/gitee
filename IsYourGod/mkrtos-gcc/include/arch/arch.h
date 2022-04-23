#ifndef _ARCH_H__
#define _ARCH_H__

#include "type.h"

/**
 * @brief OS����Ƶ��
 */
#define OS_WORK_HZ 1000
/**
 * @brief OS���ڵ�ms
 */
#define OS_WORK_PERIOD_MS  ((1000/OS_WORK_HZ))

int32_t ArchInit(void);
/**
* @breif �ر�cpu�ж�
* @return ����֮ǰ���ж�ʹ��״̬
*/
uint32_t DisCpuInter(void);
/**
* @breif �ָ�cpu�ж�״̬
*/
void RestoreCpuInter(uint32_t s);
void SetPSP(void* newPSP);
uint32_t GetPSP(void);
/**
* @breif ����CPU�ж�
*/
void EnCpuInter(void);
/**
* @breif ������û������������л����ֶ�����PendSv
*/
void _TaskSchedule(void);
/**
 * ���û�����ִ�к�������Ҫ��pspջ��ģ��ֵ
 * @param callFunc
 * @param arg0
 * @param memStack
 * @return
 */
void* SetIntoUserStack(void *callFunc,void *arg0,uint32_t *memStack,void* resAddr);
/**
* @breif ���Ĵ���
* @param mem ջ�ڴ�
* @param taskFun ������
* @param prg0 ��Ҫ���ݵĲ���0
* @param prg1��Ҫ���ݵĲ���1
* @return ջ����ַ
*/
uint32_t* OSTaskSetReg(
		uint32_t * memStack,
		void (*taskFun)(void*arg0,void *arg1),
		void *prg0,
		void *prg1,
        void *prg2
	);



#endif
