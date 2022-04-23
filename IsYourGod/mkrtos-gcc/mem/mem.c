/**
* @breif �ڴ����
* @author 1358745329@qq.com
*/

#include "arch/arch.h"
#include "errno.h"
#include "stdio.h"
#include <string.h>
#include <mkrtos/mem.h>
#include <mkrtos/task.h>

/**
* @breif �ڴ��
*/
MemType __attribute__((aligned(MALLOC_BLOCK0_SIZE))) mem0[MALLOC_MEM0_SIZE]={0};
/**
* @breif �ڴ�����
*/
MemTbType mem0ManTb[MALLOC_MANAGER0_TABLE]={0};


#ifndef NOT_USE_EX_MEM
uint8_t __attribute__((aligned(MALLOC_BLOCK0_SIZE))) malloc_ex_mem[MALLOC_EX_MEM_SIZE] __attribute__((section(".ext_sram")))={0};
MemTbType manager_ex_table[MALLOC_EX_MANAGER_TABLE] __attribute__((section(".ext_sram")))= { 0 };
#endif

/**
* @breif OS���ڴ������
*/
typedef struct _OSMemItem{
	
	/**
	* @breif �ڴ�
	*/
	MemType* mem;
	/**
	* @breif �ڴ�����
	*/
	MemTbType* memManTb;
	/**
	* @breif �ڴ��С
	*/
	uint32_t memSize;
	/**
	* @breif �ڴ������С
	*/
	uint32_t memManTbSize;
	/**
	* @breif ���С
	*/
	uint16_t memBlockSize;
	
	/**
	* @breif ʣ��Ŀ�����
	*/
	uint32_t freeBlockNum;
	
}*POSMemItem,OSMemItem;


typedef struct _OSMem{
	
	/**
	* @breif �ڴ������
	*/
	OSMemItem OSMemItemLs[MEM_AREA_NUM];
	
	/**
	* @breif �ڴ�������
	*/
	uint16_t memNum;
	
}*POSMem,OSMem;

static int mem_init_flag=0;
/**
* @breif �ڴ���������ʼ��
*/
OSMem osMem={
	.memNum=MEM_AREA_NUM,
	.OSMemItemLs[0].mem=mem0,
	.OSMemItemLs[0].memManTb=mem0ManTb,
	.OSMemItemLs[0].memSize=MALLOC_MEM0_SIZE,
	.OSMemItemLs[0].memManTbSize=MALLOC_MANAGER0_TABLE,
	.OSMemItemLs[0].memBlockSize=MALLOC_BLOCK0_SIZE,
	.OSMemItemLs[0].freeBlockNum = MALLOC_MANAGER0_TABLE,
	#ifndef NOT_USE_EX_MEM
	.OSMemItemLs[1].mem=malloc_ex_mem,
	.OSMemItemLs[1].memManTb=manager_ex_table,
	.OSMemItemLs[1].memSize=MALLOC_EX_MEM_SIZE,
	.OSMemItemLs[1].memManTbSize=MALLOC_EX_MANAGER_TABLE,
	.OSMemItemLs[1].memBlockSize=MALLOC_EX_BLOCK_SIZE,
	.OSMemItemLs[1].freeBlockNum = MALLOC_EX_MANAGER_TABLE,
	#endif
};



