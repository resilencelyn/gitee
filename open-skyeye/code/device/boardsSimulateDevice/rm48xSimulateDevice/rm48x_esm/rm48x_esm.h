#ifndef __RM48X_ESM_H__ 
#define __RM48X_ESM_H__ 
struct registers 
{
	
	uint32_t esmeepapr1;
	
	uint32_t esmdepapr1;
	
	uint32_t esmiesr1;
	
	uint32_t esmiecr1;
	
	uint32_t esmilsr1;
	
	uint32_t esmilcr1;
	
	uint32_t esmsr1;
	
	uint32_t esmsr2;
	
	uint32_t esmsr3;
	
	uint32_t esmepsr;
	
	uint32_t esmioffhr;
	
	uint32_t esmiofflr;
	
	uint32_t esmltcr;
	
	uint32_t esmltcpr;
	
	uint32_t esmekr;
	
	uint32_t esmssr2;
	
	uint32_t esmiepsr4;
	
	uint32_t esmiepcr4;
	
	uint32_t esmiesr4;
	
	uint32_t esmiecr4;
	
	uint32_t esmilsr4;
	
	uint32_t esmilcr4;
	
	uint32_t esmsr4;
	
};


struct rm48x_esm_device_t 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;
	
	struct registers regs;
	
};
typedef struct rm48x_esm_device_t rm48x_esm_device;
static char* regs_name[] = 
{
	"ESMEEPAPR1",
	"ESMDEPAPR1",
	"ESMIESR1",
	"ESMIECR1",
	"ESMILSR1",
	"ESMILCR1",
	"ESMSR1",
	"ESMSR2",
	"ESMSR3",
	"ESMEPSR",
	"ESMIOFFHR",
	"ESMIOFFLR",
	"ESMLTCR",
	"ESMLTCPR",
	"ESMEKR",
	"ESMSSR2",
	"ESMIEPSR4",
	"ESMIEPCR4",
	"ESMIESR4",
	"ESMIECR4",
	"ESMILSR4",
	"ESMILCR4",
	"ESMSR4",
	NULL
};
#define ESMEEPAPR1 0x00
#define ESMDEPAPR1 0x04
#define ESMIESR1 0x08
#define ESMIECR1 0x0c
#define ESMILSR1 0x10
#define ESMILCR1 0x14
#define ESMSR1 0x18
#define ESMSR2 0x1c
#define ESMSR3 0x20
#define ESMEPSR 0x24
#define ESMIOFFHR 0x28
#define ESMIOFFLR 0x2c
#define ESMLTCR 0x30
#define ESMLTCPR 0x34
#define ESMEKR 0x38
#define ESMSSR2 0x3c
#define ESMIEPSR4 0x40
#define ESMIEPCR4 0x44
#define ESMIESR4 0x48
#define ESMIECR4 0x4c
#define ESMILSR4 0x50
#define ESMILCR4 0x54
#define ESMSR4 0x58
#endif  
