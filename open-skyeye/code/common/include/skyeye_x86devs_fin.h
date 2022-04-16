#ifndef __X86_FAULT_IN_H__
#define __X86_FAULT_IN_H__
#include <skyeye_fault_injection.h>

typedef struct fi_type{
	unsigned int addr;
	int bitnum;
	int mode;
}xfi_t;

#define FIN_NUM 20
#define DEVS_NUM 20
#define NO_ADDR -1
#define NO_FI -1

typedef struct fi_queue_t{
	int count;
	unsigned int fi[FIN_NUM][2];
}fi_q_t;
enum{
	THR = 0,
	IER,
	IIR,
	LCR,
	MCR,
	LSR,
	MSR,
	SCR
}; 

enum{
f_and = 0,
f_or,
f_xor,
};

int x86_devs_r_plug(int d_id, unsigned int addr, int buf,unsigned int count);
void x86_devs_save_fin(int d_id,unsigned int addr, int bitnum, int mode);
fi_q_t *x86_devs_get_fin(int d_id);
void x86_devs_clear_fin(int d_id, unsigned int addr, int bitnum, int mode);

char *x86_devs_get_reg_name_by_id(int d_id, int r_id);
unsigned int x86_devs_get_reg_width(int d_id);
unsigned int x86_devs_get_reg_num(int d_id);
unsigned int x86_devs_get_reg_addr(int d_id, int r_id);
unsigned int x86_devs_get_reg_value_by_id(int d_id, int r_id);
unsigned int x86_devs_get_reg_large_value_by_id(int d_id, int r_id, int offset);

char *x86_devs_get_devs_name(int d_id);
unsigned int x86_devs_get_devs_num(void);

#endif