/**
* @breif ��ʼ���ڴ����
*/
void InitMem(void) {
    if(mem_init_flag){
        return ;
    }
	/*������е��ڴ������*/
	uint16_t i;
	for (i = 0; i < osMem.memNum; i++) {
		uint32_t j=0;
		for(j=0;j<osMem.OSMemItemLs[i].memManTbSize;j++){
			osMem.OSMemItemLs[i].memManTb[j]=0;
		}
		//memset(osMem.OSMemItemLs[i].memManTb, 0, osMem.OSMemItemLs[i].memManTbSize * sizeof(MemTbType));
	}
    mem_init_flag=1;
}
/**
* @breif �����ڴ�
* @param inxMem ����һ���ڴ��������
* @param size ��Ҫ������ڴ��С
*/
void* _Malloc(uint16_t inxMem, uint32_t size) {
	uint32_t i_need_block_num;//��Ҫ��block����
	uint32_t i;
	uint32_t find_block_num = 0;//�ҵ��ĿյĿ�
	uint32_t temp;
	uint8_t flag = 0;
	uint16_t bkSize;
	MemTbType* manager_table;
	MemType* malloc_mem;
    InitMem();
	if (inxMem >= osMem.memNum) {
		/*��������*/
		return NULL;
	}
	malloc_mem = osMem.OSMemItemLs[inxMem].mem;
	manager_table = osMem.OSMemItemLs[inxMem].memManTb;
	bkSize = osMem.OSMemItemLs[inxMem].memBlockSize;

	i_need_block_num = size / bkSize + ((size % bkSize == 0) ? 0 : 1);

	for (i = 0; i < osMem.OSMemItemLs[inxMem].memManTbSize;) {
		if (manager_table[i] == 0) {
			find_block_num++;
			if (find_block_num == i_need_block_num) {
				flag = 1;
				break;
			}
			i++;
		}
		else {
			find_block_num = 0;
			i += manager_table[i];
		}
	}
	if (flag != 1 || i >= osMem.OSMemItemLs[inxMem].memManTbSize) {//û���ҵ������߳�����
		return NULL;
	}
	i -= i_need_block_num - 1;
	for (temp = i; temp < i + i_need_block_num; temp++) {
		if (temp == i) {
			manager_table[i] = i_need_block_num;
		}
		else {
			manager_table[temp] = 1;
		}
	}
	osMem.OSMemItemLs[inxMem].freeBlockNum -= i_need_block_num;
//    printk("malloc %x.\n\n",(void*)(&(malloc_mem[bkSize * i])));
    return (void*)(&(malloc_mem[bkSize * i]));
}
/**
* @brief �ͷ�������ڴ�
* @param inxMem ����һ���ڴ����ͷ�
* @param mem_addr �ͷŵ��ڴ��׵�ַ
*/
int32_t _Free(uint16_t inxMem, void* mem_addr) {
    uint32_t i;
    if (mem_addr == NULL) { return -2; }
    InitMem();
	uint32_t free_size;
	uint16_t bkSize;
	MemTbType* manager_table;
	MemType* malloc_mem;
	uint32_t i_mem_offset;
	uint32_t i_manager_offset;

    if(mem_addr<osMem.OSMemItemLs[inxMem].mem
        ||mem_addr>(osMem.OSMemItemLs[inxMem].mem+osMem.OSMemItemLs[inxMem].memSize)){
        return -1;
    }

	malloc_mem = osMem.OSMemItemLs[inxMem].mem;
	manager_table = osMem.OSMemItemLs[inxMem].memManTb;
	bkSize = osMem.OSMemItemLs[inxMem].memBlockSize;

	i_mem_offset = (uint32_t)mem_addr - (uint32_t)malloc_mem;
	i_manager_offset = i_mem_offset / bkSize;
	if (i_manager_offset > osMem.OSMemItemLs[inxMem].memManTbSize) {
		return -1;
	}
	
	osMem.OSMemItemLs[inxMem].freeBlockNum += manager_table[i_manager_offset];

	free_size = manager_table[i_manager_offset];
	for (i = i_manager_offset; i < free_size + i_manager_offset; i++) {
		manager_table[i] = 0;
	}
    return 0;
}
uint32_t GetMemSize(uint16_t inxMem, void* mem_addr){
	if (mem_addr == NULL) { return 0; }
    InitMem();
	uint32_t free_size;
	uint16_t bkSize;
	MemTbType* manager_table;
	MemType* malloc_mem;
	uint32_t i_mem_offset;
	uint32_t i_manager_offset;
	malloc_mem = osMem.OSMemItemLs[inxMem].mem;
	manager_table = osMem.OSMemItemLs[inxMem].memManTb;
	bkSize = osMem.OSMemItemLs[inxMem].memBlockSize;

	i_mem_offset = (uint32_t)mem_addr - (uint32_t)malloc_mem;
	i_manager_offset = i_mem_offset / bkSize;
	uint32_t i;
	if (i_manager_offset > osMem.OSMemItemLs[inxMem].memManTbSize) {
		return 0;
	}
	
	return manager_table[i_manager_offset]*bkSize;
}
/**
* @breif ��ȡʣ����ڴ�
* @param inxMem �ڴ������
* @return ����ʣ������ֽ�
*/
uint32_t GetFreeMemory(uint8_t mem_no) {
    InitMem();
	/*ʣ���ڴ��С*/
	return osMem.OSMemItemLs[mem_no].freeBlockNum * osMem.OSMemItemLs[OS_USE_MEM_AREA_INX].memBlockSize;

}
uint32_t GetTotalMemory(uint8_t mem_no){
    InitMem();
	return osMem.OSMemItemLs[mem_no].memSize;
}
/**
* @breif �����ڴ�
* @param size ����Ĵ�С
* @return �������뵽���ڴ棬ʧ���򷵻�NULL
*/
void* OSMalloc(uint32_t size) {
	int32_t st=DisCpuInter();
	void* res = _Malloc(0, size);
    if(!res){
        res=_Malloc(1,size);
    }
	RestoreCpuInter(st);
//    if(res) {
//        memset(res, 0, size);
//    }
	return res;
}
#ifndef NOT_USE_EX_MEM
void* OSMallocEx(uint32_t size) {
	int32_t st=DisCpuInter();
	void* res = _Malloc(OS_USE_MEM_AREA_INX1, size);
	RestoreCpuInter(st);
	return res;
}
void OSFreeEx(void* mem) {
	int32_t st=DisCpuInter();
	_Free(OS_USE_MEM_AREA_INX1, mem);
	RestoreCpuInter(st);
}
void *OSReallocEx(void* mem,uint32_t size){
	int32_t st=DisCpuInter();
	void* res = _Malloc(OS_USE_MEM_AREA_INX1, size);
	if(res==NULL){
		RestoreCpuInter(st);
		return NULL;
	}
	memcpy(res,mem,GetMemSize(OS_USE_MEM_AREA_INX1,mem));
	OSFree(mem);
	RestoreCpuInter(st);
	return res;
}
#endif
void *OSRealloc(void *mem,uint32_t size){
	int32_t st=DisCpuInter();
	void* res = OSMalloc(size);
	if(!res){
		RestoreCpuInter(st);
		return NULL;
	}
	memcpy(res,mem,GetMemSize(OS_USE_MEM_AREA_INX,mem));
	OSFree(mem);
	RestoreCpuInter(st);
	return res;
}

