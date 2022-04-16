#ifndef __X86_CHECK_DEVICES_H__
#define __X86_CHECK_DEVICES_H__


typedef struct devices_des_struct{
	char *name;
	int regnum;
	int regwid;
	char**regname;
	unsigned int *regaddr;
	unsigned int (*func)(void *, unsigned int offset, unsigned int len);
	void *th;
	fi_q_t fi_q;
}devices_des;

typedef struct devices_list{
	int count;
	devices_des devs_d[20];
}devs_l;

devs_l *x86_devs_get_devices(void);
unsigned int x86_add_device(char *name, unsigned int (*r_func)(void *, unsigned int offset, unsigned int len), void *th, int reg_num, int reg_wid, char *regname[], unsigned int *regaddr);
void x86_list_devices(void);
#endif
