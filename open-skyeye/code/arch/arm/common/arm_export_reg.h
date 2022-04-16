#ifndef _ARM_EXPORT_REG_H
#define _ARM_EXPORT_REG_H

/* export register list for gui */

enum arm_export_regno
{
    EX_R0 = 0,
    EX_R1,
    EX_R2,
    EX_R3,
    EX_R4,
    EX_R5,
    EX_R6,
    EX_R7,
    EX_R8,
    EX_R9,
    EX_R10,
    EX_R11,
    EX_R12,
    EX_R13,
    EX_LR,
    EX_R15,                                //PC,
    EX_CPSR_REG,
    EX_SPSR_REG,
    EX_MMU_Fault_Address,
    EX_MMU_CTRL,
    EX_MMU_TTBR0,
    EX_MMU_TTBR1,
    EX_MMU_TTBC,
    EX_REG_NUM
};

char *arm_export_regname[EX_REG_NUM] = {
    "R0",
    "R1",
    "R2",
    "R3",
    "R4",
    "R5",
    "R6",
    "R7",
    "R8",
    "R9",
    "R10",
    "R11",
    "R12",
    "SP",
    "LR",
    "R15",
    "Cpsr",
    "Spsr",
    "MMU_Fault_Address",
    "MMU_CTRL",
    "MMU_TTBR0",
    "MMU_TTBR1",
    "MMU_TTBC",
    NULL
};

#endif
