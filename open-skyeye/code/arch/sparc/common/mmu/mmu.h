#ifndef __SPARC_MMU_H__
#define __SPARC_MMU_H__
#include <skyeye_types.h>

#if 0
typedef struct sparc_mmu_s
{
    uint32_t contrl_reg;
    uint32_t ctp_reg;
    uint32_t context_reg;
    uint32_t fs_reg;
    uint32_t fa_reg;
} sparc_mmu_t;
#endif

/* composition sparc virtual address */
#define VA_L3_OFF(va)	(va & 0xfff)
#define VA_L2_OFF(va)	(va & 0x3ffff)
#define VA_L1_OFF(va)	(va & 0xffffff)
#define VA_L3(va)	((va >> 12) & 0x3f)
#define VA_L2(va)	((va >> 18) & 0x3f)
#define VA_L1(va)	((va >> 24) & 0xff)

/* composition sparc PTD */
#define PTD_ET(pte)	((pte >> 0) & 0x3)
#define PTD_PTP(pte)	((pte >> 2) & 0x3fffffff)

/* composition sparc PTE */
#define PTE_ET(pte)	((pte >> 0) & 0x3)
#define PTE_ACC(pte)	((pte >> 2) & 0x7)
#define PTE_R(pte)	((pte >> 5) & 0x1)
#define PTE_M(pte)	((pte >> 6) & 0x1)
#define PTE_C(pte)	((pte >> 7) & 0x1)
#define PTE_PPN(pte)	((pte >> 8) & 0xffffff)

#define PTE_CV		0x80
#define PTE_MV		0x40
#define PTE_RV		0x20
#define PTE_EXECV	0x08
#define PTE_WRITEV	0x04

typedef struct mmu_ops_s
{
    /*initilization */
    exception_t(*init) (void);
    /*free on exit */
    void (*exit) (void);
    /*translate vaddr -> paddr */
      exception_t(*translate) (conf_object_t * conf_obj, uint32_t virt_addr, uint32_t * phy_addr);
    /*read data by size */
      exception_t(*read) (short size, generic_address_t va, uint32_t * data);
    /*write data by size */
      exception_t(*write) (short size, generic_address_t va, uint32_t data);
    /*read byte data */
      exception_t(*read_byte) (conf_object_t * conf_obj, uint32_t va, uint32_t * data);
    /*write byte data */
      exception_t(*write_byte) (conf_object_t * conf_obj, uint32_t va, uint32_t data);
    /*read halfword data */
      exception_t(*read_halfword) (conf_object_t * conf_obj, uint32_t va, uint32_t * data);
    /*write halfword data */
      exception_t(*write_halfword) (conf_object_t * conf_obj, uint32_t va, uint32_t data);
    /*read word data */
      exception_t(*read_word) (conf_object_t * conf_obj, uint32_t va, uint32_t * data);
    /*write word data */
      exception_t(*write_word) (conf_object_t * conf_obj, uint32_t va, uint32_t data);
    /*load instr */
      exception_t(*load_instr) (conf_object_t * conf_obj, uint32_t va, uint32_t * instr);
    /* write mmu reg */
      exception_t(*write_reg) (conf_object_t * conf_obj, uint32_t offset, uint32_t val);
    /* read mmu reg */
      exception_t(*read_reg) (conf_object_t * conf_obj, uint32_t offset, uint32_t * val);
} mmu_ops_t;

#if 0
static sparc_mmu_t sparc_mmu;
#endif

#define CONTRAL_REG	0x00000000
#define CONTEXT_TP_REG	0x00000100
#define CONTEXT_REG	0x00000200
#define FAULT_STATUS_REG	0x00000300
#define FAULT_ADDRESS_REG	0x00000400

#if 0
#define MMU_CR		sparc_mmu.contrl_reg
#define MMU_CTPR	sparc_mmu.ctp_reg
#define MMU_CTTR	sparc_mmu.context_reg
#define MMU_FSR		sparc_mmu.fs_reg
#define MMU_FAR		sparc_mmu.fa_reg
#else
#define MMU_CR		sparc_state->ctrl
#define MMU_CTPR	sparc_state->ctp
#define MMU_CTTR	sparc_state->ctxt
#define MMU_FSR		sparc_state->fsr
#define MMU_FAR		sparc_state->far_reg
#endif

#define MMU_CR_E	MMU_CR & 0x1
#define MMU_CR_NF	(MMU_CR >> 1) & 0x1
#define MMU_CR_PSO	(MMU_CR >> 7) & 0x1

#define MMU_CTP		(MMU_CTPR >> 2)

#define MMU_FSR_L	((MMU_FSR >> 8) & 0x3)
#define MMU_FSR_AT	((MMU_FSR >> 5) & 0x7)
#define MMU_FSR_FT	((MMU_FSR >> 2) & 0x7)

#define Set_FSR_FT(x)	(MMU_FSR &= ~(0x7 << 2));	\
			(MMU_FSR |= (x << 2));
#define Set_FSR_L(x)	(MMU_FSR &= ~(0x3 << 8));	\
			(MMU_FSR |= (x << 8));

#endif
