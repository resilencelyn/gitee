#ifndef __PIC_MPC837x_H__
#define __PIC_MPC837x_H__

struct registers {
	uint32_t sicfr;     //System global interrupt configuration register (SICFR) R/W
	uint32_t sivcr;     //System regular interrupt vector register (SIVCR) R
	uint32_t sipnr_h;   //System internal interrupt pending register (SIPNR_H) R
	uint32_t sipnr_l;   //System internal interrupt pending register (SIPNR_L) R
	uint32_t siprr_a;   //System internal interrupt group A priority register (SIPRR_A) R/W
	uint32_t siprr_b;   //System internal interrupt group B priority register (SIPRR_B) R/W
	uint32_t siprr_c;   //System internal interrupt group C priority register (SIPRR_C) R/W
	uint32_t siprr_d;   //System internal interrupt group D priority register (SIPRR_D) R/W
	uint32_t simsr_h;   //System internal interrupt mask register (SIMSR_H) R/W
	uint32_t simsr_l;   //System internal interrupt mask register (SIMSR_L) R/W
	uint32_t sicnr;     //System internal interrupt control register (SICNR) R/W
	uint32_t sepnr;     //System external interrupt pending register (SEPNR) R/W
	uint32_t smprr_a;   //System mixed interrupt group A priority register (SMPRR_A) R/W
	uint32_t smprr_b;   //System mixed interrupt group B priority register (SMPRR_B) R/W
	uint32_t semsr;     //System external interrupt mask register (SEMSR) R/W
	uint32_t secnr;     //System external interrupt control register (SECNR) R/W
	uint32_t sersr;     //System error status register (SERSR) R/W
	uint32_t sermr;     //System error mask register (SERMR) R/W
	uint32_t sercr;     //System error control register (SERCR) R/W
	uint32_t sepcr;     //System external interrupt polarity control register (SEPCR)  R/W
	uint32_t sifcr_h;   //System internal interrupt force register (SIFCR_H) R/W
	uint32_t sifcr_l;   //System internal interrupt force register (SIFCR_L) R/W
	uint32_t sefcr;     //System external interrupt force register (SEFCR) R/W
	uint32_t serfr;     //System error force register (SERFR) R/W
	uint32_t scvcr;     //System critical interrupt vector register (SCVCR) R
	uint32_t smvcr;     //System management interrupt vector register (SMVCR) R
};

typedef struct mpc837x_device {
	conf_object_t* obj;
	core_signal_intf* signal_iface;
	conf_object_t* signal;
	struct registers regs;
	uint8_t iivpr_dr[1400];
	FIFO *pic_fifo;
}pic_mpc837x_device;

static char* regs_name[] = {
	"sicfr",
	"sivcr",
	"sipnr_h",
	"sipnr_l",
	"siprr_a",
	"siprr_b",
	"siprr_c",
	"siprr_d",
	"simsr_h",
	"simsr_l",
	"sicnr",
	"sepnr",
	"smprr_a",
	"smprr_b",
	"semsr",
	"secnr",
	"sersr",
	"sermr",
	"sercr",
	"sepcr",
	"sifcr_h",
	"sifcr_l",
	"sefcr",
	"serfr",
	"scvcr",
	"smvcr",
	NULL
};
#define SICFR       0x00
#define SIVCR       0x04
#define SIPNR_H     0x08
#define SIPNR_L     0x0c
#define SIPRR_A     0x10
#define SIPRR_B     0x14
#define SIPRR_C     0x18
#define SIPRR_D     0x1c
#define SIMSR_H     0x20
#define SIMSR_L     0x24
#define SICNR       0x28
#define SEPNR       0x2c
#define SMPRR_A     0x30
#define SMPRR_B     0x34
#define SEMSR       0x38
#define SECNR       0x3c
#define SERSR       0x40
#define SERMR       0x44
#define SERCR       0x48
#define SEPCR       0x4c
#define SIFCR_H     0x50
#define SIFCR_L     0x54
#define SEFCR       0x58
#define SERFR       0x5c
#define SCVCR       0x60
#define SMVCR       0x64

#endif
