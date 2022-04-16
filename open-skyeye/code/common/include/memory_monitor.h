#ifndef __MEMORY_MONITOR__
#define __MEMORY_MONITOR__
#include "skyeye_types.h"

#define MO_READ_OP       0x1  // 读操作监视 
#define MO_WRITE_OP      0x2  // 写操作监视
#define MO_COND_OP       0x4  // 条件监视
#define MO_ALL_OP        0x7  // 读/写/条件
#define MO_C28X_OP       0x10 //C28特殊架构

#define MO_VALUE(v) (void *)(&(v))

typedef enum{
	OP_INT8,
	OP_UINT8,
	OP_INT16,
	OP_UINT16,
	OP_INT32,
	OP_UINT32,
	OP_INT64,
	OP_UINT64,
	OP_FLOAT,
	OP_DOUBLE,
} OP_DataType_t;

typedef enum{
	Cond_EQ,       // ==
	Cond_NEQ,      // !=
	Cond_GT,       // >
	Cond_LT,       // <
	Cond_GTE,      // >=
	Cond_LTE,      // <=
	No_Cond,       // 无条件监视
	Cond_Error,    // 错误
} cond_compare_t;

typedef struct {
	cond_compare_t     comp_flag;
	union {
		int8_t   int8_v;
		uint8_t  uint8_v;
		int16_t  int16_v;
		uint16_t uint16_v;
		int32_t  int32_v;
		uint32_t uint32_v;
		int64_t  int64_v;
		uint64_t uint64_v;
		float    fl32_v;
		double   fl64_v;
	} value;
} logic_op_exp_t;

typedef void (*memory_callback_t)(conf_object_t *conf_obj, uint32_t mm_type, uint32_t addr, void * val, size_t count);
typedef void (*py_memory_callback_t)(void *user_data);

typedef struct {
	conf_object_t   * conf_obj;

	uint32_t          mm_type;   // 监视类型
	logic_op_exp_t    logic_exp;

	OP_DataType_t     type;
	uint32_t          addr;
	uint64_t          curr_hex_value;

	uint32_t          data_width;
	uint32_t          data_read_width;
	uint32_t          data_write_width;

	memory_callback_t read_callback_ptr;
	memory_callback_t write_callback_ptr;
	memory_callback_t cond_callback_ptr;
	
	py_memory_callback_t py_read_callback_ptr;
	py_memory_callback_t py_write_callback_ptr;

	uint32_t          addr_access_lock;
	void *py_value[2];
} memory_addr_info_t;

typedef void ***** mm_fast_map;

#ifdef __cplusplus
extern "C" void memory_read_hit_pgd_item(mm_fast_map pgd, uint32_t addr, void * val, size_t count);
extern "C" void memory_write_hit_pgd_item(mm_fast_map pgd, uint32_t addr, void * val, size_t count);

extern "C" mm_fast_map create_memory_pgd();
extern "C" mm_fast_map insert_memory_pgd_intem(mm_fast_map pgd, unsigned addr, memory_addr_info_t *inf);
extern "C" int         clear_memory_pgd_item(mm_fast_map pgd, unsigned addr);

/*
 * cpu_name:   CPU名称
 * m_type:     监视类型
 * m_addr:     监视地址
 * data_type:  数据类型
 * cond:       条件类型
 * value:      条件值, 无条件监视类型忽略
 * callback    监视回调函数
 */
extern "C" int SkyEye_Memory_Monitor(conf_object_t *conf_obj, char *cpu_name, int mm_type, uint32_t m_addr, \
    OP_DataType_t data_type, cond_compare_t cond, void *value, memory_callback_t callback);
extern "C" int Py_SetWatchOnMemory(conf_object_t *conf_obj, char *ms_name, int mm_type, uint32_t m_addr, \
	uint32_t data_type, uint32_t m_length, py_memory_callback_t callback, void *user_data);

extern "C" int SkyEye_Memory_Monitor_Remove(char *cpu_name, int mm_type, uint32_t m_addr);
#else

void memory_read_hit_pgd_item(mm_fast_map pgd, uint32_t addr, void * val, size_t count);
void memory_write_hit_pgd_item(mm_fast_map pgd, uint32_t addr, void * val, size_t count);

mm_fast_map create_memory_pgd();
mm_fast_map insert_memory_pgd_intem(mm_fast_map pgd, unsigned addr, memory_addr_info_t *inf);
int         clear_memory_pgd_item(mm_fast_map pgd, unsigned addr);

/*
 * cpu_name:   CPU名称
 * m_type:     监视类型
 * m_addr:     监视地址
 * data_type:  数据类型
 * cond:       条件类型
 * value:      条件值, 无条件监视类型忽略
 * callback    监视回调函数
 */
int SkyEye_Memory_Monitor(conf_object_t *conf_obj, char *cpu_name, int mm_type, uint32_t m_addr, \
    OP_DataType_t data_type, cond_compare_t cond, void *value, memory_callback_t callback);
	
int Py_SetWatchOnMemory(conf_object_t *conf_obj, char *ms_name, int mm_type, uint32_t m_addr, \
	uint32_t data_type, uint32_t m_length, py_memory_callback_t callback, void *user_data);
	
int SkyEye_Memory_Monitor_Remove(char *cpu_name, int mm_type, uint32_t m_addr);
void SkyEye_SetDyncomWatchMemoryStatus(uint32_t addr, int status);
#endif

#endif