/**
* @breif �ͷ�������ڴ�
* @param mem �ͷŵ��ڴ���׵�ַ
*/
void OSFree(void* mem) {
	int32_t st;
    int ret;

    if(!mem){
        return ;
    }
//    printk("free %x.\n\n",mem);
    st=DisCpuInter();
    ret=_Free(0, mem);
    if(ret<0){
        _Free(1, mem);
    }
	RestoreCpuInter(st);
}

static int32_t mem_add(void *mem,int32_t lenght,struct inode *inode,int ofst){
    struct mem_struct* tmp;
    tmp= OSMalloc(sizeof(struct mem_struct));
    if(tmp==NULL){
        return -1;
    }
    tmp->length=lenght;
    tmp->ofst=ofst;
    tmp->mem_start=mem;
    tmp->inode=inode;
    tmp->next=NULL;
    uint32_t  t;
    t=DisCpuInter();
    if(!CUR_TASK->mems){
        CUR_TASK->mems=tmp;
    }else{
        tmp->next=CUR_TASK->mems;
        CUR_TASK->mems=tmp;
    }
    RestoreCpuInter(t);
    return 0;
}
static void mem_remove(void *mem,int length){
    struct mem_struct *tmp;
    tmp=CUR_TASK->mems;
    struct mem_struct *prev=NULL;
    uint32_t t;
    t=DisCpuInter();
    while(tmp){
        if(tmp->mem_start ==mem) {
            if (prev==NULL) {
                //ɾ���ĵ�һ��
                CUR_TASK->mems=tmp->next;
                OSFree(tmp);
                break;
            }else{
                prev->next=tmp->next;
                break;
            }
        }
        prev=tmp;
        tmp=tmp->next;
    }
    RestoreCpuInter(t);
}
struct mem_struct * mem_get(void *start){
    struct mem_struct *tmp;
    tmp=CUR_TASK->mems;
    uint32_t t;
    t=DisCpuInter();
    while(tmp){
        if(tmp->mem_start ==start) {
            RestoreCpuInter(t);
            return tmp;
        }
        tmp=tmp->next;
    }
    RestoreCpuInter(t);
    return NULL;
}
/**
 * �������ռ�õ��ڴ棬�Ñ���exit�������{��
 */
