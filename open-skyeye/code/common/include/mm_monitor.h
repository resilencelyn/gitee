#ifndef __MM_MONITOR__
#define __MM_MONITOR__
#include "skyeye_types.h"

#define MM_BIT0 0x1
#define MM_BIT1 0x2
#define MM_BIT2 0x4
#define MM_BIT3 0x8
#define MM_BIT4 0x10
#define MM_BIT5 0x20
#define MM_BIT6 0x40
#define MM_BIT7 0x80


typedef enum{
	LOGICAL_OR = 0,
	LOGICAL_AND,
}logical_t;

typedef enum{
	MM_UINT32 = 0,
	MM_INT32,
	MM_UINT64,
	MM_INT64,
	MM_DOUBLE,
	MM_FLOAT,
}MMDataType_t;

typedef enum{
	 Equal= 0, //==
	 Greater,  //>
	 Less,     //<
	 GEqual,   //>=
	 LEqual,   //<=
}compare_t;


typedef struct{
	compare_t	comp_flag;
	unsigned long long v;
}logic_exp_t;



#define LOGIC_EXP_CNT 10

typedef void(*mm_callback_t)(void *addr_info, uint32_t addr, uint8_t val);
typedef struct{

	logic_exp_t     logic_exp[LOGIC_EXP_CNT];
	logical_t 	logic[LOGIC_EXP_CNT];
	bool_t 		exp_enable;

	unsigned char   bit_w;	

	MMDataType_t 	type;

	unsigned 	addr;

	mm_callback_t   callback_ptr;

	void 	        *vm_point;
	
	unsigned long long val_w; 
}addr_info_t;

typedef addr_info_t ***** pgd_t;
typedef addr_info_t **** pmd_t;
typedef addr_info_t *** pte_t;



pgd_t create_pgd();

void hit_pgd_item(pgd_t pgd, unsigned addr, uint8_t val);

addr_info_t *get_pgd_item(pgd_t pgd, unsigned addr);


void clear_pgd_item(pgd_t pgd, unsigned addr);


void insert_pgd_intem(pgd_t pgd, unsigned addr, addr_info_t *inf);

int get_items(char **its, char *exp);

void decode_cmd(addr_info_t *ptr, char **items);

bool_t verif_cmd(addr_info_t *ptr, unsigned long long val);
#endif
