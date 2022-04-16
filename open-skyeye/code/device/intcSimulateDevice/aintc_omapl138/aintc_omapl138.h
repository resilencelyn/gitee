#ifndef __AINTC_omapl138_H__
#define __AINTC_omapl138_H__
#include <stdint.h>

#define AINTC_REVID	0x000
#define AINTC_CR	0x004
#define AINTC_GER	0x010
#define AINTC_GNLR	0x01c
#define AINTC_SISR	0x020
#define AINTC_SICR	0x024
#define AINTC_EISR	0x028
#define AINTC_EICR	0x02c
#define AINTC_HIEISR	0x034
#define AINTC_HIEICR	0x038
#define AINTC_VBR	0x050
#define AINTC_VSR	0x054
#define AINTC_VNR	0x058
#define AINTC_GPIR	0x080
#define AINTC_GPVR	0x084

#define AINTC_SRSR1	0x200
#define AINTC_SRSR2	0x204
#define AINTC_SRSR3	0x208
#define AINTC_SRSR4	0x20c

#define AINTC_SECR1	0x280
#define AINTC_SECR2	0x284
#define AINTC_SECR3	0x288
#define AINTC_SECR4	0x28c

#define AINTC_ESR1	0x300
#define AINTC_ESR2	0x304
#define AINTC_ESR3	0x308
#define AINTC_ESR4	0x30c

#define AINTC_ECR1	0x380
#define AINTC_ECR2	0x384
#define AINTC_ECR3	0x388
#define AINTC_ECR4	0x38c

#define AINTC_CMR	0x400

#define AINTC_HIPIR1	0x900
#define AINTC_HIPIR2	0x904

#define AINTC_HINLR1	0x1100
#define AINTC_HINLR2	0x1104

#define AINTC_HIER	0x1500

#define AINTC_HIPVR1	0x1600
#define AINTC_HIPVR2	0x1604

typedef struct omapl138_device {
	conf_object_t *obj;
	struct registers{
		uint32_t aintc_revid;
		uint32_t aintc_cr;
		uint32_t aintc_ger;
		uint32_t aintc_gnlr;
		uint32_t aintc_sisr;
		uint32_t aintc_sicr;
		uint32_t aintc_eisr;
		uint32_t aintc_eicr;
		uint32_t aintc_hieisr;
		uint32_t aintc_hieicr;
		uint32_t aintc_vbr;
		uint32_t aintc_vsr;
		uint32_t aintc_vnr;
		uint32_t aintc_gpir;
		uint32_t aintc_gpvr;
		uint32_t aintc_srsr1;
		uint32_t aintc_srsr2;
		uint32_t aintc_srsr3;
		uint32_t aintc_srsr4;
		uint32_t aintc_secr1;
		uint32_t aintc_secr2;
		uint32_t aintc_secr3;
		uint32_t aintc_secr4;
		uint32_t aintc_esr1;
		uint32_t aintc_esr2;
		uint32_t aintc_esr3;
		uint32_t aintc_esr4;
		uint32_t aintc_ecr1;
		uint32_t aintc_ecr2;
		uint32_t aintc_ecr3;
		uint32_t aintc_ecr4;
		uint32_t aintc_cmr[26];
		uint32_t aintc_hipir1;
		uint32_t aintc_hipir2;
		uint32_t aintc_hinlr1;
		uint32_t aintc_hinlr2;
		uint32_t aintc_hier;
		uint32_t aintc_hipvr1;
		uint32_t aintc_hipvr2;
	}regs;
	core_signal_intf *signal_iface;
	conf_object_t *signal;
	uint32_t aintc_sysinter1;
	uint32_t aintc_sysinter2;
	uint32_t aintc_sysinter3;
	uint32_t aintc_sysinter4;
}aintc_omapl138_device;

static char* regs_name[] = {
	"aintc_cr",
	"aintc_ger",
	"aintc_gnlr",
	"aintc_sisr",
	"aintc_sicr",
	"aintc_eisr",
	"aintc_eicr",
	"aintc_hieisr",
	"aintc_hieicr",
	"aintc_vbr",
	"aintc_vsr",
	"aintc_vnr",
	"aintc_gpir",
	"aintc_gpvr",
	"aintc_srsr1",
	"aintc_srsr2",
	"aintc_srsr3",
	"aintc_srsr4",
	"aintc_secr1",
	"aintc_secr2",
	"aintc_secr3",
	"aintc_secr4",
	"aintc_esr1",
	"aintc_esr2",
	"aintc_esr3",
	"aintc_esr4",
	"aintc_ecr1",
	"aintc_ecr2",
	"aintc_ecr3",
	"aintc_ecr4",
	"aintc_cmr[0]",
	"aintc_cmr[1]",
	"aintc_cmr[2]",
	"aintc_cmr[3]",
	"aintc_cmr[4]",
	"aintc_cmr[5]",
	"aintc_cmr[6]",
	"aintc_cmr[7]",
	"aintc_cmr[8]",
	"aintc_cmr[9]",
	"aintc_cmr[10]",
	"aintc_cmr[11]",
	"aintc_cmr[12]",
	"aintc_cmr[13]",
	"aintc_cmr[14]",
	"aintc_cmr[15]",
	"aintc_cmr[16]",
	"aintc_cmr[17]",
	"aintc_cmr[18]",
	"aintc_cmr[19]",
	"aintc_cmr[20]",
	"aintc_cmr[21]",
	"aintc_cmr[22]",
	"aintc_cmr[23]",
	"aintc_cmr[24]",
	"aintc_cmr[25]",
	"aintc_hipir1",
	"aintc_hipir2",
	"aintc_hinlr1",
	"aintc_hinlr2",
	"aintc_hier",
	"aintc_hipvr1",
	"aintc_hipvr2",
	NULL,
};
#endif
