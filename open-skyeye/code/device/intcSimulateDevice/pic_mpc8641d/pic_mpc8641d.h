#ifndef __PIC_MPC8641D_H__ 
#define __PIC_MPC8641D_H__ 
struct registers 
{
	
};

typedef struct pic_global_s{
	uint32 gcr;
	uint32 frr;
	uint32 tfrr;
	uint32 gtdr0;
	uint32 gtdr1;
	uint32 gtdr2;
	uint32 gtdr3;
	uint32 gtvpr0;
	uint32 gtvpr1;
	uint32 gtvpr2;
	uint32 gtvpr3;
	uint32 gtbcra0; /* for vxworks */
	uint32 gtbcra1; /* for vxworks */
	uint32 gtbcra2; /* for vxworks */
	uint32 gtbcra3; /* for vxworks */

	uint32 gtccra0; /* for vxworks */
	uint32 gtccra1; /* for vxworks */
	uint32 gtccra2; /* for vxworks */
	uint32 gtccra3; /* for vxworks */

	uint32 svr;
	uint32 iack;
	uint32 mer;  /* for vxworks */
	uint32 tcra;  /* for vxworks */
	uint32 prr;
}pic_global_t;

typedef struct pic_ram_s{
	uint32 eivpr[11];
	uint32 eidr[11];
	uint32 ctpr0, ctpr1;
	//uint32 iivpr[32];
	uint32 iivpr[64];
	//uint32 iidr[32];
	uint32 iidr[64];
	uint32 eoi[2];
}pic_ram_t;

typedef struct pic_percpu_s{
	uint32 iack[2];
}pic_percpu_t;

typedef struct mpic_s{
	uint32 gcr; /* Global configuration register */
	uint32 ipivpr[4]; /* IPI 0-3 vector/priority register */
	uint32 ipidr[4]; /* IPI 0 - 3 dispatch register */
	uint32 iivpr[64];
	uint32 iidr[64];

	uint32 mivpr[4];
	uint32 midr[4];
	uint32 msivpr[8];
	uint32 msidr[8];

}mpic_t;


struct mpc8641d_device 
{
	
	conf_object_t* obj;
	
	struct registers regs;

	pic_global_t pic_global;
	pic_ram_t pic_ram;
	pic_percpu_t pic_percpu;
	mpic_t mpic;

	conf_object_t *cpu;
	uint32_t uart_signal;
	int irq_num;
};
typedef struct mpc8641d_device pic_mpc8641d_device;
static char* regs_name[] = 
{
	NULL
};
#endif  
