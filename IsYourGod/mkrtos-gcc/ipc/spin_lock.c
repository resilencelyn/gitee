/**
* @breif ������ֻ����ϵͳֻ��ͬ���ȼ�����ʱ�����ֻ����һ�������ȼ���������������������������
* @author 1358745329@qq.com
*/
#include <type.h>
#include <ipc/spin_lock.h>
#include <mkrtos/mem.h>
/**
* @breif ����������
*/
pspinlock_handler spin_lock_create(void){
    pspinlock_handler psh;
	psh=OSMalloc(sizeof(spin_lock));
	if(psh==NULL){
		return psh;
	}
    atomic_set(&(psh->lock),0);
	return psh;
}
/**
* @breif ��������ʼ��
* @breif ��Ҫ��ʼ����������
*/
void spin_lock_init(pspinlock_handler pslh){
	if(pslh==NULL){
		return ;
	}
    atomic_set(&(pslh->lock),0);
}
/**
 * @brief ����������
 * 
 * @param pslh 
 */
void spin_lock_destory(pspinlock_handler pslh){
	if(pslh==NULL){
		return ;
	}
	OSFree(pslh);
}
/**
* @breif ����
* @param psh ������������
*/
int32_t spin_lock(pspinlock_handler psh){
	if(psh==NULL){
		return -1;
	}
again:
	if(!atomic_test_set(&(psh->lock),1)){
		/*����ʧ�ܣ������������߲��ȴ�������*/
		goto again;
	}else{
        /*ԭ�Ӳ��������ɹ�*/
	}
	return 0;
}
/**
* @breif ����
* @param psh ������������
*/ 
int32_t spin_unlock(pspinlock_handler psh){
	if(psh==NULL){
		return -1;
	}
	atomic_set(&(psh->lock),0);
	return 0;
}