void mem_clear(void){
    uint32_t t;
    struct mem_struct **tmp;
   // t=DisCpuInter();
    tmp=&CUR_TASK->mems;
    while(*tmp){
        struct mem_struct *next;
        next=(*tmp)->next;

        struct mem_struct *rel_mem;

        rel_mem=(*tmp);
        if(rel_mem->inode){
            if(rel_mem->inode->i_ops
               &&rel_mem->inode->i_ops->default_file_ops->mumap
                    ){
                rel_mem->inode->i_ops->default_file_ops->mumap(rel_mem->inode,rel_mem->mem_start,rel_mem->length);
                atomic_dec(&rel_mem->inode->i_used_count);
            }
        }
        OSFree(rel_mem->mem_start);
        OSFree(rel_mem);
        *tmp=next;
    }
    CUR_TASK->mems=NULL;
  //  RestoreCpuInter(t);
}
void* sys_mmap(void *start, size_t length, int prot, int flags,int fd, off_t offset){
    struct inode *inode=NULL;
    int alloc_len=length;
    if(start){
        return -1;
    }
    if(fd!=-1) {
        if (fd < 0 || fd >= NR_FILE) {
            printk("%s fp.\n", __FUNCTION__);
            return -EBADF;
        }
        if (CUR_TASK->files[fd].used == 0) {
            return -EINVAL;
        }
        inode = CUR_TASK->files[fd].f_inode;
        struct super_block  *sb;
        sb= CUR_TASK->files[fd].f_inode->i_sb;
        //������Ҫ����ĳ���
        alloc_len=(
                length/sb->s_bk_size+((length%sb->s_bk_size)?1:0)
                + ((offset%sb->s_bk_size)?1:0)//offset���û�иպö���bksize������Ҫ���һ��
                )*sb->s_bk_size;
    }

    void* res_mem = OSMalloc(alloc_len);
    if(!res_mem){
        return -1;
    }
    if(mem_add(res_mem,length,inode,offset)<0){
        OSFree(res_mem);
        return -ENOMEM;
    }
//    printk("mmap %x start:%x length:%d prot:%d flags:%x fd:%d ost:%d\r\n",res_mem,start,length,prot,flags,fd,offset);
    if(fd>=0) {
        if (inode->i_ops &&
            inode->i_ops->default_file_ops->mmap
                ) {
            int res = inode->i_ops->default_file_ops->mmap(inode, CUR_TASK->files + fd, res_mem, alloc_len, 0, offset);
            if (res < 0) {
                mem_remove(res_mem,length);
                return res;
            }
            //mmapһ�Σ����ü���������һ��
            //�û���mmap�󣬾Ϳ��Ե��ùر�����ļ���
            atomic_inc(&inode->i_used_count);
        }

    }

    return res_mem;
}
void sys_munmap(void *start, size_t length){
    struct mem_struct *mem;
    mem=mem_get(start);
    if(!mem){
        return ;
    }
    if(mem->inode){
        if(mem->inode->i_ops
        &&mem->inode->i_ops->default_file_ops->mumap
        ){
            mem->inode->i_ops->default_file_ops->mumap(mem->inode,start,length);
           puti(mem->inode);
        }
    }
    mem_remove(start,length);

    OSFree(start);
}
void *sys_mremap(void *__addr, size_t __old_len, size_t __new_len,
                 unsigned long __may_move){
    struct mem_struct *mem;
    mem=mem_get(__addr);
    if(!mem){
        return NULL;
    }
    if(mem->inode){
        void* newmem= OSMalloc(__new_len);
        if(newmem==NULL){
            return NULL;
        }
        //���ͷ��ļ�ӳ��
        if(mem->inode->i_ops
           &&mem->inode->i_ops->default_file_ops->mumap
                ){
            mem->inode->i_ops->default_file_ops->mumap(mem->inode,__addr,mem->length);
        }else{
            OSFree(newmem);
            return NULL;
        }

        //����ӳ��
        if (mem->inode->i_ops &&
                mem->inode->i_ops->default_file_ops->mmap
                ) {
            int res = mem->inode->i_ops->default_file_ops->mmap(mem->inode, NULL, (unsigned long)newmem, __new_len, 0, mem->ofst);
            if (res < 0) {
                OSFree(newmem);
                return NULL;
            }
        }
        OSFree(mem->mem_start);
        mem->mem_start=newmem;
        mem->length=__new_len;
        return newmem;
    }else{
        //����Ҫ����ӳ���ļ�
//        mem_remove(__addr,__old_len);
        void* newmem= OSRealloc(__addr,__new_len);
        if(newmem!=NULL){
            mem->length=__new_len;
        }
        return newmem;
    }
}