#ifndef __VIC_at91rm9200_H__
#define __VIC_at91rm9200_H__

#define AIC_SMR         (0x0000)	// Source Mode Register
#define AIC_SVR         (0x0080)	// Source Vector Register
#define AIC_IVR         (0x0100)	// IRQ Vector Register
#define AIC_FVR         (0x0104)	// FIQ Vector Register
#define AIC_ISR         (0x0108)	// Interrupt Status Register
#define AIC_IPR         (0x010c)	// Interrupt Pending Register
#define AIC_IMR         (0x0110)	// Interrupt Mask Register
#define AIC_CISR        (0x0114)	// Core Interrupt Status Register
#define AIC_IECR        (0x0120)	// Interrupt Enable Command Register
#define AIC_IDCR        (0x0124)	// Interrupt Disable Command Register
#define AIC_ICCR        (0x0128)	// Interrupt Clear Command Register
#define AIC_ISCR        (0x012c)	// Interrupt Set Command Register
#define AIC_EOICR       (0x0130)	// End of Interrupt Command Register
#define AIC_SPU         (0x0134)	// Spurious Vector Register
#define AIC_DCR         (0x0138)	// Debug Control Register (Protect)
#define AIC_FFER        (0x0140)	// Fast Forcing Enable Register
#define AIC_FFDR        (0x0144)	// Fast Forcing Disable Register
#define AIC_FFSR        (0x0148)	// Fast Forcing Status Register

typedef struct at91rm9200_device {
	conf_object_t *obj;
	struct registers{
		uint32_t aic_smr[32];
		uint32_t aic_svr[32];
		uint32_t aic_ivr;
		uint32_t aic_fvr;
		uint32_t aic_isr;
		uint32_t aic_ipr;
		uint32_t aic_imr;
		uint32_t aic_cisr;
		uint32_t aic_iecr;
		uint32_t aic_idcr;
		uint32_t aic_iccr;
		uint32_t aic_iscr;
		uint32_t aic_eoicr;
		uint32_t aic_spu;
		uint32_t aic_dcr;
		uint32_t aic_ffer;
		uint32_t aic_ffdr;
		uint32_t aic_ffsr;
	}regs;
	core_signal_intf *signal_iface;
	conf_object_t *signal;
}vic_at91rm9200_device;

static char* regs_name[] = {
	"aic_smr[0]",
	"aic_smr[1]",
	"aic_smr[2]",
	"aic_smr[3]",
	"aic_smr[4]",
	"aic_smr[5]",
	"aic_smr[6]",
	"aic_smr[7]",
	"aic_smr[8]",
	"aic_smr[9]",
	"aic_smr[10]",
	"aic_smr[11]",
	"aic_smr[12]",
	"aic_smr[13]",
	"aic_smr[14]",
	"aic_smr[15]",
	"aic_smr[16]",
	"aic_smr[17]",
	"aic_smr[18]",
	"aic_smr[19]",
	"aic_smr[20]",
	"aic_smr[21]",
	"aic_smr[22]",
	"aic_smr[23]",
	"aic_smr[24]",
	"aic_smr[25]",
	"aic_smr[26]",
	"aic_smr[27]",
	"aic_smr[28]",
	"aic_smr[29]",
	"aic_smr[30]",
	"aic_smr[31]",

	"aic_svr[0]",
	"aic_svr[1]",
	"aic_svr[2]",
	"aic_svr[3]",
	"aic_svr[4]",
	"aic_svr[5]",
	"aic_svr[6]",
	"aic_svr[7]",
	"aic_svr[8]",
	"aic_svr[9]",
	"aic_svr[10]",
	"aic_svr[11]",
	"aic_svr[12]",
	"aic_svr[13]",
	"aic_svr[14]",
	"aic_svr[15]",
	"aic_svr[16]",
	"aic_svr[17]",
	"aic_svr[18]",
	"aic_svr[19]",
	"aic_svr[20]",
	"aic_svr[21]",
	"aic_svr[22]",
	"aic_svr[23]",
	"aic_svr[24]",
	"aic_svr[25]",
	"aic_svr[26]",
	"aic_svr[27]",
	"aic_svr[28]",
	"aic_svr[29]",
	"aic_svr[30]",
	"aic_svr[31]",

	"aic_ivr",
	"aic_fvr",
	"aic_isr",
	"aic_ipr",
	"aic_imr",
	"aic_cisr",
	"aic_iecr",
	"aic_idcr",
	"aic_iccr",
	"aic_iscr",
	"aic_eoicr",
	"aic_spu",
	"aic_dcr",
	"aic_ffer",
	"aic_ffdr",
	"aic_ffsr",
	NULL,
};
#endif
