
#include "mkrtos/smem_knl.h"
#include <ipc/spin_lock.h>
#include <mkrtos/mem.h>

#define SHARE_MEM_SIZE 2048

/**
* @breif �����ڴ�
*/
static uint8_t *share_mem=NULL;

static spinlock_handler share_mem_lock_handl;

/**
* @breif ����洢������ֲ���Ҫʹ�ô��ڴ�ʱʹ��malloc�������ܽ��ͣ���ֱ�Ӷ��嵼��ջ���
*/
int32_t share_mem_init(void){
    share_mem=OSMallocEx(SHARE_MEM_SIZE);
	if(share_mem==NULL){
		return -1;
	}
	spin_lock_init(&share_mem_lock_handl);
	return 0;
}
/**
* @breif �����ڴ���
*/
uint8_t* share_mem_lock(uint32_t hope_size){
	if(hope_size>SHARE_MEM_SIZE){
		return NULL;
	}
	spin_lock(&share_mem_lock_handl);
	return share_mem;
}
/**
* @breif �����ڴ����
*/
void share_mem_unlock(void){
	spin_unlock(&share_mem_lock_handl);
}

