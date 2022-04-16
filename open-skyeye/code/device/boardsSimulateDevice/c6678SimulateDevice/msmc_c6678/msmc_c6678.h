#ifndef __MSMC_C6678_H__
#define __MSMC_C6678_H__

struct registers
{
	uint32_t pid;
	uint32_t smcerrar;
	uint32_t smcerrxr;
	uint32_t smedcc;
	uint32_t smcea;
	uint32_t smsecc;
	uint32_t smpfar;
	uint32_t smpfxr;
	uint32_t smpfr;
	uint32_t smpfcr;
	uint32_t sbndc0;
	uint32_t sbndc1;
	uint32_t sbndc2;
	uint32_t sbndc3;
	uint32_t sbndc4;
	uint32_t sbndc5;
	uint32_t sbndc6;
	uint32_t sbndc7;
	uint32_t sbndm;
	uint32_t sbnde;
	uint32_t cfglck;
	uint32_t cfgulck;
	uint32_t cfglckstat;
	uint32_t sms_mpax_lck;
	uint32_t sms_mpax_ulck;
	uint32_t sms_mpax_lckstat;
	uint32_t ses_mpax_lck;
	uint32_t ses_mpax_ulck;
	uint32_t ses_mpax_lckstat;
	uint32_t smestat;
	uint32_t smirstat;
	uint32_t smirc;
	uint32_t smiestat;
	uint32_t smiec;
	uint32_t smncerrar;
	uint32_t smncerrxr;
	uint32_t smncea;
};


struct c6678_device
{
	conf_object_t* obj;
	core_signal_intf* signal_iface;

	conf_object_t* signal;

	struct registers regs;
	uint32_t sms_mpaxl[128];
	uint32_t sms_mpaxh[128];
	uint32_t ses_mpaxl[128];
	uint32_t ses_mpaxh[128];
};

typedef struct c6678_device msmc_c6678_device;

static char* regs_name[] =
{
	"pid",
	"smcerrar",
	"smcerrxr",
	"smedcc",
	"smcea",
	"smsecc",
	"smpfar",
	"smpfxr",
	"smpfr",
	"smpfcr",
	"sbndc0",
	"sbndc1",
	"sbndc2",
	"sbndc3",
	"sbndc4",
	"sbndc5",
	"sbndc6",
	"sbndc7",
	"sbndm",
	"sbnde",
	"cfglck",
	"cfgulck",
	"cfglckstat",
	"sms_mpax_lck",
	"sms_mpax_ulck",
	"sms_mpax_lckstat",
	"ses_mpax_lck",
	"ses_mpax_ulck",
	"ses_mpax_lckstat",
	"smestat",
	"smirstat",
	"smirc",
	"smiestat",
	"smiec",
	"smncerrar",
	"smncerrxr",
	"smncea",
	NULL
};

#define PID					0x00
#define SMCERRAR			0x08
#define SMCERRXR			0x0c
#define SMEDCC				0x10
#define SMCEA				0x14
#define SMSECC				0x18
#define SMPFAR				0x1c
#define SMPFXR				0x20
#define SMPFR 				0x24
#define SMPFCR				0x28
#define SBNDC0				0x30
#define SBNDC1				0x34
#define SBNDC2				0x38
#define SBNDC3				0x3c
#define SBNDC4				0x40
#define SBNDC5				0x44
#define SBNDC6				0x48
#define SBNDC7				0x4c
#define SBNDM				0x50
#define SBNDE				0x54
#define CFGLCK				0x5c
#define CFGULCK				0x60
#define CFGLCKSTAT			0x64
#define SMS_MPAX_LCK		0x68
#define SMS_MPAX_ULCK		0x6c
#define SMS_MPAX_LCKSTAT	0x70
#define SES_MPAX_LCK		0x74
#define SES_MPAX_ULCK		0x78
#define SES_MPAX_LCKSTAT	0x7c
#define SMESTAT				0x80
#define SMIRSTAT			0x84
#define SMIRC				0x88
#define SMIESTAT			0x8c
#define SMIEC				0x90
#define SMNCERRAR			0xc4
#define SMNCERRXR			0xc8
#define SMNCEA				0xcc
#endif
